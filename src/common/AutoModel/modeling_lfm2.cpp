/// \file lfm2.cpp
/// \brief lfm2 class
/// \author FastFlowLM Team
/// \date 2025-09-04
/// \version 0.9.15
/// \note This is a source file for the lfm2 class

#include "AutoModel/modeling_lfm2.hpp"
#include "utils/utils.hpp"
#include "metrices.hpp"

/************              LFM2 family            **************/
LFM2::LFM2(xrt::device* npu_device_inst) : AutoModel(npu_device_inst, "LFM2") {}

void LFM2::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    // model_type == llama
    this->lm_engine = std::make_unique<lfm2_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

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

void LFM2::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
    this->tokenizer->is_doubled_encoded = true;
}

std::string LFM2::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool LFM2::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
    // preprocess
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

        messages.push_back({ {"role", "user"}, {"content", input.prompt} });
        templated_text = this->apply_chat_template(messages);
    }

    std::vector<int> tokens = this->tokenizer->encode(templated_text);
    
    // some models are very sensitive to this bos token, such as lfm2
    if (this->is_first_prompt == false) {
        tokens.erase(tokens.begin()); // remove bos token in multi round conversation
    }
    this->is_first_prompt = false; // always set to false if the insert is ever called

    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware
    if (this->total_tokens + tokens.size() >= this->MAX_L){
        header_print("WARNING", "Max length reached, stopping prefilling...");
        return false;
    }
    for (int token : tokens){
        this->token_history.push_back(token);
    }
    buffer<bf16> y;

    auto prefill_start_time = this->profiler_list[PREFILL_TIME].start();
    y = this->lm_engine->prefill(tokens, nullptr);


    auto prefill_end_time = this->profiler_list[PREFILL_TIME].stop(tokens.size());
    meta_info.prefill_duration = (uint64_t)time_utils::duration_ns(prefill_start_time, prefill_end_time).first;
    meta_info.prompt_tokens = tokens.size();
    this->total_tokens += tokens.size() + 1;
    if (this->total_tokens >= this->MAX_L){
        header_print("WARNING", "Max length reached, stopping prefilling...");
    }
    this->profiler_list[SAMPLING_TIME].start();
    this->last_token = this->sampler->sample(y);
    this->profiler_list[SAMPLING_TIME].stop(1);
    return true;
}


std::string LFM2::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os, std::function<bool()> is_cancelled) {
    return this->_shared_generate(meta_info, length_limit, os, is_cancelled);
}

std::string LFM2::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->generate(meta_info, length_limit, os);
}