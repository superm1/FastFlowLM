/// \file Qwen2VL.hpp
/// \brief Qwen2VL class
/// \author FastFlowLM Team
/// \date 2025-09-03
/// \version 0.9.24
/// \note This is a source file for the Qwen2VL class

#pragma once
#include "AutoModel/automodel.hpp"
#include "metrices.hpp"


#include "typedef.hpp"
#include "image/image_reader.hpp"
#include "image_process_utils/imageproc.hpp"
#include "image_process_utils/imageprocAVX512.hpp"
#include "tensor_utils/q4_npu_eXpress.hpp"
#include "base64.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
/************              Qwen2VL_4b            **************/
class Qwen2VL : public AutoModel {
private:

    void setup_tokenizer(std::string model_path);
    
    // Image processing functionality
    ImageReader image_reader_;
    qwen2vl_image_t load_image(const std::string& filename);
    qwen2vl_image_t load_image_base64(const std::string& base64_string);
    

    int debug_count= 0;
    void smart_resize(
    int height, int width,
    int& h_bar,int& w_bar,
    int factor,
    int min_pixels,
    int max_pixels);
    
    void preprocess_image(qwen2vl_image_t& image,  std::vector<bf16> &pixel_values);

public:
    Qwen2VL(xrt::device* npu_device_inst);

    void load_model(std::string model_path, json model_inf, int default_context_length = -1, bool enable_preemption = false) override;
    //void toggle_enable_think() override;
    bool insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) override;
    std::string generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os, std::function<bool()> is_cancelled = [] { return false; }) override;
    std::string generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os = std::cout) override;
    std::string apply_chat_template(nlohmann::ordered_json& messages, nlohmann::ordered_json tools = nlohmann::ordered_json::object()) override;
};
