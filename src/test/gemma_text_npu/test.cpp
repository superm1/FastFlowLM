#include <iostream>
#include <cmath>
#include "utils/utils.hpp"
#include "utils/vm_args.hpp"
#include "AutoModel/modeling_gemma3_text.hpp"
#include "model_list.hpp"

// Model-specific factory function for Gemma3 Text family only
inline std::pair<std::string, std::unique_ptr<AutoModel>> get_gemma_text_model(const std::string& model_tag, xrt::device* npu_device_inst) {
    static std::unordered_set<std::string> gemma3_text_Tags = {
        "gemma3", "gemma3:270m", "gemma3:1b"
    };

    std::unique_ptr<AutoModel> auto_chat_engine = nullptr;
    std::string new_model_tag = model_tag;
    
    if (gemma3_text_Tags.count(model_tag))
        auto_chat_engine = std::make_unique<Gemma3_Text_Only>(npu_device_inst);
    else {
        new_model_tag = "gemma3:1b"; // Default to text-only model
        auto_chat_engine = std::make_unique<Gemma3_Text_Only>(npu_device_inst);
    }
  
    return std::make_pair(new_model_tag, std::move(auto_chat_engine));
}

int main(int argc, char* argv[]) {
    arg_utils::po::options_description desc("Allowed options");
    arg_utils::po::variables_map vm;
    arg_utils::add_default_options(desc);
    desc.add_options()("model,m", arg_utils::po::value<std::string>()->required(), "Model file");
    desc.add_options()("Short,s", arg_utils::po::value<bool>()->default_value(true), "Short Prompt");
    desc.add_options()("Preemption,p", arg_utils::po::value<bool>()->default_value(false), "Preemption");
    arg_utils::parse_options(argc, argv, desc, vm);

    std::string tag = vm["model"].as<std::string>();
    bool short_prompt = vm["Short"].as<bool>();
    bool preemption = vm["Preemption"].as<bool>();

    std::cout << "Model: " << tag << std::endl;
    std::string model_list_path = "model_list.json";
    std::string exe_dir = ".";
    model_list model_list(model_list_path, exe_dir);
   
    header_print("info", "Initializing chat model...");
    std::string model_path = model_list.get_model_path(tag);
    nlohmann::json model_info = model_list.get_model_info(tag);

    auto npu_device_global = xrt::device(0);

    // Use model-specific factory
    auto [actual_tag, chat] = get_gemma_text_model(tag, &npu_device_global);
    if (actual_tag != tag) {
        std::cout << "Model tag adjusted to: " << actual_tag << std::endl;
        model_path = model_list.get_model_path(actual_tag);
        model_info = model_list.get_model_info(actual_tag);
    }
   
    chat->load_model(model_path, model_info, -1, preemption);
    chat->set_topk(1);
    chat_meta_info_t meta_info;
    lm_uniform_input_t uniformed_input;

    if (short_prompt) {
        uniformed_input.prompt = "Hello! What is the capital of USA?";
        std::cout << "Prompt: " << uniformed_input.prompt << std::endl;
        std::cout << "Response: ";
        chat->start_total_timer();
        std::string response = chat->generate_with_prompt(meta_info, uniformed_input, 1024, std::cout);
        chat->stop_total_timer();
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << chat->show_profile() << std::endl;
        uniformed_input.prompt = "How far is it from Beijing?";
        std::cout << "Prompt: " << uniformed_input.prompt << std::endl;
        std::cout << "Response: " << std::endl;
        chat->start_total_timer();
        response = chat->generate_with_prompt(meta_info, uniformed_input, 1024, std::cout);
        chat->stop_total_timer();
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << chat->show_profile() << std::endl;
    }
    else{
        std::ifstream file("../../../../prompt.txt", std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Failed to open prompt file" << std::endl;
            return 1;
        }
        uniformed_input.prompt = "";
        file.seekg(0, std::ios::end);
        uniformed_input.prompt.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(uniformed_input.prompt.data(), uniformed_input.prompt.size());
        file.close();
        std::cout << "Prompt: " << uniformed_input.prompt << std::endl;
        std::cout << "Response: ";
        chat->start_total_timer();
        std::string response = chat->generate_with_prompt(meta_info, uniformed_input, 1024, std::cout);
        chat->stop_total_timer();
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << chat->show_profile() << std::endl;
    }
   
    std::pair<std::string, std::vector<int>> history = chat->get_history();
    std::cout << "History length: " << history.second.size() << std::endl;
    std::cout << std::endl;
    std::cout << "History: " << history.first << std::endl;

    std::cout << std::endl;

    return 0;
}