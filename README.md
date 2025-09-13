<p align="center">
  <a href="https://www.fastflowlm.com" target="_blank">
    <img src="assets/logo.png" alt="FastFlowLM Logo" width="200"/>
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/NPU-Optimized-red" />
</p>

## ⚡ FastFlowLM (FLM) — Unlock Ryzen™ AI NPUs

Run large language models — now with **Vision support** — on AMD Ryzen™ AI NPUs in minutes.  
**No GPU required. Faster and over 10× more power-efficient. Context lengths up to 256k tokens.**

✨ *From Idle Silicon to Instant Power — FastFlowLM Makes Ryzen™ AI Shine.*

> FastFlowLM (FLM) supports all Ryzen™ AI Series chips with XDNA2 NPUs (Strix, Strix Halo, and Kraken).

---

## 🔗 Quick Links

| 🔽 **[Download](https://github.com/FastFlowLM/FastFlowLM/releases/latest/download/flm-setup.exe)** | 📊 **[Benchmarks](https://docs.fastflowlm.com/benchmarks/)** | 📦 **[Model List](https://docs.fastflowlm.com/models/)** |


| 📖 **[Docs](https://docs.fastflowlm.com)** | 📺 **[Demos](https://www.youtube.com/@FastFlowLM-YT/playlists)** | 🧪 **[Test Drive](https://github.com/FastFlowLM/FastFlowLM?tab=readme-ov-file#-remote-test-drive)** | 💬 **[Discord](https://discord.gg/z24t23HsHF)** |

---

## 🚀 Quick Start

A packaged FLM Windows installer is available here: [**flm-setup.exe**](https://github.com/FastFlowLM/FastFlowLM/releases/latest/download/flm-setup.exe). For more details, see the [release notes](https://github.com/FastFlowLM/FastFlowLM/releases/).

📺 [**Watch the quick start video**](https://www.youtube.com/watch?v=mYOfDNkyBII)

> ⚠️ Ensure NPU driver is **32.0.203.258** or later (check via Task Manager→Performance→NPU or Device Manager) — [Driver Download](https://www.amd.com/en/support).

After installation, open **PowerShell**. To run a model in terminal (CLI Mode):
```powershell
flm run llama3.2:1b
```
> Requires internet access to HuggingFace to pull (download) the optimized model kernel. The model will be downloaded to the folder: ``C:\Users\<USER>\Documents\flm\models\``. ⚠️ If HuggingFace is not directly accessible in your region, you can manually download the model and place it in this directory.

To start the local server (Server Mode):
```powershell
flm serve llama3.2:1b
```
> The model tag (e.g., `llama3.2:1b`) sets the initial model, which is optional. If another model is requested, FastFlowLM will automatically switch to it. Local server is on port 11434 (default).  

🔗 **[Detailed instructions](https://docs.fastflowlm.com/instructions/)**

---

## 🧠 Local AI on NPU

FLM makes it easy to run cutting-edge **LLMs** (and now **VLMs**) locally with:
- ⚡ Fast and low power
- 🧰 Simple CLI and API
- 🔐 Fully private and offline

No model rewrites, no tuning — it just works.

---

## ✅ Highlights

- **Runs fully on AMD Ryzen™ AI NPU** — no GPU or CPU load  
- **Developer-first flow** — like Ollama, but optimized for NPU  
- **Support for long context windows** — up to 256k tokens (e.g., Qwen3-4B-Thinking-2507)  
- **No low-level tuning required** — You focus on your app, we handle the rest

---

## 🧪 Remote Test Drive

🚀 Don’t have a Ryzen™ AI PC? Instantly try FastFlowLM on a live AMD Ryzen™ AI 5 340 NPU (in our office) with 32 GB memory ([spec](https://www.amazon.com/4X4-BOX-AI340-Display-Support-Copilot/dp/B0F2GFLF67/ref=sr_1_5?crid=1X16RDUCQ2497&dib=eyJ2IjoiMSJ9.C5GS4xMl_kkJ7Yr6dNFi6g.Dfj_l9Dk1yuIBjppqmKSqNAAPQc1F4Mu3zJ9-MDlszw&dib_tag=se&keywords=4x4+box+ai340&qid=1752010554&sprefix=www.amazon.com%2F4X4-BOX-AI340%2Caps%2C176&sr=8-5)) — no setup needed.  

✨ Now with **Gemma3:4b (the first NPU-only VLM!)** supported here.  

🌐 **Launch Now**: [https://open-webui.testdrive-fastflowlm.com/](https://open-webui.testdrive-fastflowlm.com/)  
🔐 **Login**: `guest@flm.npu`  
🔑 **Password**: `0000`

📺 [**Watch this short video**](https://youtu.be/0AhkX2ZLu7Y?list=PLf87s9UUZrJqcaO6Vrl4YAmkofdGkvlom) to see how to try the remote demo in just a few clicks.

> Alternatively, **sign up** with your own credentials instead of using the shared guest account.
> ⚠️ Some universities or companies may **block access** to the test drive site. If it doesn't load over Wi-Fi, try switching to a **cellular network**.  
> Real-time demo powered by **FastFlowLM + Open WebUI** — no downloads, no installs.  
> Try optimized LLM models: `gemma3:4b`, `qwen3:4b`, etc. — all accelerated on NPU.

⚠️ **Please note**:  
- FastFlowLM is designed for **single-user local use**. This remote demo machine may experience short wait times when **multiple users** access it concurrently — please be patient.
- When switching models, it may take longer time to replace the model in memory.
- Large prompts and VLM (gemma3:4b) may take longer — but it works! 🙂

---

## 📄 License

- All orchestration code and CLI tools are open-source under the [MIT License](./LICENSE_RUNTIME.txt).
- NPU-accelerated kernels are **proprietary binaries**, free for **non-commercial use** only — see [LICENSE_BINARY.txt](./LICENSE_BINARY.txt) and [TERMS.md](./TERMS.md) for details.
- **Non-commercial users need to acknowledge FastFlowLM** by adding this line to your README or project page:  
  ```
  Powered by [FastFlowLM](https://github.com/FastFlowLM/FastFlowLM)
  ```
For commercial use or licensing inquiries, email us: info@fastflowlm.com

---

💬 Have **feedback/issues** or want **early access** to our new releases?[Open an issue](https://github.com/fastflowlm/fastflowlm/issues/new) or [Join our Discord community](https://discord.gg/z24t23HsHF)

---

## 🙏 Acknowledgements

- Powered by the advanced **AMD Ryzen™ AI NPU architecture**
- Inspired by the widely adopted [Ollama](https://github.com/ollama/ollama)
- Tokenization accelerated with [MLC-ai/tokenizers-cpp](https://github.com/mlc-ai/tokenizers-cpp)
- Chat formatting via [Google/minja](https://github.com/google/minja)
- Low-level kernels optimized using the powerful [IRON](https://github.com/Xilinx/mlir-aie/tree/main/programming_guide)+[AIE-MLIR](https://github.com/Xilinx/mlir-aie/tree/main/mlir_tutorials)
