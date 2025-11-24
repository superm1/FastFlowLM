/// \file image_reader.hpp
/// \brief image_reader class
/// \author FastFlowLM Team
/// \date 2025-08-16
/// \version 0.9.21
/// \note This is a header file for the image_reader functions
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "typedef.hpp"
#include "buffer.hpp"

// Save RGB24 image as PPM file
bool save_image(const std::string& filename, const bytes& image);

