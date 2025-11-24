/// \file modeling_Qwen3VL_image.cpp
/// \brief Qwen3VL image processing implementation
/// \author FastFlowLM Team
/// \date 2025-09-01
/// \version 0.9.21
/// \note This is a source file for the Qwen3VL image processing functionality

#include "AutoModel/modeling_qwen3vl.hpp"

// Initialize FFmpeg once
bool Qwen3VL::ffmpeg_initialized = false;

void Qwen3VL::initialize_ffmpeg() {
    if (!ffmpeg_initialized) {
        ffmpeg_initialized = true;
    }
}

// Helper to map deprecated YUVJ pixel formats and determine source range
void Qwen3VL::resolve_source_format_and_range(AVPixelFormat input_format,
                                    AVPixelFormat &resolved_format,
                                    int &src_full_range,
                                    AVColorRange frame_color_range,
                                    AVCodecID codec_id) {
    resolved_format = input_format;
    src_full_range = 0; // default: limited range

    // If decoder reports full-range, honor it
    if (frame_color_range == AVCOL_RANGE_JPEG) {
        src_full_range = 1;
    }

    // JPEG is typically full range
    if (codec_id == AV_CODEC_ID_MJPEG) {
        src_full_range = 1;
    }

    switch (input_format) {
        case AV_PIX_FMT_YUVJ420P:
            resolved_format = AV_PIX_FMT_YUV420P;
            src_full_range = 1;
            break;
        case AV_PIX_FMT_YUVJ422P:
            resolved_format = AV_PIX_FMT_YUV422P;
            src_full_range = 1;
            break;
        case AV_PIX_FMT_YUVJ444P:
            resolved_format = AV_PIX_FMT_YUV444P;
            src_full_range = 1;
            break;
        case AV_PIX_FMT_YUVJ440P:
            resolved_format = AV_PIX_FMT_YUV440P;
            src_full_range = 1;
            break;
        default:
            break;
    }
}

