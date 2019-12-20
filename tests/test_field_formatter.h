//
// Copyright(c) 2018 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

#include "spdlog/formatter.h"
#include "spdlog/details/log_msg.h"

#include <memory>

constexpr char sep[] = ":";

namespace spdlog {

class test_field_formatter : public spdlog::formatter
{
public:
    test_field_formatter(std::string eol = "") : eol_ (eol) {}

    void format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest) override
    {
        // Always put in the payload
        dest.append(msg.payload.data(), msg.payload.data() + msg.payload.size());

        // Append any fields that are present
        for (const auto & entry : msg.entries)
        {
            dest.append(sep, sep + 1);
            dest.append(entry.value.c_str(), entry.value.c_str() + entry.value.size());
        }

        if (msg.shared_entries)
        {
            for (const auto & entry : *msg.shared_entries)
            {
                dest.append(sep, sep + 1);
                dest.append(entry.value.c_str(), entry.value.c_str() + entry.value.size());
            }
        }
        dest.append(eol_.c_str(), eol_.c_str() + eol_.size());
    }

    std::unique_ptr<spdlog::formatter> clone() const override {return std::make_unique<test_field_formatter>(eol_); }

protected:
    std::string eol_;
};

} // namespace spdlog
