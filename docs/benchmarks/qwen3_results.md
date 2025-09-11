---
title: Qwen 3
parent: Benchmarks
nav_order: 2
---

## ⚡ Performance and Efficiency Benchmarks

This section reports the performance and power usage of Qwen 3 on NPU (FastFlowLM, or FLM).

> **Note:** 
- Results are based on FastFlowLM v0.9.8.  
- Under FLM's default NPU power mode (Performance)    
- Test system spec: AMD Ryzen™ AI 7 350 (Krakan Point) with 32 GB DRAM.   
- Newer versions may deliver improved performance.   

---

### 🚀 Decoding Speed (TPS, or Tokens per Second, @ different context lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**Model**  |
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------|
| **Qwen 3 0.6B**  | NPU (FLM)    | 50.4|	45.3|	36.0|	25.7|	16.4|	9.6|**Qwen 3 0.6B**|
| **Qwen 3 1.7B**  | NPU (FLM)    | 27.8|	26.1|	22.7|	18.3|	13.1|	8.3|**Qwen 3 1.7B**  |
| **Qwen 3 4B**    | NPU (FLM)    | 14.0|	13.3|	11.9|	10.1|	7.7|	5.3|**Qwen 3 4B**    | 
| **Qwen 3 8B**    | NPU (FLM)    | 8.1|	7.9|	7.4|	6.6|	5.5|	4.1|**Qwen 3 8B**    |

---

### 🚀 Prefill Speed (TTFT, or Time to First Token in **Seconds**, with different prompt lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**Model**  |
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------|
| **Qwen 3 0.6B**  | NPU (FLM)    | 0.88|	1.42|	3.59|	10.66|	37.20|	139.44|**Qwen 3 0.6B**|
| **Qwen 3 1.7B**  | NPU (FLM)    | 1.19|	2.11|	4.70|	12.50|	40.81|	146.53|**Qwen 3 1.7B**  |
| **Qwen 3 4B**    | NPU (FLM)    | 2.04|	3.61|	7.68|	19.23|	58.85|	203.42|**Qwen 3 4B**    | 
| **Qwen 3 8B**    | NPU (FLM)    | 2.87|	4.87|	10.17|	23.87|	68.02|	224.52|**Qwen 3 8B**    |