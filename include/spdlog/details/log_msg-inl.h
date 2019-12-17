// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/details/log_msg.h>
#endif

#include <spdlog/details/os.h>

namespace spdlog {
namespace details {

SPDLOG_INLINE log_msg::log_msg(
    spdlog::source_loc loc, string_view_t a_logger_name, spdlog::level::level_enum lvl, field_entries_ptr shared_entries, spdlog::string_view_t msg)
    : logger_name(a_logger_name)
    , level(lvl)
#ifndef SPDLOG_NO_STRUCTURED_LOGGING
    , shared_entries(shared_entries)
#endif
    , time(os::now())
#ifndef SPDLOG_NO_THREAD_ID
    , thread_id(os::thread_id())
#endif
    , source(loc)
    , payload(msg)
{}

SPDLOG_INLINE log_msg::log_msg(string_view_t a_logger_name, spdlog::level::level_enum lvl, field_entries_ptr shared_entries, spdlog::string_view_t msg)
    : log_msg(source_loc{}, a_logger_name, lvl, shared_entries, msg)
{}

SPDLOG_INLINE log_msg::log_msg(
    spdlog::source_loc loc, string_view_t a_logger_name, spdlog::level::level_enum lvl, field_entries &src_entries, field_entries_ptr shared_entries, spdlog::string_view_t msg)
    : logger_name(a_logger_name)
    , level(lvl)
#ifndef SPDLOG_NO_STRUCTURED_LOGGING
    , entries(std::move(src_entries))
    , shared_entries(shared_entries)
#endif
    , time(os::now())
#ifndef SPDLOG_NO_THREAD_ID
    , thread_id(os::thread_id())
#endif
    , source(loc)
    , payload(msg)
{}

SPDLOG_INLINE log_msg::log_msg(
    string_view_t a_logger_name, spdlog::level::level_enum lvl, field_entries &src_entries, field_entries_ptr shared_entries, spdlog::string_view_t msg)
    : log_msg(source_loc{}, a_logger_name, lvl, src_entries, shared_entries, msg)
{}

} // namespace details
} // namespace spdlog
