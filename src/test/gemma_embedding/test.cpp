#include <iostream>
#include <cmath>
#include "utils/utils.hpp"
#include "utils/vm_args.hpp"
#include "AutoEmbeddingModel/auto_embedding_model.hpp"
#include "model_list.hpp"
#include "AutoEmbeddingModel/modeling_gemma_embedding.hpp"
#include "metrices.hpp"

// Model-specific factory function for Gemma3 Text family only
inline std::pair<std::string, std::unique_ptr<AutoEmbeddingModel>> get_gemma_embedding_model(const std::string& model_tag, xrt::device* npu_device_inst) {
    static std::unordered_set<std::string> gemma_embedding_Tags = {
        "embed-gemma", "embed-gemma:300m"
    };

    std::unique_ptr<AutoEmbeddingModel> auto_embedding_engine = nullptr;
    std::string new_model_tag = model_tag;
    
    if (gemma_embedding_Tags.count(model_tag))
        auto_embedding_engine = std::make_unique<Gemma_Embedding>(npu_device_inst);
    else {
        new_model_tag = "embed-gemma:300m"; // Default to text-only model
        auto_embedding_engine = std::make_unique<Gemma_Embedding>(npu_device_inst);
    }
  
    return std::make_pair(new_model_tag, std::move(auto_embedding_engine));
}

int main(int argc, char* argv[]) {
    arg_utils::po::options_description desc("Allowed options");
    arg_utils::po::variables_map vm;
    desc.add_options()("model,m", arg_utils::po::value<std::string>()->required(), "Model file");
    desc.add_options()("Short,s", arg_utils::po::value<bool>()->default_value(true), "Short Prompt");
    desc.add_options()("Preemption,p", arg_utils::po::value<bool>()->default_value(false), "Preemption");
    
    arg_utils::po::store(arg_utils::po::parse_command_line(argc, argv, desc), vm);

    std::string tag = vm["model"].as<std::string>();
    bool short_prompt = vm["Short"].as<bool>();
    bool preemption = vm["Preemption"].as<bool>();

    std::cout << "Model: " << tag << std::endl;
    std::string exe_dir = utils::get_executable_directory();
    std::string model_dir = utils::get_models_directory();
    std::string model_list_path = exe_dir + "/model_list.json";
    model_list model_list(model_list_path, model_dir);
   
    header_print("info", "Initializing embedding model...");
    std::string model_path = model_list.get_model_path(tag);
    nlohmann::json model_info = model_list.get_model_info(tag);
    std::cout << "Model path: " << model_path << std::endl;
    std::cout << "Model info: " << model_info.dump() << std::endl;

    auto npu_device_global = xrt::device(0);

    // Use model-specific factory
    std::unique_ptr<AutoEmbeddingModel> embedding = std::make_unique<Gemma_Embedding>(&npu_device_global);
    model_path = model_list.get_model_path(tag);
    model_info = model_list.get_model_info(tag);

    embedding->load_model(model_path, model_info, preemption);

    std::string text = "Alice's Adventures in Wonderland ALICE'S ADVENTURES IN WONDERLAND Lewis Carroll THE MILLENNIUM FULCRUM EDITION 3.0 CHAPTER I Down the Rabbit-HoleAlice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do:  once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, and what is the use of a book,'thought Alice without pictures or conversation?' So she was considering in her own mind (as well as she could, for the hot day made her feel very sleepy and stupid), whether the pleasure of making a daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly a White Rabbit with pink eyes ran close by her. There was nothing so VERY remarkable in that; nor did Alice think it so VERY much out of the way to hear the Rabbit say to itself, `Oh dear!  Oh dear!  I shall be late!'";

    time_utils::time_point start_time = time_utils::now();
    auto y = embedding->embed(text, task_query);
    buffer<bf16> y_bf16(y.size());
    for (int i = 0; i < y.size(); i++){
        y_bf16[i] = (bf16)y[i];
    }
    time_utils::time_point end_time = time_utils::now();
    std::cout << "Time: " << time_utils::duration_ms(start_time, end_time).first << "ms" << std::endl;
    utils::print_matrix(y_bf16, 128);


    return 0;
}