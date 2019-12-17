// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>
#include <spdlog/formatter.h>

#include <chrono>
#include <ctime>
#include <memory>

#include <string>
#include <vector>

namespace spdlog {
namespace details {

} //namespace details

class json_formatter final : public formatter
{
public:
    explicit json_formatter(
        pattern_time_type time_type = pattern_time_type::local, std::string eol = spdlog::details::os::default_eol);

    json_formatter(const json_formatter &other) = delete;
    json_formatter &operator=(const json_formatter &other) = delete;

    std::unique_ptr<formatter> clone() const override;
    void format(const details::log_msg &msg, memory_buf_t &dest) override;
private:
    std::string eol_;
    pattern_time_type time_type_;

    void append_json_value(bool last_item, const details::log_msg &msg, const field_entry &entry, memory_buf_t &dest);
};
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#include "json_formatter-inl.h"
#endif
