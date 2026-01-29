#pragma once
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <locale>
#include <random>
#include "nlohmann/json.hpp"
#include "AutoModel/automodel.hpp"

using json = nlohmann::ordered_json;

class PromptCache {
private:
    uint64_t checksum_;
    uint64_t _calculate_checksum(const void* p, size_t len, uint64_t sum = 0) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(p);
        uint64_t _sum = sum;

        const uint64_t* p64 = reinterpret_cast<const uint64_t*>(data);
        size_t blocks = len / sizeof(uint64_t);
        for (size_t i = 0; i < blocks; ++i) {
            _sum += p64[i];
        }

        const uint8_t* p8 = data + blocks * sizeof(uint64_t);
        size_t remain = len % sizeof(uint64_t);
        for (size_t i = 0; i < remain; ++i) {
            _sum += p8[i];
        }

        return _sum;
    }
public:
    PromptCache() : checksum_(0) {}
    bool can_use_cache(json& messages, chat_template_type_t template_type) {
        uint64_t check_sum_to_compare = 0;
        uint64_t new_checksum = 0;
        if (messages.size() > 2) {
            for (size_t i = 0; i < messages.size(); ++i) {
                const auto& msg = messages[i];
                const std::string role = msg.value("role", "");
                const std::string content = msg.value("content", "");

                bool has_tool_call = msg.contains("tool_calls");
                bool skip_this_message = (role == "tool") || has_tool_call;
                if (template_type == chat_template_type_t::harmony) {
                    skip_this_message = false; 
                }

                if (skip_this_message) continue;
                    
                if(i < messages.size() - 2)
                    check_sum_to_compare = _calculate_checksum(content.data(), content.size(), check_sum_to_compare);
                new_checksum = _calculate_checksum(content.data(), content.size(), new_checksum);
            }

            if (checksum_ == check_sum_to_compare) {
                checksum_ = new_checksum;
                return true;
            }
            else {
                checksum_ = new_checksum;
                return false;
            }
        }
        else {
            return false;
        }

    }

    void update_checksum(json& messages) {
        uint64_t new_checksum = 0;
        for (size_t i = 0; i < messages.size(); ++i) {
            const auto& msg = messages[i];
            const std::string content = msg.value("content", "");
            new_checksum = _calculate_checksum(content.data(), content.size(), new_checksum);
        }
        checksum_ = new_checksum;
    }

    /// @brief Reset the checksum to force cache miss
    /// @note This function increments the checksum value by 1 to ensure that
    ///       the next call to can_use_cache will result in a cache miss.
    void reset() {
        checksum_ = checksum_ + 1;
    }
};