qwen3vl_image_t Qwen3VL::load_image(const std::string& filename) {
    initialize_ffmpeg();
    
    qwen3vl_image_t empty_result;
    
    try {
        // Check if file exists
        if (!std::filesystem::exists(filename)) {
            std::cerr << "Error: File does not exist: " << filename << std::endl;
            return empty_result;
        }

        // Read file into memory
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file: " << filename << std::endl;
            return empty_result;
        }
        
        std::streamsize file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> file_data(file_size);
        if (!file.read(reinterpret_cast<char*>(file_data.data()), file_size)) {
            std::cerr << "Error: Could not read file data" << std::endl;
            return empty_result;
        }
        file.close();

        // Create memory context for decoding
        AVCodecContext* codecContext = nullptr;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        
        try {
            // Find decoder based on file header
            const AVCodec* codec = nullptr;
            if (file_data.size() >= 8) {
                // Check for JPEG
                if (file_data[0] == 0xFF && file_data[1] == 0xD8) {
                    codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
                }
                // Check for PNG
                else if (file_data.size() >= 8 && 
                        file_data[0] == 0x89 && file_data[1] == 0x50 && 
                        file_data[2] == 0x4E && file_data[3] == 0x47) {
                    codec = avcodec_find_decoder(AV_CODEC_ID_PNG);
                }
            }
            
            if (!codec) {
                std::cerr << "Error: Unsupported image format (only JPEG and PNG supported)" << std::endl;
                return empty_result;
            }

            // Create codec context
            codecContext = avcodec_alloc_context3(codec);
            if (!codecContext) {
                std::cerr << "Error: Could not allocate codec context" << std::endl;
                return empty_result;
            }

            // Open codec
            if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                std::cerr << "Error: Could not open codec" << std::endl;
                return empty_result;
            }

            // Allocate frame and packet
            frame = av_frame_alloc();
            packet = av_packet_alloc();
            if (!frame || !packet) {
                std::cerr << "Error: Could not allocate frame or packet" << std::endl;
                return empty_result;
            }

            // Set packet data
            packet->data = file_data.data();
            packet->size = static_cast<int>(file_data.size());

            // Send packet for decoding
            int response = avcodec_send_packet(codecContext, packet);
            if (response < 0) {
                std::cerr << "Error: Error sending packet for decoding" << std::endl;
                return empty_result;
            }

            // Receive decoded frame
            response = avcodec_receive_frame(codecContext, frame);
            if (response < 0) {
                std::cerr << "Error: Error during decoding" << std::endl;
                return empty_result;
            }

            // Resolve deprecated pixel formats and color range
            AVPixelFormat srcFmtResolved = AV_PIX_FMT_NONE;
            int srcIsFullRange = 0;
            resolve_source_format_and_range(static_cast<AVPixelFormat>(frame->format),
                                            srcFmtResolved,
                                            srcIsFullRange,
                                            frame->color_range,
                                            codecContext->codec_id);

            // Convert to RGB24 without rescaling
            SwsContext* swsContext = sws_getContext(
                frame->width, frame->height, srcFmtResolved,
                frame->width, frame->height, AV_PIX_FMT_RGB24,
                SWS_BILINEAR, nullptr, nullptr, nullptr
            );
            
            if (!swsContext) {
                std::cerr << "Error: Could not create scaling context" << std::endl;
                if (frame) av_frame_free(&frame);
                if (packet) av_packet_free(&packet);
                if (codecContext) {
                    avcodec_close(codecContext);
                    avcodec_free_context(&codecContext);
                }
                return empty_result;
            }

            // Set colorspace details to respect full/limited range
            const int* srcCoeffs = sws_getCoefficients(SWS_CS_DEFAULT);
            const int* dstCoeffs = sws_getCoefficients(SWS_CS_DEFAULT);
            int dstIsFullRange = 1; // RGB is full range 0..255
            sws_setColorspaceDetails(swsContext, srcCoeffs, srcIsFullRange,
                                    dstCoeffs, dstIsFullRange,
                                    0, 1 << 16, 1 << 16);

            // Allocate RGB24 frame
            AVFrame* rgbFrame = av_frame_alloc();
            int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
            
            qwen3vl_image_t result;
            result.width = frame->width;
            result.height = frame->height;
            result._data.resize(rgbBufferSize);
            
            if (result._data.size() > 0) {
                av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, result._data.data(), AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
                
                // Convert to RGB24 without rescaling
                sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, rgbFrame->data, rgbFrame->linesize);
                
                // Reorder from HWC (height, width, 3) to CHW (3, height, width)
                // RGB24 layout: pixel 0=[r0,g0,b0], pixel 1=[r1,g1,b1], ...
                // Desired layout: all R, then all G, then all B
                std::vector<uint8_t> reordered_data(rgbBufferSize);
                int num_pixels = frame->width * frame->height;
                const uint8_t* src = result._data.data();
                uint8_t* dst = reordered_data.data();
                
                // Separate RGB channels into CHW layout
                for (int y = 0; y < frame->height; y++) {
                    for (int x = 0; x < frame->width; x++) {
                        int hwc_idx = y * frame->width * 3 + x * 3;
                        int pixel_idx = y * frame->width + x;
                        
                        dst[0 * num_pixels + pixel_idx] = src[hwc_idx];     // R channel
                        dst[1 * num_pixels + pixel_idx] = src[hwc_idx + 1]; // G channel
                        dst[2 * num_pixels + pixel_idx] = src[hwc_idx + 2]; // B channel
                    }
                }
                
                result._data = std::move(reordered_data);
                
                av_frame_free(&rgbFrame);
                sws_freeContext(swsContext);
                // Cleanup decoder resources before returning
                if (frame) av_frame_free(&frame);
                if (packet) av_packet_free(&packet);
                if (codecContext) {
                    avcodec_close(codecContext);
                    avcodec_free_context(&codecContext);
                }
                return result;
            }

            av_frame_free(&rgbFrame);
            sws_freeContext(swsContext);
            return empty_result;

        } catch (...) {
            // Cleanup on error
            if (frame) av_frame_free(&frame);
            if (packet) av_packet_free(&packet);
            if (codecContext) avcodec_free_context(&codecContext);
            throw;
        }

        if (frame) {
            av_frame_free(&frame);
        }
        if (packet) {
            av_packet_free(&packet);
        }
        if (codecContext) {
            avcodec_close(codecContext);
            avcodec_free_context(&codecContext);
        }

        return empty_result;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
        return empty_result;
    }
}

