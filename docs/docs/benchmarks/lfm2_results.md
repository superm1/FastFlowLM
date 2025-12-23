---
layout: docs
title: LiquidAI/LFM2
parent: Benchmarks
nav_order: 5
---

## ⚡ Performance and Efficiency Benchmarks

This section reports the performance on NPU with FastFlowLM (FLM).

> **Note:** 
> - Results are based on FastFlowLM v0.9.24.
> - Under FLM's default NPU power mode (Performance)  
> - Test system spec: AMD Ryzen™ AI 7 350 (Krakan Point) with 32 GB DRAM. 
> - Newer versions may deliver improved performance. 

<div style="display:flex; flex-wrap:wrap;">
  <img src="/assets/bench/lfm2_decoding.png" style="width:15%; min-width:300px; margin:4px;">
  <img src="/assets/bench/lfm2_prefill.png" style="width:15%; min-width:300px; margin:4px;">
</div>

---

### 🚀 Decoding Speed (TPS, or Tokens per Second, starting @ different context lengths)

| **Model**        | **HW**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|
| **LFM2-1.2B**  | NPU (FLM)    | 61.3	|60.5	|57.5	|51.2	|45.3	|35.4|
| **LFM2-2.6B**  | NPU (FLM)    | 30.3	|29.9	|29.0	|27.3	|24.5	|20.4|

---

### 🚀 Prefill Speed (TPS, or Tokens per Second, with different prompt lengths)

| **Model**        | **HW**       | **1k** | **2k** | **4k** | **8k** | **16k** | **32k** |
|------------------|--------------------|--------:|--------:|--------:|--------:|---------:|---------:|
| **LFM2-1.2B**  | NPU (FLM)    | 1431	| 1829	| 2032	| 1920	| 1519	|1059 |
| **LFM2-2.6B**  | NPU (FLM)    | 685	| 881	| 1010	| 1008	| 864	| 654 |