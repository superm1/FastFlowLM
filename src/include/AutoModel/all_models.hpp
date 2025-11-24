/// \file all_models.hpp
/// \brief all_models class
/// \author FastFlowLM Team
/// \date 2025-09-10
/// \version 0.9.21
/// \note This is a header file for the all_models class
#pragma once

#include "modeling_gemma3.hpp"
#include "modeling_gemma3_text.hpp"
#include "modeling_llama3.hpp"
#include "modeling_qwen3.hpp"
#include "modeling_gpt_oss.hpp"
#include "modeling_qwen3vl.hpp"
#include "modeling_lfm2.hpp"

inline std::string complete_simple_tag(std::string model_tag) {
    if (model_tag == "llama3.1")
        return "llama3.1:8b";
    else if (model_tag == "llama3.2")
        return "llama3.2:1b";
    else if (model_tag == "deepseek-r1")
        return "deepseek-r1:8b";
    else if (model_tag == "deepseek-r1-0528")
        return "deepseek-r1-0528:8b";
    else if (model_tag == "qwen3")
        return "qwen3:0.6b";
    else if (model_tag == "qwen3-it")
        return "qwen3-it:4b";
    else if (model_tag == "qwen3-tk")
        return "qwen3-tk:4b";
    else if (model_tag == "gemma3")
        return "gemma3:1b";
    else if (model_tag == "medgemma")
        return "medgemma:4b";
    else if (model_tag == "gpt-oss")
        return "gpt-oss:20b";
    else if (model_tag == "gpt-oss-sg")
        return "gpt-oss-sg:20b";
    else if (model_tag == "qwen3vl-it")
        return "qwen3vl-it:4b";
    else if (model_tag == "qwen3vl-tk")
        return "qwen3vl-tk:4b";
    else if (model_tag == "lfm2")
        return "lfm2:1.2b";
    else if (model_tag == "lfm2:1.2b")
        return "lfm2:1.2b";
    else if (model_tag == "lfm2:2.6b")
        return "lfm2:2.6b";
    else
        return model_tag;

}


inline std::pair<std::string, std::unique_ptr<AutoModel>> get_auto_model(const std::string& model_tag, xrt::device* npu_device_inst) {
    
    static std::unordered_set<std::string> llamatags = {
        "llama3.1", "llama3.1:8b", 
        "llama3.2","llama3.2:1b", "llama3.2:3b"
    };

    static std::unordered_set<std::string> deepseek_r1_tags = {
        "deepseek-r1", "deepseek-r1:8b"
    };
    static std::unordered_set<std::string> deepseek_r1_0528_tags = {
        "deepseek-r1-0528", "deepseek-r1-0528:8b"
    };
    static std::unordered_set<std::string> qwen3_tags = {
        "qwen3", "qwen3:0.6b", "qwen3:1.7b", "qwen3:4b", "qwen3:8b"
    };
    static std::unordered_set<std::string> qwen3_it_tags = {
        "qwen3-it", "qwen3-it:4b"
    };
    static std::unordered_set<std::string> qwen3_tk_tags = {
        "qwen3-tk", "qwen3-tk:4b"
    };
    static std::unordered_set<std::string> gemma3_text_tags = {
        "gemma3", "gemma3:270m", "gemma3:1b"
    };
    static std::unordered_set<std::string> gemma3_vlm_tags = {
        "gemma3:4b", 
        "medgemma", "medgemma:4b"
    };
    static std::unordered_set<std::string> gpt_oss_tags = {
        "gpt-oss:20b", "gpt-oss",
        "gpt-oss-sg:20b", "gpt-oss-sg"
    };
    static std::unordered_set<std::string> whisper_tags = {
        "whisper-v3:turbo", "whisper-v3", "whisper"
    };
    static std::unordered_set<std::string> embedgemma_tags = {
        "embed-gemma:300m", "embed-gemma"
    };
    static std::unordered_set<std::string> qwen3vl_tags = {
        "qwen3vl-it", "qwen3vl-it:4b"
    };
    static std::unordered_set<std::string> lfm2_tags = {
        "lfm2", "lfm2:1.2b", "lfm2:2.6b"
    };

    std::unique_ptr<AutoModel> auto_chat_engine = nullptr;
    std::string new_model_tag = complete_simple_tag(model_tag);
    if (llamatags.count(model_tag)) // tag
        auto_chat_engine = std::make_unique<Llama3>(npu_device_inst);
    else if (deepseek_r1_0528_tags.count(model_tag))
        auto_chat_engine = std::make_unique<DeepSeek_r1_0528_8b>(npu_device_inst);
    else if (deepseek_r1_tags.count(model_tag))
        auto_chat_engine = std::make_unique<DeepSeek_r1_8b>(npu_device_inst);
    else if (qwen3_it_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3_IT>(npu_device_inst);
    else if (qwen3_tk_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3_TK>(npu_device_inst);
    else if (qwen3_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3>(npu_device_inst);
    else if (gemma3_text_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Gemma3_Text_Only>(npu_device_inst);
    else if (gemma3_vlm_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Gemma3>(npu_device_inst);
    else if (gpt_oss_tags.count(model_tag))
        auto_chat_engine = std::make_unique<GPT_OSS>(npu_device_inst);
    else if (qwen3vl_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3VL>(npu_device_inst);
    else if (whisper_tags.count(model_tag)) {
        header_print("Warning", "whisper is not a LLM; Running llama3.2:1b now");
        new_model_tag = "llama3.2:1b";
        auto_chat_engine = std::make_unique<Llama3>(npu_device_inst);
    }
    else if (lfm2_tags.count(model_tag)) {
        auto_chat_engine = std::make_unique<LFM2>(npu_device_inst);
    }
    else if (embedgemma_tags.count(model_tag)) {
        header_print("Warning", "embed-gemma is not a LLM; Running llama3.2:1b now");
        new_model_tag = "llama3.2:1b";
        auto_chat_engine = std::make_unique<Llama3>(npu_device_inst);
    }
    else {
        new_model_tag = "llama3.2:1b"; // No arguments, use default tag
        auto_chat_engine = std::make_unique<Llama3>(npu_device_inst);
    }
  
    return std::make_pair(new_model_tag, std::move(auto_chat_engine));
} 