qwen3vl_image_t Qwen3VL::load_image_base64(const std::string& base64_string) {
    initialize_ffmpeg();
    
    qwen3vl_image_t empty_result;
    
    try {
        // Decode base64 to binary data
        std::string decoder_bytes = base64::from_base64(base64_string);
        
        // Check for valid image formats
        if (decoder_bytes.size() < 8) {
            std::cerr << "Error: Invalid image data (too small)" << std::endl;
            return empty_result;
        }
        
        // Convert string to vector<uint8_t> for processing
        std::vector<uint8_t> file_data(decoder_bytes.begin(), decoder_bytes.end());
        
        // Create memory context for decoding
        AVCodecContext* codecContext = nullptr;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        
        try {
            // Find decoder based on file header
            const AVCodec* codec = nullptr;
            if (file_data.size() >= 8) {
                // Check for JPEG
                if (file_data[0] == 0xFF && file_data[1] == 0xD8) {
                    codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
                }
                // Check for PNG
                else if (file_data[0] == 0x89 && file_data[1] == 0x50 && 
                        file_data[2] == 0x4E && file_data[3] == 0x47) {
                    codec = avcodec_find_decoder(AV_CODEC_ID_PNG);
                }
            }
            
            if (!codec) {
                std::cerr << "Error: Unsupported image format (only JPEG and PNG supported)" << std::endl;
                return empty_result;
            }

            // Create codec context
            codecContext = avcodec_alloc_context3(codec);
            if (!codecContext) {
                std::cerr << "Error: Could not allocate codec context" << std::endl;
                return empty_result;
            }

            // Open codec
            if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                std::cerr << "Error: Could not open codec" << std::endl;
                return empty_result;
            }

            // Allocate frame and packet
            frame = av_frame_alloc();
            packet = av_packet_alloc();
            if (!frame || !packet) {
                std::cerr << "Error: Could not allocate frame or packet" << std::endl;
                return empty_result;
            }

            // Set packet data
            packet->data = file_data.data();
            packet->size = static_cast<int>(file_data.size());

            // Send packet for decoding
            int response = avcodec_send_packet(codecContext, packet);
            if (response < 0) {
                std::cerr << "Error: Error sending packet for decoding" << std::endl;
                return empty_result;
            }

            // Receive decoded frame
            response = avcodec_receive_frame(codecContext, frame);
            if (response < 0) {
                std::cerr << "Error: Error during decoding" << std::endl;
                return empty_result;
            }

            // Resolve deprecated pixel formats and color range
            AVPixelFormat srcFmtResolved = AV_PIX_FMT_NONE;
            int srcIsFullRange = 0;
            resolve_source_format_and_range(static_cast<AVPixelFormat>(frame->format),
                                            srcFmtResolved,
                                            srcIsFullRange,
                                            frame->color_range,
                                            codecContext->codec_id);

            // Convert to RGB24 without rescaling
            SwsContext* swsContext = sws_getContext(
                frame->width, frame->height, srcFmtResolved,
                frame->width, frame->height, AV_PIX_FMT_RGB24,
                SWS_BILINEAR, nullptr, nullptr, nullptr
            );
            
            if (!swsContext) {
                std::cerr << "Error: Could not create scaling context" << std::endl;
                if (frame) av_frame_free(&frame);
                if (packet) av_packet_free(&packet);
                if (codecContext) {
                    avcodec_close(codecContext);
                    avcodec_free_context(&codecContext);
                }
                return empty_result;
            }

            // Set colorspace details to respect full/limited range
            const int* srcCoeffs = sws_getCoefficients(SWS_CS_DEFAULT);
            const int* dstCoeffs = sws_getCoefficients(SWS_CS_DEFAULT);
            int dstIsFullRange = 1; // RGB is full range 0..255
            sws_setColorspaceDetails(swsContext, srcCoeffs, srcIsFullRange,
                                    dstCoeffs, dstIsFullRange,
                                    0, 1 << 16, 1 << 16);

            // Allocate RGB24 frame
            AVFrame* rgbFrame = av_frame_alloc();
            int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
            
            qwen3vl_image_t result;
            result.width = frame->width;
            result.height = frame->height;
            result._data.resize(rgbBufferSize);
            
            if (result._data.size() > 0) {
                av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, result._data.data(), AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
                
                // Convert to RGB24 without rescaling
                sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, rgbFrame->data, rgbFrame->linesize);
                
                // Reorder from HWC (height, width, 3) to CHW (3, height, width)
                // RGB24 layout: pixel 0=[r0,g0,b0], pixel 1=[r1,g1,b1], ...
                // Desired layout: all R, then all G, then all B
                std::vector<uint8_t> reordered_data(rgbBufferSize);
                int num_pixels = frame->width * frame->height;
                const uint8_t* src = result._data.data();
                uint8_t* dst = reordered_data.data();
                
                // Separate RGB channels into CHW layout
                for (int y = 0; y < frame->height; y++) {
                    for (int x = 0; x < frame->width; x++) {
                        int hwc_idx = y * frame->width * 3 + x * 3;
                        int pixel_idx = y * frame->width + x;
                        
                        dst[0 * num_pixels + pixel_idx] = src[hwc_idx];     // R channel
                        dst[1 * num_pixels + pixel_idx] = src[hwc_idx + 1]; // G channel
                        dst[2 * num_pixels + pixel_idx] = src[hwc_idx + 2]; // B channel
                    }
                }
                
                result._data = std::move(reordered_data);
                
                av_frame_free(&rgbFrame);
                sws_freeContext(swsContext);
                // Cleanup decoder resources before returning
                if (frame) av_frame_free(&frame);
                if (packet) av_packet_free(&packet);
                if (codecContext) {
                    avcodec_close(codecContext);
                    avcodec_free_context(&codecContext);
                }
                return result;
            }

            av_frame_free(&rgbFrame);
            sws_freeContext(swsContext);
            return empty_result;

        } catch (...) {
            // Cleanup on error
            if (frame) av_frame_free(&frame);
            if (packet) av_packet_free(&packet);
            if (codecContext) avcodec_free_context(&codecContext);
            throw;
        }

        // Cleanup
        if (frame) av_frame_free(&frame);
        if (packet) av_packet_free(&packet);
        if (codecContext) avcodec_free_context(&codecContext);

        return empty_result;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading base64 image: " << e.what() << std::endl;
        return empty_result;
    }
}


