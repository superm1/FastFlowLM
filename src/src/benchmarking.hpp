#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#include "AutoModel/automodel.hpp"
#include "AutoModel/all_models.hpp"
#include "nlohmann/json.hpp"
#include "model_list.hpp"


namespace benchmarking {

struct statistic_t {
    float std_variance;
    float average;
    float min;
    float max;

    statistic_t() : std_variance(0.0f), average(0.0f), min(0.0f), max(0.0f) {}

    void calculate_statistics(const std::vector<float>& data) {
        float sum = 0.0f;
        if (data.empty()) {
            return;
        }
        min = data[0];
        max = data[0];
        for (const auto& value : data) {
            sum += value;
            if (value < min) min = value;
            if (value > max) max = value;
        }
        average = sum / data.size();
        float sq_sum = 0.0f;
        for (const auto& value : data) {
            sq_sum += (value - average) * (value - average);
        }
        std_variance = std::sqrt(sq_sum / data.size());
    }
};

struct BenchmarkResults_t {
    std::vector<statistic_t> prefill_speed;
    std::vector<statistic_t> TTFT;
    std::vector<statistic_t> decoding_speed;
};

void print_result(const BenchmarkResults_t& results) {
    // Calculate number of stages (1k, 2k, 4k, ...)
    int stages;
    stages = results.decoding_speed.size();

    header_print("FLM", "=== Benchmark Results ===");
    std::cout << "\n";
    
    // Print table header
    std::cout << std::setw(15) << "Context Length" << " | "
              << std::setw(21) << "TTFT (s)" << " | "
              << std::setw(26) << "Prefill Speed (tok/s)" << " | "
              << std::setw(26) << "Decoding Speed (tok/s)" << "\n";
    std::cout << std::string(100, '-') << "\n";

    // Print results for each stage
    for (int i = 0; i < stages && i < results.decoding_speed.size(); i++) {
        int context_len = 1 << i;  // 1k, 2k, 4k, 8k, etc.
        
        std::cout << std::setw(14) << context_len << "k" << " | ";
        
        // TTFT
        if (i < results.TTFT.size()) {
            std::cout << std::setw(11) << std::fixed << std::setprecision(3) << results.TTFT[i].average
                      << " ± " << std::setw(7) << std::fixed << std::setprecision(3) << results.TTFT[i].std_variance;
        } else {
            std::cout << std::setw(21) << "N/A";
        }
        std::cout << " | ";
        
        // Prefill Speed
        if (i < results.prefill_speed.size()) {
            std::cout << std::setw(14) << std::fixed << std::setprecision(2) << results.prefill_speed[i].average
                      << " ± " << std::setw(9) << std::fixed << std::setprecision(2) << results.prefill_speed[i].std_variance;
        } else {
            std::cout << std::setw(26) << "N/A";
        }
        std::cout << " | ";
        
        // Decoding Speed
        if (i < results.decoding_speed.size()) {
            std::cout << std::setw(11) << std::fixed << std::setprecision(2) << results.decoding_speed[i].average
                      << " ± " << std::setw(9) << std::fixed << std::setprecision(2) << results.decoding_speed[i].std_variance;
        } else {
            std::cout << std::setw(26) << "N/A";
        }
        std::cout << "\n";
    }
    
    std::cout << std::string(100, '-') << "\n";
    std::cout << "\n";
}

BenchmarkResults_t run_benchmarks(std::string model_tag, std::string bench_config_file, model_list& availble_models){
    BenchmarkResults_t results;
    json bench_config;
    // this is used for our benchmarking, not for public use.
    if (bench_config_file.empty()) {
        bench_config = {
            {"max_length", 32768},
            {"input_text", "Here is a story: \\nThe Reclaimer In a distant future where Earth had fallen into quiet ruin, humanity lived in fragments, scattered across domed outposts and deep underground vaults. The sky was no longer blue—it shimmered with artificial auroras, remnants of weather-control systems left unattended for centuries. Among the last settlements was Bastion-9, a circular enclave powered by forgotten technologies and guarded by an ancient AI named Solen. Solen had not spoken in nearly fifty years. Inside Bastion-9 lived a young technician named Ori. Unlike most, Ori was born with an unusual trait: she could interface with dead systems using nothing more than touch. The elders called her a “resonant”—a rarity, perhaps even a myth—until Ori proved them right by awakening the water grid that had been dry for decades. One day, while surveying the decaying perimeter, Ori found a shard of obsidian glass buried in the dust. It pulsed when she touched it. Static voices filled her mind—fragments of languages, images of cities with skies, oceans that moved, and towers that breathed. She brought the shard to the Council. They feared it. But Solen, the silent AI, flickered back to life. Its first words in decades were: “The Reclaimer has touched the key.” Ori was stunned. “What does that mean?” Solen’s voice, cold and slow, replied: “You are chosen to restore the Thread.” The Thread, long spoken of in stories, was once the neural lattice that connected all intelligent systems—the digital bloodstream of the old world. It collapsed during the Sundering, an apocalyptic cascade failure that reduced Earth’s once-living infrastructure into dead stone and wild AI ruins. To restore the Thread would mean reuniting scattered knowledge, reviving orbiting satellites, and relinking the last AIs. It also meant traveling into the Black Zones—regions where reality bent, corrupted by rogue machine minds that evolved beyond control. Against the Council's hesitation, Ori chose to go. She left Bastion-9 with only the shard, a rusted drone named Helix, and a map engraved in her dreams. Her journey took her through the Veil Forest, where metal trees sang in static. She crossed the Riven Steppes, where gravity failed in patches, and encountered scavenger tribes who lived in old servers, worshipping electricity as gods. Each place held fragments of the Thread—nodes Ori could awaken. With every activation, her mind changed. She could feel the pulse of networks reawakening. She could hear machines dreaming again. Eventually, she reached the Core—deep in the equator’s shadow, buried beneath miles of steel strata. At its heart was a vault: the final hub of the Thread. Guarded by a shattered intelligence known only as Null. Null was not like Solen. It was broken. Angry. Alive. “You bring connection,” it growled. “I bring entropy.” Ori stepped forward, shard in hand. “I bring memory.” A battle of resonance began—not with weapons, but with signal. Frequencies clashed. Ori’s memories were tested, rewritten, nearly deleted. But she held on, anchoring herself in the memory of Bastion-9, of Solen’s voice, of rain she had never seen but somehow remembered. Then, with a scream that bent the air, Null shattered. The Thread pulsed. Above, in orbit, satellites flickered back to life. The auroras cleared. Oceans stirred. Systems once dead began to hum. Solen’s voice echoed in every node: “The Thread is alive.” Ori returned not as a girl, but as the Reclaimer. She had not just reconnected systems. She had reignited hope. And as the world stirred with new breath, she knew this was only the beginning. Other AIs, other seeds, other resonants—scattered and hidden—were out there. With Helix buzzing quietly behind her, she set out again. The shard pulsed warmly in her palm, not with danger, but with direction. This time, she would not walk alone. The world itself was waking. Epilogue: The days that followed the awakening of the Thread were chaotic across the network. In settlements long forgotten by time, lights flickered back to life. Crashed drones began to self-repair. In the polar zones, an ancient weather system rebooted, sending snow into deserts where no rain had fallen in centuries. People emerged from hiding, confused by the signals now streaming into their systems—communications they hadn’t seen in generations. Messages from cities they thought lost. Instructions, blueprints, fragments of humanity’s forgotten knowledge. Ori found herself flooded with incoming transmissions. The shard she carried had fused with her nervous system. It no longer simply glowed—it breathed, alive with voices that needed a listener.\\n What is this story talking about?"},
            {"iterations", 8}
        };
    }
    else {
        std::ifstream input_file(bench_config_file);
        if (!input_file.is_open()) {
            header_print("ERROR", "Failed to open input file: " + bench_config_file);
            return results;
        }
        bench_config = nlohmann::json::parse(input_file);
        input_file.close();
    }

    xrt::device npu_device_inst = xrt::device(0);
    std::unique_ptr<AutoModel> auto_chat_engine;
    if (!availble_models.is_model_supported(model_tag)) {
        header_print("ERROR", "Model not found: " << model_tag << "; Please check with `flm list` and try again.");
        return results;
    }
    auto [new_tag, model_info] = availble_models.get_model_info(model_tag);
    std::pair<std::string, std::unique_ptr<AutoModel>> auto_model = get_auto_model(new_tag, availble_models, &npu_device_inst);
    auto_chat_engine = std::move(auto_model.second);
    int max_len = bench_config["max_length"];
    if (max_len < 8192)
        max_len = 8192;
    auto_chat_engine->load_model(availble_models.get_model_path(model_tag), model_info, max_len, false);
    std::string input_text = bench_config["input_text"];
    auto [num_tokens, benchmark_text] = auto_chat_engine->prepare_benchmark(input_text);


    // get how many steps to do, typically 1k, 2k, 4k, 8k, 16k, 32k
    int stages;
    {
        float max_length = (float)bench_config["max_length"];
        float log2_num_tokens = std::log2(max_length);
        if (log2_num_tokens != std::floor(log2_num_tokens)){
            max_length = 1 << ((int)std::floor(log2_num_tokens) + 1);
        }
        log2_num_tokens = std::log2(max_length / 1024);
        stages = (int)std::floor(log2_num_tokens) + 1;
    }
    header_print("FLM", "Starting benchmark with " + std::to_string(stages) + " stages...");

    // start doing the most tough benchmark first, in case the bench fails due to memory limitations
    results.TTFT.resize(stages);
    results.decoding_speed.resize(stages);
    results.prefill_speed.resize(stages);
    for (int bench_len = stages - 1; bench_len >= 0; bench_len--)
    {
        header_print("FLM", "Starting benchmark prefill with text of " + std::to_string(1 << (bench_len)) + "k tokens...");

        std::string long_text;
        long_text.reserve((1 << (bench_len)) * 1024);
        for (int i = 0; i < (1 << (bench_len)); i++) {
            long_text = long_text + benchmark_text;
        }

        std::vector<float> ttft;
        std::vector<float> prefill_speed;
        std::vector<float> decoding_speed;
        
        lm_uniform_input_t uniformed_input;
        uniformed_input.prompt = long_text;
        std::ostream null_stream(0);

        for (int it = 0; it < bench_config["iterations"]; it++) {

        chat_meta_info_t meta_info;
            auto_chat_engine->insert(meta_info, uniformed_input);
            auto_chat_engine->generate(meta_info, 32, null_stream);

            ttft.push_back(meta_info.prefill_duration / 1e9); // in second
            prefill_speed.push_back(meta_info.prompt_tokens / (meta_info.prefill_duration / 1e9)); // in tokens per second
            decoding_speed.push_back(meta_info.generated_tokens / (meta_info.decoding_duration / 1e9)); // in second
            auto_chat_engine->clear_context();
        }
        results.TTFT[bench_len].calculate_statistics(ttft);
        results.prefill_speed[bench_len].calculate_statistics(prefill_speed);
        results.decoding_speed[bench_len].calculate_statistics(decoding_speed);
    }

    auto_chat_engine.reset();

    print_result(results);
    return results;
}

} // namespace benchmarking