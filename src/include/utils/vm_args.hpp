/// \file vm_args.hpp
/// \brief vm_args class
/// \author FastFlowLM Team
/// \date 2025-06-24
/// \version 0.9.21
/// \note This class is used to parse the command line arguments.
#pragma once

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

namespace arg_utils {

namespace po = boost::program_options;

/// \brief Structure to hold parsed command line arguments
struct ParsedArgs {
    std::string command;
    std::string model_tag;
    std::string power_mode;
    std::string list_filter;
    bool force_redownload;
    bool version_requested;
    bool port_requested;
    bool quiet_list;
    bool preemption;
    int ctx_length;
    size_t max_socket_connections;
    size_t max_npu_queue;
    int port;
    bool cors;
    bool asr;
    bool embed;
    ParsedArgs() : power_mode("performance"), force_redownload(false), 
                   version_requested(false), port_requested(false),
                   quiet_list(false) {}
};

/// \brief parse the options using Boost Program Options with positional arguments
/// \param argc the number of arguments
/// \param argv the arguments
/// \param parsed_args reference to store parsed arguments
/// \return true if parsing was successful, false otherwise
bool parse_options(int argc, char *argv[], ParsedArgs& parsed_args) {
    try {
        // Define the command line options
        po::options_description general("Allowed options");
        general.add_options()
            ("help,h", "Show help message")
            ("version,v", "Show version information")
            ("pmode", po::value<std::string>(&parsed_args.power_mode)->default_value("performance"),
             "Set power mode: powersaver, balanced, performance, turbo")
            ("asr,a", po::value<bool>(&parsed_args.asr)->default_value(0),
             "If load asr model")
            ("embed,e", po::value<bool>(&parsed_args.embed)->default_value(0),
            "If load embed model")
            ("port,p", po::value<int>(&parsed_args.port)->default_value(-1), 
             "Set the server port number (for serve command)")
            ("force", po::bool_switch(&parsed_args.force_redownload),
             "Force re-download even if model exists (for pull command)")
            ("filter", po::value<std::string>(&parsed_args.list_filter)->default_value("all"),
             "Show models: all | installed | not-installed")
            ("quiet", "Hide emojis in the model list (can be used with --filter)")
            ("ctx-len,c", po::value<int>(&parsed_args.ctx_length)->default_value(-1),
             "Set context length")
            ("socket,s", po::value<size_t>(&parsed_args.max_socket_connections)->default_value(10),
            "Set the maximum number of socket connections allowed (for serve command)")
            ("q-len,q", po::value<size_t>(&parsed_args.max_npu_queue)->default_value(10),
            "Set number of max npu queue length (for serve command)")
            ("cors", po::value<bool>(&parsed_args.cors)->default_value(1),
             "Enable or disable Cross-Origin Resource Sharing (CORS) (for serve command)")
            ("preemption", po::value<bool>(&parsed_args.preemption)->default_value(false),
             "Enable preemption");

        // Define positional arguments
        po::positional_options_description pos_desc;
        pos_desc.add("command", 1);
        pos_desc.add("model_tag", 1);

        // Define hidden options for positional arguments
        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("command", po::value<std::string>(&parsed_args.command), "Command to execute")
            ("model_tag", po::value<std::string>(&parsed_args.model_tag), "Model tag");

        // Combine all options
        po::options_description all_options;
        all_options.add(general).add(hidden);

        // Parse command line
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                  .options(all_options)
                  .positional(pos_desc)
                  .run(), vm);
        po::notify(vm);

        // Check for help or version flags
        if (vm.count("help")) {
            // Custom help formatting to match the desired style
            std::cout << "Usage: " << argv[0] << " <command> [options] [model_tag]" << std::endl;
            std::cout << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  run <model_tag>     - Run the model interactively" << std::endl;
            std::cout << "  serve <model_tag>   - Start the  server" << std::endl;
            std::cout << "  pull <model_tag>    - Download model files if not present" << std::endl;
            std::cout << "  remove <model_tag>  - Remove a model" << std::endl;
            std::cout << "  list                - List all available models" << std::endl;
            std::cout << "  version             - Show version information" << std::endl;
            std::cout << "  help                - Show this help message" << std::endl;
            std::cout << "  port                - Show the default server port" << std::endl;
            std::cout << std::endl;
            std::cout << general << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  " << argv[0] << " run llama3.2:1b" << std::endl;
            std::cout << "  " << argv[0] << " run llama3.2:1b --asr 1" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --pmode balanced" << std::endl;
            std::cout << "  " << argv[0] << " pull llama3.2:1b --force" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --ctx-len 8192" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --socket 10" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --q-len 10" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --port 8000" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --cors 0" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --asr 1" << std::endl;
            std::cout << "  " << argv[0] << " serve llama3.2:1b --embed 1" << std::endl;
            std::cout << "  " << argv[0] << " list" << std::endl;
            std::cout << "  " << argv[0] << " list --quiet" << std::endl;
            std::cout << "  " << argv[0] << " list --filter installed" << std::endl;
            return false; // Exit after showing help
        }

        if (vm.count("version")) {
            parsed_args.version_requested = true;
            return true; // Exit after showing help
        }
       

        // Extract command and model_tag from positional arguments
        if (vm.count("command")) {
            parsed_args.command = vm["command"].as<std::string>();
            
            // Handle help and version commands directly
            if (parsed_args.command == "help") {
                // Custom help formatting to match the desired style
                std::cout << "Usage: " << argv[0] << " <command> [options] [model_tag]" << std::endl;
                std::cout << std::endl;
                std::cout << "Commands:" << std::endl;
                std::cout << "  run <model_tag>     - Run the model interactively" << std::endl;
                std::cout << "  serve <model_tag>   - Start the Ollama-compatible server" << std::endl;
                std::cout << "  pull <model_tag>    - Download model files if not present" << std::endl;
                std::cout << "  remove <model_tag>  - Remove a model" << std::endl;
                std::cout << "  list                - List all available models" << std::endl;
                std::cout << "  version             - Show version information" << std::endl;
                std::cout << "  help                - Show this help message" << std::endl;
                std::cout << "  port                - Show the default server port" << std::endl;
                std::cout << std::endl;
                std::cout << general << std::endl;
                std::cout << "Examples:" << std::endl;
                std::cout << "  " << argv[0] << " run llama3.2:1b" << std::endl;
                std::cout << "  " << argv[0] << " run llama3.2:1b --asr 1" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --pmode balanced" << std::endl;
                std::cout << "  " << argv[0] << " pull llama3.2:1b --force" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --ctx-len 8192" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --socket 10" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --q-len 10" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --port 8000" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --cors 0" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --asr 1" << std::endl;
                std::cout << "  " << argv[0] << " serve llama3.2:1b --embed 1" << std::endl;
                std::cout << "  " << argv[0] << " list" << std::endl;
                std::cout << "  " << argv[0] << " list --quiet" << std::endl;
                std::cout << "  " << argv[0] << " list --filter installed" << std::endl;
                return false; // Exit after showing help
            }
            
            if (parsed_args.command == "version") {
                parsed_args.version_requested = true;
                return true;
            }
            if (parsed_args.command == "port") {
                parsed_args.port_requested = true;
                return true;
            }
            if (parsed_args.command == "list") {
                if (vm.count("quiet")) {
                    parsed_args.quiet_list = true;
                }
                return true;
            }
        } else {
            std::cerr << "Error: Command is required" << std::endl;
            return false;
        }

        if (vm.count("model_tag")) {
            parsed_args.model_tag = vm["model_tag"].as<std::string>();
        }

        // Validate command-specific requirements
        if (parsed_args.command == "run" || parsed_args.command == "pull" || parsed_args.command == "remove") {
            if (parsed_args.model_tag.empty()) {
                std::cerr << "Error: Model tag is required for command '" << parsed_args.command << "'" << std::endl;
                return false;
            }
        }
        if (parsed_args.command != "serve")
        {
            if (!vm["socket"].defaulted())
            {
                std::cerr << "Error: Max socket connections is only required for serve command! " << std::endl;
                return false;
            }
            if (!vm["q-len"].defaulted())
            {
                std::cerr << "Error: Max npu queue length is only required for serve command! " << std::endl;
                return false;
            }
            if (!vm["port"].defaulted())
            {
                std::cerr << "Error: The port number option is only supported with the serve command! " << std::endl;
                return false;
            }
            if (!vm["cors"].defaulted())
            {
                std::cerr << "Error: The cors option is only supported with the serve command! " << std::endl;
                return false;
            }
        }
        // Note: serve command allows empty model_tag (will use default)

        // Validate power mode for run/serve commands
        if ((parsed_args.command == "run" || parsed_args.command == "serve") && 
            !parsed_args.power_mode.empty()) {
            const std::vector<std::string> valid_modes = {"default", "powersaver", "balanced", "performance", "turbo"};
            if (std::find(valid_modes.begin(), valid_modes.end(), parsed_args.power_mode) == valid_modes.end()) {
                std::cerr << "Error: Invalid power mode '" << parsed_args.power_mode << "'" << std::endl;
                std::cerr << "Valid power modes: default, powersaver, balanced, performance, turbo" << std::endl;
                return false;
            }
            //if(parsed_args.model_tag == "")
        }

        return true;

    } catch (const std::exception &ex) {
        std::cerr << "Error parsing arguments: " << ex.what() << std::endl;
        return false;
    }
}


}