void Qwen3VL::smart_resize(
    int height,
    int width,
    int& h_bar,
    int& w_bar,
    int factor,
    int min_pixels,
    int max_pixels
) {
    double aspect_ratio = static_cast<double>(std::max(height, width)) /
                          static_cast<double>(std::min(height, width));
    if (aspect_ratio > 200.0) {
        std::cerr << "absolute aspect ratio must be smaller than 200, got " +
            std::to_string(aspect_ratio);
    }

    h_bar = static_cast<int>(std::round(static_cast<double>(height) / factor)) * factor;
    w_bar = static_cast<int>(std::round(static_cast<double>(width) / factor)) * factor;

    long long total_pixels = static_cast<long long>(h_bar) * w_bar;

    if (total_pixels > max_pixels) {
        double beta = std::sqrt((static_cast<double>(height) * width) / max_pixels);
        h_bar = std::max(factor,
                         static_cast<int>(std::floor(height / beta / factor)) * factor);
        w_bar = std::max(factor,
                         static_cast<int>(std::floor(width / beta / factor)) * factor);
    } else if (total_pixels < min_pixels) {
        double beta = std::sqrt(static_cast<double>(min_pixels) /
                                (static_cast<double>(height) * width));
        h_bar = static_cast<int>(std::ceil(height * beta / factor)) * factor;
        w_bar = static_cast<int>(std::ceil(width * beta / factor)) * factor;
    }
}


