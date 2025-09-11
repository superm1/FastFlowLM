---
title: Gemma 3
parent: Benchmarks
nav_order: 3
---

## ⚡ Performance and Efficiency Benchmarks

This section reports the performance and power usage of Gemma 3 on NPU (FastFlowLM, or FLM).

> **Note:** 
- Results are based on FastFlowLM v0.9.8.
- Under FLM's default NPU power mode (Performance)  
- Test system spec: AMD Ryzen™ AI 7 350 (Krakan Point) with 32 GB DRAM. 
- Newer versions may deliver improved performance. 

---

### 🚀 Decoding Speed (TPS, or Tokens per Second, @ different context lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**64k** | **128k** |**Model**|
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------:|---------:|---------|
| **Gemma 3 1B**  | NPU (FLM)    | 34.2|	33.7|	32.6|	31.4|	28.3|	24.1|	OOC|	OOC| **Gemma 3 1B**  | 
| **Gemma 3 4B**  | NPU (FLM)    | 14.4|	14.4|	14.1|	13.7|	13.0|	11.9| 10.8|	9.2|**Gemma 3 4B**  |

> OOC: Out Of Context Length  
> Each LLM has a maximum supported context window. For example, the gemma3:1b model supports up to 32k tokens.

---

### 🚀 Prefill Speed (TTFT, or Time to First Token in **Seconds**, with different prompt lengths)

| **Model**        | **Hardware**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |**Model**|
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|---------|
| **Gemma 3 1B**   | NPU (FLM)    | 1.02|	1.64|	2.70|	4.90|	9.74|	21.03|**Gemma 3 1B**  |
| **Gemma 3 4B**   | NPU (FLM)    | 1.98|	3.27|	5.82|	11.06|	22.91|	50.87|**Gemma 3 4B**  |