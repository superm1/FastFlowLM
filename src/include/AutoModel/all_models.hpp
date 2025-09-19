/// \file all_models.hpp
/// \brief all_models class
/// \author FastFlowLM Team
/// \date 2025-09-10
/// \version 0.9.10
/// \note This is a header file for the all_models class
#pragma once

#include "modeling_gemma3.hpp"
#include "modeling_gemma3_text.hpp"
#include "modeling_llama3.hpp"
#include "modeling_qwen3.hpp"



inline std::pair<std::string, std::unique_ptr<AutoModel>> get_auto_model(const std::string& model_tag) {
    
    static std::unordered_set<std::string> llamaTags = {
        "llama3.1", "llama3.1:8b", 
        "llama3.2","llama3.2:1b", "llama3.2:3b"
    };

    static std::unordered_set<std::string> deepseek_r1_Tags = {
        "deepseek-r1", "deepseek-r1:8b"
    };
    static std::unordered_set<std::string> deepseek_r1_0528_Tags = {
        "deepseek-r1-0528", "deepseek-r1-0528:8b"
    };
    static std::unordered_set<std::string> qwen3_Tags = {
        "qwen3", "qwen3:0.6b", "qwen3:1.7b", "qwen3:4b", "qwen3:8b"
    };
    static std::unordered_set<std::string> qwen3_it_Tags = {
        "qwen3-it", "qwen3-it:4b"
    };
    static std::unordered_set<std::string> qwen3_tk_Tags = {
        "qwen3-tk", "qwen3-tk:4b"
    };
    static std::unordered_set<std::string> gemma3_text_Tags = {
        "gemma3", "gemma3:270m", "gemma3:1b"
    };
    static std::unordered_set<std::string> gemma3_vlm_tags = {
        "gemma3:4b", 
        "medgemma", "medgemma:4b"
    };

    std::unique_ptr<AutoModel> auto_chat_engine = nullptr;
    std::string new_model_tag = model_tag;
    if (llamaTags.count(model_tag)) // tag
        auto_chat_engine = std::make_unique<Llama3>(0);
    else if (deepseek_r1_0528_Tags.count(model_tag)) 
        auto_chat_engine = std::make_unique<DeepSeek_r1_0528_8b>(0);
    else if (deepseek_r1_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<DeepSeek_r1_8b>(0);
    else if (qwen3_it_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3_IT>(0);
    else if (qwen3_tk_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3_TK>(0);
    else if (qwen3_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<Qwen3>(0);
    else if (gemma3_text_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<Gemma3_Text_Only>(0);
    else if (gemma3_vlm_tags.count(model_tag))
        auto_chat_engine = std::make_unique<Gemma3>(0);
    else {
        new_model_tag = "llama3.2:1b"; // No arguments, use default tag
        auto_chat_engine = std::make_unique<Llama3>(0);
    }
  
    return std::make_pair(new_model_tag, std::move(auto_chat_engine));
} 