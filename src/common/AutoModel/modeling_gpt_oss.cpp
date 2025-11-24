/// \file modeling_gpt_oss.cpp
/// \brief modeling_gpt_oss class
/// \author FastFlowLM Team
/// \date 2025-10-01
/// \version 0.9.21
/// \note This is a source file for the gpt-oss class
#include "AutoModel/modeling_gpt_oss.hpp"   

GPT_OSS::GPT_OSS(xrt::device* npu_device_inst) : AutoModel(npu_device_inst, "gpt-oss") {}

void GPT_OSS::load_model(std::string model_path, json model_info, int default_context_length, bool enable_preemption) {
    this->model_path = model_path;
    this->_shared_load_model(model_path, model_info, default_context_length, enable_preemption);
    this->q4nx = std::make_unique<Q4NX>(this->model_path);
    this->lm_engine = std::make_unique<gpt_oss_npu>(*this->lm_config, this->npu.get(), this->MAX_L);
    this->lm_engine->load_weights(*this->q4nx);
    this->q4nx.reset();
    this->tokenizer = std::make_unique<Tokenizer>(model_path);

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

void GPT_OSS::setup_tokenizer(std::string model_path){
    auto tokenizer_config = this->_shared_setup_tokenizer(model_path);
}

std::string GPT_OSS::apply_chat_template(nlohmann::ordered_json& messages) {
    minja::chat_template_inputs inputs;
    inputs.add_generation_prompt = true;
    inputs.messages = messages;
    inputs.extra_context = this->extra_context;
    inputs.extra_context["enable_thinking"] = this->enable_think;
    inputs.extra_context["reasoning_effort"] = this->reasoning_effort;
    inputs.extra_context["model_identity"] = this->model_identity;
    inputs.extra_context["role"] = this->role;

    return this->chat_tmpl->apply(inputs);
}

bool GPT_OSS::insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input)
{
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
    return this->_shared_insert(meta_info, tokens);

}


std::string GPT_OSS::generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) {
    os << "<|start|>" << std::flush;
    os << "assistant" << std::flush;
    std::string result = this->_shared_generate(meta_info, length_limit, os);
    os << "<|end|>" << std::flush;
    return "<|start|>assistant" + result + "<|end|>";
}

std::string GPT_OSS::generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os) {
    if (!this->insert(meta_info, input)) {
        return "";
    }
    os << "<|start|>" << std::flush;
    os << "assistant" << std::flush;
    std::string result = this->_shared_generate(meta_info, length_limit, os);
    os << "<|end|>" << std::flush;
    return "<|start|>assistant" + result + "<|end|>";
}