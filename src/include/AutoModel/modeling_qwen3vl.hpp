/// \file Qwen3VL.hpp
/// \brief Qwen3VL class
/// \author FastFlowLM Team
/// \date 2025-09-03
/// \version 0.9.21
/// \note This is a source file for the Qwen3VL class

#pragma once
#include "AutoModel/automodel.hpp"
#include "metrices.hpp"


#include "typedef.hpp"
#include "image_process_utils/imageproc.hpp"
#include "image_process_utils/imageprocAVX512.hpp"
#include "tensor_utils/q4_npu_eXpress.hpp"
#include "base64.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
// FFmpeg includes for image processing only
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
}


//Parameter for QWEN3IMAGE 
constexpr unsigned int QWEN3_PATCH_SIZE = 16;
constexpr unsigned int QWEN3_IMAGE_MERGE_SIZE=2;
constexpr unsigned int QWEN3_SPATIAL_MERGE_SIZE=2;
constexpr unsigned int QWEN3_SHORTEST_EDGE = 65536;
constexpr unsigned int QWEN3_LONGEST_EDGE = 16777216;
constexpr float QWEN3_VISION_RESCALE_FACTOR = 0.00392156862745098;
constexpr float QWEN3_VISION_RESCALE_IMAGE_MEAN = 0.5f;
constexpr float QWEN3_VISION_RESCALE_IMAGE_STD = 0.5f;
constexpr unsigned int QWEN3_TEMPORAL_PATCH_SIZE = 2;
constexpr unsigned int QWEN3_MERGE_SIZE = 2;


typedef struct {
    int height;
    int width;
    int height_resized;  // assigned by image preprocessing
    int width_resized;
    int grid_h;
    int grid_w;

    std::vector<uint8_t> _data;

} qwen3vl_image_t;



typedef struct {
    std::vector<qwen3vl_image_t> images;
    std::vector<bf16> _data__processed;    
    unsigned int num_images;
}qwen3vl_image_payload_t;




/************              Qwen3VL_4b            **************/
class Qwen3VL : public AutoModel {
private:

    void setup_tokenizer(std::string model_path);
    
    // Image processing functionality
    static bool ffmpeg_initialized;
    void initialize_ffmpeg();
    void resolve_source_format_and_range(AVPixelFormat input_format,
                                        AVPixelFormat &resolved_format,
                                        int &src_full_range,
                                        AVColorRange frame_color_range,
                                        AVCodecID codec_id);
    qwen3vl_image_t load_image(const std::string& filename);
    qwen3vl_image_t load_image_base64(const std::string& base64_string);
    

    int debug_count= 0;
    void smart_resize(
    int height, int width,
    int& h_bar,int& w_bar,
    int factor,
    int min_pixels,
    int max_pixels);
    
    void preprocess_image(qwen3vl_image_t& image,  std::vector<bf16> &pixel_values);

public:
    Qwen3VL(xrt::device* npu_device_inst);

    void load_model(std::string model_path, json model_inf, int default_context_length = -1, bool enable_preemption = false) override;
    //void toggle_enable_think() override;
    bool insert(chat_meta_info_t& meta_info, lm_uniform_input_t& input) override;
    std::string generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) override;
    std::string generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os = std::cout) override;
    std::string apply_chat_template(nlohmann::ordered_json& messages) override;
};


/************              Qwen3VL_Thinking            **************/
class Qwen3VL_Thinking : public Qwen3VL {
    private:
        int think_marker_id;
    
    public:
        Qwen3VL_Thinking(xrt::device* npu_device_inst) : Qwen3VL(npu_device_inst) {
    
        }
        std::string generate(chat_meta_info_t& meta_info, int length_limit, std::ostream& os) override;
        std::string generate_with_prompt(chat_meta_info_t& meta_info, lm_uniform_input_t& input, int length_limit, std::ostream& os = std::cout) override;
    };