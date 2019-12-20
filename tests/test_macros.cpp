/*
 * This content is released under the MIT License as specified in https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE
 */

#include "includes.h"
#include "test_field_formatter.h"
#include "test_sink.h"
#include "utils.h"

#if SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_DEBUG
#error "Invalid SPDLOG_ACTIVE_LEVEL in test. Should be SPDLOG_LEVEL_DEBUG"
#endif

TEST_CASE("debug and trace w/o format string", "[macros]]")
{

    prepare_logdir();
    std::string filename = "test_logs/simple_log";

    auto logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger", filename);
    logger->set_pattern("%v");
    logger->set_level(spdlog::level::trace);

    SPDLOG_LOGGER_TRACE(logger, "Test message 1");
    SPDLOG_LOGGER_DEBUG(logger, "Test message 2");
    logger->flush();

    REQUIRE(ends_with(file_contents(filename), "Test message 2\n"));
    REQUIRE(count_lines(filename) == 1);

    spdlog::set_default_logger(logger);

    SPDLOG_TRACE("Test message 3");
    SPDLOG_DEBUG("Test message {}", 4);
    logger->flush();

    REQUIRE(ends_with(file_contents(filename), "Test message 4\n"));
    REQUIRE(count_lines(filename) == 2);
}

#ifndef SPDLOG_NO_STRUCTURED_LOGGING

namespace tags {
constexpr char FIELD_1[] = "field1";
constexpr char FIELD_2[] = "field2";
}

using namespace spdlog;

TEST_CASE("debug and trace w/ structured fields", "[macros]]")
{
    std::ostringstream oss;
    auto test_sink = std::make_shared<sinks::test_sink_st>();
    test_sink->set_formatter(std::make_unique<test_field_formatter>(details::os::default_eol));

    auto logger = std::make_shared<spdlog::logger>("orig", test_sink);
    logger->set_level(level::trace);

    SPDLOG_LOGGER_TRACE(logger, "Test message 1", f_(tags::FIELD_1, 1));
    SPDLOG_LOGGER_DEBUG(logger, "Test message 2", f_(tags::FIELD_2, 2));
    logger->flush();

    REQUIRE(test_sink->msg_counter() == 1);
    REQUIRE(test_sink->lines()[0] == "Test message 2:2");

    test_sink->reset_output();
    set_default_logger(logger);

    SPDLOG_DEBUG("Test message 3", f_(tags::FIELD_1, 1));
    SPDLOG_TRACE("Test message 4", f_(tags::FIELD_2, 2));
    logger->flush();
    REQUIRE(test_sink->lines().size() == 1);
    REQUIRE(test_sink->lines()[0] == "Test message 3:1");
}
#endif

TEST_CASE("disable param evaluation", "[macros]")
{
    SPDLOG_TRACE("Test message {}", throw std::runtime_error("Should not be evaluated"));
}

TEST_CASE("pass logger pointer", "[macros]")
{
    auto logger = spdlog::create<spdlog::sinks::null_sink_mt>("refmacro");
    auto &ref = *logger;
    SPDLOG_LOGGER_TRACE(&ref, "Test message 1");
    SPDLOG_LOGGER_DEBUG(&ref, "Test message 2");
}

// ensure that even if right macro level is on- don't evaluate if the logger's level is not high enough
// TEST_CASE("disable param evaluation2", "[macros]")
//{
//    auto logger = std::make_shared<spdlog::logger>("test-macro");
//    logger->set_level(spdlog::level::off);
//    int x = 0;
//    SPDLOG_LOGGER_DEBUG(logger, "Test message {}", ++x);
//    REQUIRE(x == 0);
//}
