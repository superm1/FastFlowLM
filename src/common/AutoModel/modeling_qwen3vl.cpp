/// \file deepseek.cpp
/// \brief deepseek class
/// \author FastFlowLM Team
/// \date 2025-09-01
/// \version 0.9.21
/// \note This is a source file for the deepseek class


#include "AutoModel/modeling_qwen3vl.hpp"
#include "metrices.hpp"


/************              Qwen3VL family            **************/
Qwen3VL::Qwen3VL(xrt::device* npu_device_inst) : AutoModel(npu_device_inst, "Qwen3VL") {}

void Qwen3VL::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    // lm_config->model_type == qwen3
    this->lm_engine = std::make_unique<qwen3vl_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

    this->lm_engine->load_weights(*this->q4nx);
    //free the q4nx
    this->q4nx.reset();
    this->lm_engine->clear_context();
    this->setup_tokenizer(model_path);
    this->sampler.reset();

    sampler_config config;
    config.rep_penalty = 1.1;
    config.temperature = 0.8;
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

void Qwen3VL::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string Qwen3VL::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool Qwen3VL::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
    // preprocess
    constexpr int image_soft_token_id = 151655;
    this->profiler_list[TKOEN_ENCODE_TIME].start();
    std::string templated_text;
    if (input.messages.empty() && input.prompt.empty()) {
        header_print("WARNING", "No messages or prompt provided");
        return false;
    }

    constexpr bool DEBUG_IMAGE_PREPROCESS = false;
    qwen3vl_image_payload_t image_payload;
    image_payload.num_images = 0;
    if (input.images.size() > 0) {


        // header_print("info", "Processing images...");
        
        // time_utils::time_point preprocess_start = time_utils::now();
        for(const auto& img_str : input.images){
            qwen3vl_image_t image = this->load_image(img_str);
            
            preprocess_image(image, image_payload._data__processed);
            // Push the image AFTER preprocessing so grid_h and grid_w are set
            image_payload.images.push_back(image);
            image_payload.num_images++;
        } 
    }
    if (!input.messages.empty()) { // already a formated messages, usually from REST API
        json qwenvl_message = json::array();
        for (const auto& item : input.messages) {
            if (!item.contains("images")) {
                qwenvl_message.push_back(item);
                continue;
            }

            json newContent = json::array();
            for (const auto& img : item["images"]) {
                newContent.push_back({
                    {"type", "image"},
                    {"image", img}
                });
            }
            newContent.push_back({
                {"type", "text"},
                {"text", item["content"]}
            });

            json newItem = {
                {"role", item["role"]},
                {"content", newContent}
            };

            qwenvl_message.push_back(newItem);
        }
        templated_text = this->apply_chat_template(qwenvl_message);
        int total_images = 0;
        for (auto& message : qwenvl_message) {
            auto content = message.value("content", nlohmann::ordered_json::array());
            for (auto& item : content) {
                if (item.contains("type") && item["type"] == "image") {
                    std::string img_str = item.value("image", "");
                    if (!img_str.empty()) {
                        total_images++;
                    }
                    qwen3vl_image_t image = this->load_image_base64(img_str);
                    preprocess_image(image, image_payload._data__processed);
                    image_payload.images.push_back(image);
                    image_payload.num_images++;
                }
            }
        }
        header_print("FLM", "Total images: " << total_images);
    }
    else if (!input.prompt.empty()) { // a pure text, usually from the cli
        nlohmann::ordered_json messages;
        nlohmann::ordered_json content;
        content["role"] = "user";
        content["content"] = nlohmann::ordered_json::array();
        
        // Add image objects to content array
        for (int i = 0; i < input.images.size(); i++) {
            nlohmann::ordered_json image_obj;
            image_obj["type"] = "image";
            image_obj["image"] = input.images[i];
            content["content"].push_back(image_obj);
        }
        
        // Add text object to content array
        nlohmann::ordered_json text_obj;
        text_obj["type"] = "text";
        text_obj["text"] = input.prompt;
        content["content"].push_back(text_obj);
        
        messages.push_back(content);
        templated_text = this->apply_chat_template(messages);
    }
    std::vector<int> tokens_init = this->tokenizer->encode(templated_text);

    // update the tokens to include the image tokens
    std::vector<int> tokens;
    int total_image_tokens = 0;
    for (int i = 0; i < input.images.size(); i++) {
        total_image_tokens += image_payload.images[i].grid_h * image_payload.images[i].grid_w;
    }
    tokens.reserve(tokens_init.size() + total_image_tokens);
    int image_counter = 0;
    for (int i = 0; i < tokens_init.size(); i++) {
        if (tokens_init[i] == image_soft_token_id) {
            for (int j = 0; j < image_payload.images[image_counter].grid_h * image_payload.images[image_counter].grid_w / 4; j++) {
                tokens.push_back(image_soft_token_id);
            }
            image_counter++;
        } else {
            tokens.push_back(tokens_init[i]);
        }
    }
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware
    if (image_payload.num_images > 0){
        return this->_shared_insert(meta_info, tokens, &image_payload);
    }else{
        return this->_shared_insert(meta_info, tokens, nullptr);
    }

}

std::string Qwen3VL::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    return this->_shared_generate(meta_info, length_limit, os);
}

std::string Qwen3VL::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}

/************              Qwen3VL_Thinking            **************/

std::string Qwen3VL_Thinking::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->generate(meta_info, length_limit, os);
}


std::string Qwen3VL_Thinking::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    std::string result;
    os << "<think>\n\n";
    result = this->_shared_generate(meta_info, length_limit, os);
    result = "<think>\n\n" + result;
    return result;
}
