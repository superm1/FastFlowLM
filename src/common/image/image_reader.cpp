/// \file image_reader.cpp
/// \brief image_reader class
/// \author FastFlowLM Team
/// \date 2025-08-16
/// \version 0.9.21
/// \note This is a source file for the image_reader functions

#include "image/image_reader.hpp"
#include "typedef.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <immintrin.h>

// FFmpeg includes for image processing only
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
}



bool save_image(const std::string& filename, const bytes& image) {
    try {
        // Calculate dimensions from image size (RGB24 = 3 bytes per pixel)
        if (image.size() % 3 != 0) {
            std::cerr << "Error: Invalid image data size (not divisible by 3)" << std::endl;
            return false;
        }
        
        int totalPixels = static_cast<int>(image.size() / 3);
        int width = static_cast<int>(sqrt(totalPixels));
        int height = totalPixels / width;
        
        if (width * height != totalPixels) {
            std::cerr << "Error: Image data does not represent a square image" << std::endl;
            return false;
        }

        // Write PPM format (Portable Pixmap)
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }

        // Write PPM header: "P6\nwidth height\n255\n"
        file << "P6\n" << width << " " << height << "\n255\n";
        
        // Write RGB data
        file.write(reinterpret_cast<const char*>(image.data()), image.size());
        file.close();

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving image: " << e.what() << std::endl;
        return false;
    }
}
