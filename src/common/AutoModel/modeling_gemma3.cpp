/// \file gemma3.cpp
/// \brief gemma3 class
/// \author FastFlowLM Team
/// \date 2025-09-03
/// \version 0.9.21
/// \note This is a source file for the gemma3 class

#include "AutoModel/modeling_gemma3.hpp"

/************              Gemma3 family            **************/
Gemma3::Gemma3(xrt::device* npu_device_inst) : AutoModel(npu_device_inst, "Gemma3") {}

void Gemma3::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    // model_type == gemma
    this->lm_engine = std::make_unique<gemma_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

    this->lm_engine->load_weights(*this->q4nx);

    //free the q4nx
    this->q4nx.reset();
    this->lm_engine->clear_context();
    this->setup_tokenizer(model_path);
    this->sampler.reset();

    sampler_config config;
    config.rep_penalty = 1.1;
    config.temperature = 0.6;
    config.top_p = 0.95;
    config.top_k = 10;
    config.rep_penalty_window = 1024;
    config.freq_penalty = 1.1;
    config.freq_penalty_window = 1024;
    this->set_sampler(config);
    for (size_t i = 0; i < PROFILER_TYPE_NUM; i++) {
        this->profiler_list[i].reset();
    }
}

void Gemma3::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string Gemma3::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool Gemma3::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
    
    this->profiler_list[TKOEN_ENCODE_TIME].start();
    std::string templated_text;
    if (input.messages.empty() && input.prompt.empty()) {
        header_print("WARNING", "No messages or prompt provided");
        return false;
    }
    if (!input.messages.empty()) { // already a formated messages, usually from REST API
        templated_text = this->apply_chat_template(input.messages);
    }
    else if (!input.prompt.empty()) { // a pure text, usually from the cli
        nlohmann::ordered_json messages;
        if (input.images.size() > 0) {
            nlohmann::ordered_json content;
            content["role"] = "user";
            content["content"] = input.prompt;
            content["images"] = nlohmann::ordered_json::array();
            for (int i = 0; i < input.images.size(); i++) {
                content["images"].push_back(input.images[i]);
            }
            messages.push_back(content);
        }
        else {
            messages.push_back({ {"role", "user"}, {"content", input.prompt} });
        }
        templated_text = this->apply_chat_template(messages);
    }
    // process all images

    bytes pixel_values;
    if (!input.messages.empty()) {
        int total_images = 0;
        for (auto& message : input.messages){
            nlohmann::ordered_json::array_t images = message.value("images", nlohmann::ordered_json::array());
            if (images.size() > 0){
                total_images += images.size();
            }
        }
        header_print("FLM", "Total images: " << total_images);
        // temporary solution
        if (total_images > 0){
            pixel_values.resize(3 * 896 * 896 * sizeof(bf16) * total_images);
            uint8_t* pixel_values_ptr = pixel_values.data();
            for (auto& message : input.messages){
                nlohmann::ordered_json::array_t images = message.value("images", nlohmann::ordered_json::array());
                for (auto& image : images){
                    std::string image_str = image.get<std::string>();
                    bytes image_rgb = load_image_base64(image_str);
                    buffer<bf16> pv = preprocess_image(image_rgb);
                    memcpy(pixel_values_ptr, pv.data(), pv.size() * sizeof(bf16));
                    pixel_values_ptr += pv.size() * sizeof(bf16);
                }
            }
        }
    }
    else { // from cli, typically only one image, typically a file path
        if (input.images.size() > 0){
            pixel_values.resize(3 * 896 * 896 * sizeof(bf16) * input.images.size());
            uint8_t* pixel_values_ptr = pixel_values.data();
            auto start_time = std::chrono::high_resolution_clock::now();
            for (auto& image : input.images){
                bytes image_rgb = load_image(image);
                if (image_rgb.size() == 0){
                    header_print("FLM", "Error: Could not load image: " << image);
                    header_print("FLM", "Please check if the file exists and is readable.");
                    continue;
                }
                
                buffer<bf16> pv = preprocess_image(image_rgb);
                if (pv.size() == 0){
                    header_print("FLM", "Error: Could not preprocess image: " << image);
                    header_print("FLM", "Please check if the image is valid.");
                    continue;
                }
                memcpy(pixel_values_ptr, pv.data(), pv.size() * sizeof(bf16));
                pixel_values_ptr += pv.size() * sizeof(bf16);
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            header_print("FLM", "Image loaded in " << duration.count() << "ms");
        }
    }
    std::vector<int> tokens = this->tokenizer->encode(templated_text);
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware
    void* payload = pixel_values.size() > 0 ? static_cast<void*>(&pixel_values) : nullptr;
    return this->_shared_insert(meta_info, tokens, payload);
}

std::string Gemma3::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    return this->_shared_generate(meta_info, length_limit, os);
}

std::string Gemma3::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}


