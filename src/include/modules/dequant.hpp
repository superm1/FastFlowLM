/// \file dequant.hpp
/// \brief dequant class
/// \author FastFlowLM Team
/// \date 2025-06-24
/// \version 0.9.21
/// \note This is a header file for the dequant class
#pragma once
#include "lm_config.hpp"
#include "npu_utils/npu_instr_utils.hpp"

/// \brief dequant class
/// \note This is a class for the dequant layer
class Dequant{
public:
    Dequant(){}

    /// \brief Constructor
    /// \param config the configuration
    /// \param xclbin_name the xclbin name
    /// \param npu the npu manager
    Dequant(LM_Config& config);
    ~Dequant();

    /// \brief Generate the sequence
    /// \param seq the sequence
    /// \param D_in the input dimension
    /// \param D_out the output dimension
    /// \param weight_offset the weight offset
    void generate_seq(npu_sequence* seq, const uint32_t D_in, const uint32_t D_out, const uint32_t weight_offset);

private:
    struct Impl;
    Impl* _impl;

};

