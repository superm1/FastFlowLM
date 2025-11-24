/// \file deepseek.cpp
/// \brief deepseek class
/// \author FastFlowLM Team
/// \date 2025-09-01
/// \version 0.9.21
/// \note This is a source file for the deepseek class


#include "AutoModel/modeling_qwen3.hpp"


/************              Qwen3 family            **************/
Qwen3::Qwen3(xrt::device* npu_device_inst) : AutoModel(npu_device_inst, "Qwen3") {}

void Qwen3::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    // lm_config->model_type == qwen3
    this->lm_engine = std::make_unique<qwen_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

    this->lm_engine->load_weights(*this->q4nx);

    //free the q4nx
    this->q4nx.reset();
    this->lm_engine->clear_context();
    this->setup_tokenizer(model_path);
    this->sampler.reset();

    this->enable_think = true;

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

void Qwen3::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string Qwen3::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    inputs.extra_context["enable_thinking"] = this->enable_think;
    return this->chat_tmpl->apply(inputs);
}

bool Qwen3::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
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
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware

    return this->_shared_insert(meta_info, tokens);
}

std::string Qwen3::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    return this->_shared_generate(meta_info, length_limit, os);
}

std::string Qwen3::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}

/************              Qwen3_IT family            **************/
Qwen3_IT::Qwen3_IT(xrt::device* npu_device_inst) : AutoModel(npu_device_inst) {}

void Qwen3_IT::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);

    // lm_config->model_type == qwen3
    this->lm_engine = std::make_unique<qwen_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

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

void Qwen3_IT::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string Qwen3_IT::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool Qwen3_IT::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
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
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware

    return this->_shared_insert(meta_info, tokens);
}

std::string Qwen3_IT::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    return this->_shared_generate(meta_info, length_limit, os);
}

std::string Qwen3_IT::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}

/************              Qwen3_TK family            **************/
Qwen3_TK::Qwen3_TK(xrt::device* npu_device_inst) : AutoModel(npu_device_inst) {}

void Qwen3_TK::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);

    // lm_config->model_type == qwen3
    this->lm_engine = std::make_unique<qwen_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

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

void Qwen3_TK::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
    this->think_marker_id = this->tokenizer->encode("<think>")[0];
}

std::string Qwen3_TK::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool Qwen3_TK::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
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
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware

    return this->_shared_insert(meta_info, tokens);
}

std::string Qwen3_TK::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    std::string result;
    os << "<think>\n\n";
    result = this->_shared_generate(meta_info, length_limit, os);
    result = "<think>\n\n" + result;
    return result;
}

std::string Qwen3_TK::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}

/************              DeepSeek_r1_0528_8b family            **************/
DeepSeek_r1_0528_8b::DeepSeek_r1_0528_8b(xrt::device* npu_device_inst) : AutoModel(npu_device_inst) {}

void DeepSeek_r1_0528_8b::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    // model_type == llama
    this->lm_engine = std::make_unique<qwen_npu>(*this->lm_config, this->npu.get(), this->MAX_L);

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

void DeepSeek_r1_0528_8b::setup_tokenizer(std::string model_path) {
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string DeepSeek_r1_0528_8b::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    return this->chat_tmpl->apply(inputs);
}

bool DeepSeek_r1_0528_8b::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) {
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
    this->profiler_list[TKOEN_ENCODE_TIME].stop(tokens.size());
    // hardware

    return this->_shared_insert(meta_info, tokens);
}

std::string DeepSeek_r1_0528_8b::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    std::string result = this->_shared_generate(meta_info, length_limit, os);
    return result;
}

std::string DeepSeek_r1_0528_8b::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    return this->_shared_generate(meta_info, length_limit, os);
}