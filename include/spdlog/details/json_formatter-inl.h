// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/details/json_formatter.h>
#endif

#include <spdlog/details/fmt_helper.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/formatter.h>
#include <spdlog/details/pattern_formatter.h>

#include <array>
#include <chrono>
#include <ctime>
#include <cctype>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace spdlog {
namespace details {


} // namespace details

SPDLOG_INLINE json_formatter::json_formatter(pattern_time_type time_type, std::string eol)
    : eol_(std::move(eol))
    , time_type_(time_type)
{
}

SPDLOG_INLINE void json_formatter::append_json_value(bool last_item, const details::log_msg &msg, const field_entry &entry, memory_buf_t &dest)
{
        //TODO(bhansen): don't format ints as strings
        //TODO(bhansen): \-encode non-valid characters (https://tools.ietf.org/html/rfc8259#section-7)
        //TODO(bhansen): don't output an item if it's already been emitted, with newer items taking precedence
        const char * separator = last_item ? "" : ", ";
        details::fmt_helper::append_string_view("\"", dest);
        details::fmt_helper::append_string_view(entry.name, dest);
        details::fmt_helper::append_string_view("\":\"", dest);
        if (entry.pattern)
        {
            //TODO(bhansen) cache these
            pattern_formatter pattern_fmt(entry.value, time_type_, "");
            pattern_fmt.format(msg, dest);
        }
        else
        {
            details::fmt_helper::append_string_view(entry.value.data(), dest);
        }
        details::fmt_helper::append_string_view("\"", dest);
        details::fmt_helper::append_string_view(separator, dest);
}

SPDLOG_INLINE void json_formatter::format(const details::log_msg &msg, memory_buf_t &dest)
{
    //TODO(bhansen): check if dest in nullptr

    details::fmt_helper::append_string_view("{" , dest);
    bool has_shared = (msg.shared_entries && !msg.shared_entries->empty());

    for (const auto & entry : msg.entries)
    {
        bool last_item = !has_shared && (&entry == &msg.entries.back());
        append_json_value(last_item, msg, entry, dest);
    }

    // Output shared entries
    if (msg.shared_entries)
    {
        for (const auto & entry : *msg.shared_entries)
        {
            bool last_item = &entry == &msg.shared_entries->back();
            append_json_value(last_item, msg, entry, dest);
        }
    }

    details::fmt_helper::append_string_view("}" , dest);
    details::fmt_helper::append_string_view(eol_, dest);
}

SPDLOG_INLINE std::unique_ptr<formatter> json_formatter::clone() const
{
    return details::make_unique<json_formatter>(time_type_, eol_);
}


} // namespace spdlog
