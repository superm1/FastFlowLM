/// \file utils.cpp
/// \brief utils class
/// \author FastFlowLM Team
/// \date 2025-06-24
/// \version 0.9.24
/// 
/// \note This file contains some utility functions for the FastFlowLM project.
#include "utils/utils.hpp"
#include <filesystem>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace utils {

std::string find_model_list() {
    std::string install_prefix = CMAKE_INSTALL_PREFIX;

    // 1. Check FLM_CONFIG_PATH environment variable
    const char* env_path = std::getenv("FLM_CONFIG_PATH");
    if (env_path && *env_path) {
        if (std::filesystem::exists(env_path)) {
            return env_path;
        }
    }

    // 2. Check for install prefix
    std::string installed_path = install_prefix + "/share/flm/model_list.json";
    if (std::filesystem::exists(installed_path)) {
        return installed_path;
    }

    // 3. Check relative to executable
    std::string exe_dir = get_executable_directory();
    std::string exe_relative_path = exe_dir + "/model_list.json";
    if (std::filesystem::exists(exe_relative_path)) {
        return exe_relative_path;
    }

    // 4. Check current working directory
    if (std::filesystem::exists("model_list.json")) {
        return "model_list.json";
    }

    // If not found, throw an error
    throw std::runtime_error("model_list.json not found. Please set FLM_CONFIG_PATH or place it next to the executable.");
}

std::string get_executable_directory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exe_path(buffer);
    size_t last_slash = exe_path.find_last_of("\\");
    if (last_slash != std::string::npos) {
        return exe_path.substr(0, last_slash);
    }
    return ".";
#else
    char buffer[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';
        std::string exe_path(buffer);
        size_t last_slash = exe_path.find_last_of("/");
        if (last_slash != std::string::npos) {
            return exe_path.substr(0, last_slash);
        }
    }
    return ".";
#endif
}

std::string get_user_documents_directory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, buffer))) {
        return std::string(buffer);
    }
    // Fallback to current directory if Documents folder cannot be found
    return ".";
#else
    const char* home = std::getenv("HOME");
    if (home && *home) {
        return std::string(home) + "/.config";
    }
    return ".";
#endif
}

int get_server_port(int user_port) {
    if (user_port > 0 && user_port <= 65535) {
        return user_port;
    }
    else {
#ifdef _WIN32
        char* port_env = nullptr;
        size_t len = 0;
        if (_dupenv_s(&port_env, &len, "FLM_SERVE_PORT") == 0 && port_env != nullptr) {
            try {
                int port = std::stoi(port_env);
                free(port_env);
                if (port > 0 && port <= 65535) {
                    return port;
                }
            }
            catch (const std::exception&) {
                free(port_env);
                // Invalid port number, use default
            }
        }
#else
        const char* port_env = std::getenv("FLM_SERVE_PORT");
        if (port_env && *port_env) {
            try {
                int port = std::stoi(port_env);
                if (port > 0 && port <= 65535) {
                    return port;
                }
            }
            catch (const std::exception&) {
                // Invalid port number, use default
            }
        }
#endif
    }

    return 52625; // Default port
}

std::string get_models_directory() {
#ifdef _WIN32
    char* model_path_env = nullptr;
    size_t len = 0;
    if (_dupenv_s(&model_path_env, &len, "FLM_MODEL_PATH") == 0 && model_path_env != nullptr) {
        std::string custom_path(model_path_env);
        free(model_path_env);
        if (!custom_path.empty()) {
            return custom_path;
        }
    }
#else
    const char* model_path_env = std::getenv("FLM_MODEL_PATH");
    if (model_path_env && *model_path_env) {
        return std::string(model_path_env);
    }
#endif
    // Fallback to Documents/flm/models on Windows or ~/.config/flm on Linux if environment variable is not set
    std::string documents_dir = get_user_documents_directory();
#ifdef _WIN32
    return documents_dir + "/flm/models";
#else
    return documents_dir + "/flm";
#endif
}

} // end of namespace utils
