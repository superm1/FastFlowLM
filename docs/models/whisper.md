---
title: whisper
nav_order: 7
parent: Models
---

## 🧩 Model Card: whisper-large-v3-turbo  

- **Type:** Speach-to-Text (ASR: Automatic Speech Recognition)
- **Think:** No
- **Base Model:** [openai/whisper-large-v3-turbo](https://huggingface.co/openai/whisper-large-v3-turbo)
- **Max Context Length:** NA
- **Default Context Length:** NA

▶️ Run with FastFlowLM in PowerShell:  

> ASR model requires to use with an LLM (load concurrently) for both CLI and Server Modes.

### CLI Mode   

Start with ASR enabled:

```powershell
flm run gemma3:4b --asr 1 # Load the ASR model (whisper-v3:turbo) in the background, with concurrent LLM loading (gemma3:4b).
```

Then, type (replace `filename.mp3` with your audio file path):

```powershell
/input "path/to/audio_sample.mp3" summarize it
```

### Server Mode 

Start with ASR enabled:

```powershell
flm serve gemma3:4b --asr 1 # # Load the ASR model (whisper-v3:turbo) in the background, with concurrent LLM loading (gemma3:4b).
```

Send audio to `POST /v1/audio/transcriptions` via any OpenAI client or Open WebUI.

**Example 1**: OpenAI client

```python
from openai import OpenAI

client = OpenAI(
    base_url="http://localhost:52625/v1",  # FastFlowLM's local API endpoint
    api_key="flm",  # Dummy key (FastFlowLM doesn’t require authentication)
)

with open("audio.mp3", "rb") as f:
    resp = client.audio.transcriptions.create(
        model="whisper-v3",
        file=f,
    )
    print(resp.text)
```

**Example 2**: Open WebUI

- Follow Open WebUI setup guide.
- In the bottom-left corner, click User icon, then select Settings.
- In the bottom panel, open Admin Settings.
- In the left sidebar, navigate to Audio.
- Set Speech-to-Text Engine to OpenAI.
- Enter:
> API Base URL: http://host.docker.internal:52625/v1  
> API KEY: flm (any value works)  
> STT Model: whisper-large-v3-turbo (type in the model name; can be different)  
- Save the setting.
- You're ready to upload audio files! (Choose an LLM to load and use concurrently)

---