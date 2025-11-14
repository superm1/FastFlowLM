---
title: gpt-oss
parent: Benchmarks
nav_order: 4
---

## ⚡ Performance and Efficiency Benchmarks

This section reports the performance on NPU with FastFlowLM (FLM).

> **Note:** 
- Results are based on FastFlowLM v0.9.18.
- Under FLM's default NPU power mode (Performance)  
- Test system spec: AMD Ryzen™ AI 7 350 (Krakan Point) with 96 GB DRAM. 
- Newer versions may deliver improved performance. 

---

### 🚀 Decoding Speed (TPS, or Tokens per Second, @ different context lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**64k** | **128k** |**Model**|
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------:|---------:|---------|
| **gpt-oss-20b**  | NPU (FLM)    | 11.2|	11.1|	10.9|	10.3|	9.7|	8.4|	6.5|	4.7| **gpt-oss-20b**  | 

---

### 🚀 Prefill Speed (TPS, or Tokens per Second, @ different context lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**Model**|
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------|
| **gpt-oss-20b**  | NPU (FLM)    | 197|	282|	348|	340|	266|	198| **gpt-oss-20b**  | 