---
title: GPT-oss
nav_order: 6
parent: Models
---

## 🧩 Model Card: gpt-oss-20b  

- **Type:** Text-to-Text
- **Think:** Low / Medium / High intensity (reasoning effort) 
- **Base Model:** [openai/gpt-oss-20b](https://huggingface.co/openai/gpt-oss-20b)
- **Max Context Length:** 128k tokens 
- **Default Context Length:** 8192 tokens ([change default](https://docs.fastflowlm.com/instructions/cli.html#-change-default-context-length-max))  
- **[Set Context Length at Launch](https://docs.fastflowlm.com/instructions/cli.html#%EF%B8%8F-set-context-length-at-launch)**

▶️ Run with FastFlowLM in PowerShell:  

```powershell
flm run gpt-oss:20b
```

Set reasoning effort (CLI):

```powershell
# CLI
flm run gpt-oss:20b
/set r-eff medium
```

> 📝 NOTE

> - **Memory Requirements**  
   ⚠️ **Note**: Running `gpt-oss:20b` currently requires a system with **48 GB RAM or more**. The model itself uses approximately 16.4 GB of memory in FLM, and there is an internal cap (~15.6 GB) on NPU memory allocation enforced by AMD, which makes only about half of the total system RAM available to the NPU. On 32 GB machines, this limitation prevents the model from loading successfully, so we recommend using 48 GB or more RAM for a smooth experience.

> - **Open WebUI Patch**  
   A community patch is available to run `gpt-oss` seamlessly with Open WebUI using the OpenAI Harmony template with FLM: 👉 [https://openwebui.com/f/alfredxu/harmony2think_filter_flm](https://openwebui.com/f/alfredxu/harmony2think_filter_flm). This patch enables GPT-OSS in FLM to integrate smoothly with browser-based UIs while preserving compatibility with the OpenAI API format.


---