///@brief: preprocess the image for Qwen3VL model
///@note: Converts uint8 image to BF16 format, data is already in (3, H, W) CHW layout
///@param: image: the image to preprocess (already in CHW format)
///@return: the preprocessed image in BF16 format
void Qwen3VL::preprocess_image(qwen3vl_image_t& image, std::vector<bf16> &pixel_values) {
    const int width = image.width;
    const int height = image.height;
    const int channels = 3; // RGB
    int resized_height; 
    int resized_width;    
    // do the automatically resizing in here 
    smart_resize(
        height, width,
        resized_height, resized_width,
        QWEN3_PATCH_SIZE * QWEN3_IMAGE_MERGE_SIZE,
        QWEN3_SHORTEST_EDGE,
        QWEN3_LONGEST_EDGE
    );
    // std::cout << "resized_height "<< resized_height << " resized_width " << resized_width <<std::endl;

    // Cache size calculations for efficiency
    const uint32_t single_frame_size = resized_height * resized_width * channels;
    const uint32_t total_patch_size = single_frame_size * QWEN3_TEMPORAL_PATCH_SIZE;
    const uint32_t grid_h = resized_height / QWEN3_PATCH_SIZE;
    const uint32_t grid_w = resized_width / QWEN3_PATCH_SIZE;

    // Pre-allocate final buffer to avoid reallocation
    const uint32_t prev_pixel_values_size = pixel_values.size();
    pixel_values.resize(prev_pixel_values_size + total_patch_size);

    // Use non-optimized path for consistent results across platforms
    auto resize_image =  imgproc::avx512::resize_bicubic_antialias_rgb_planar_avx512(
        image._data.data(), width, height, resized_width, resized_height, true
    );

    // Allocate patch_vector with exact size needed
    std::vector<float> patch_vector(total_patch_size);
    
    // Apply rescale and normalization to first frame
    imgproc::avx512::rescale_and_normalize_avx512(
        resize_image.data(), patch_vector.data(),
        resized_width, resized_height, channels,
        true, QWEN3_VISION_RESCALE_FACTOR,
        true, QWEN3_VISION_RESCALE_IMAGE_MEAN, QWEN3_VISION_RESCALE_IMAGE_STD
    );

    // Free resize_image early to reduce memory footprint
    resize_image.clear();
    resize_image.shrink_to_fit();
    
    // Replicate first frame for temporal patches (optimized for QWEN3_TEMPORAL_PATCH_SIZE = 2)
    // This is more efficient than a loop for the common case
    if constexpr (QWEN3_TEMPORAL_PATCH_SIZE == 2) {
        memcpy(
            patch_vector.data() + single_frame_size,
            patch_vector.data(),
            single_frame_size * sizeof(float)
        );
    } else {
        // Generic loop for other TEMPORAL_PATCH_SIZE values
        for(unsigned l = 1; l < QWEN3_TEMPORAL_PATCH_SIZE; l++){
            memcpy(
                patch_vector.data() + l * single_frame_size,
                patch_vector.data(),
                single_frame_size * sizeof(float)
            );
        }
    }

    // Reorder patches directly into pre-allocated pixel_values
    imgproc::reorder_patches_inplace(
        patch_vector.data(),
        pixel_values.data() + prev_pixel_values_size,
        1, 1, // something special for image
        QWEN3_TEMPORAL_PATCH_SIZE,
        channels,
        grid_h, grid_w,
        QWEN3_MERGE_SIZE,
        QWEN3_PATCH_SIZE
    );


    image.width_resized = resized_width;
    image.height_resized = resized_height;
    image.grid_h = grid_h;
    image.grid_w = grid_w;
    image._data.clear(); // free the data

    // release the original uint8_t data now, since is no longer useful to us
}
