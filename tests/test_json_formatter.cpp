#include "includes.h"
#include "test_sink.h"
#include <variant>

using spdlog::memory_buf_t;
using spdlog::field_entries;

// log to str and return it
template<typename... Args>
static std::string log_to_str(field_entries& ent, const Args &... args)
{
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    spdlog::logger oss_logger("json_tester", oss_sink);
    oss_logger.set_level(spdlog::level::info);

    oss_logger.set_formatter(std::unique_ptr<spdlog::formatter>(new spdlog::json_formatter(args...)));

    oss_logger.log(spdlog::source_loc{}, spdlog::level::info, "msg value", ent);
    return oss.str();
}

template<typename... Args>
static std::string scoped_log_to_str(field_entries& shared_ent, field_entries &ent, const Args &... args)
{
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto oss_logger_original = std::make_shared<spdlog::logger>("json_tester", oss_sink);
    auto oss_logger = oss_logger_original->scope("test", shared_ent);
    oss_logger->set_level(spdlog::level::info);

    oss_logger->set_formatter(std::unique_ptr<spdlog::formatter>(new spdlog::json_formatter(args...)));

    oss_logger->log(spdlog::source_loc{}, spdlog::level::info, "", ent);
    return oss.str();
}

TEST_CASE("json custom eol", "[json_formatter]")
{
    field_entries fields = {{"field", "Hello custom eol test"}};
    std::string eol = ";)";
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, ";)") == R"({"field":"Hello custom eol test"})" + eol);
}

TEST_CASE("json string", "[json_formatter]")
{
    field_entries fields = {{"field", std::string("string value")}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"string value"})");
}

TEST_CASE("json literal", "[json_formatter]")
{
    field_entries fields = {{"field", "string literal"}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"string literal"})");
}

TEST_CASE("json int", "[json_formatter]")
{
    field_entries fields = {{"field", static_cast<int>(42)}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"42"})");
}

TEST_CASE("json int64", "[json_formatter]")
{
    field_entries fields = {{"field", static_cast<int64_t>(42)}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"42"})");
}

TEST_CASE("json uint64", "[json_formatter]")
{
    field_entries fields = {{"field", static_cast<uint64_t>(42)}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"42"})");
}

TEST_CASE("json double", "[json_formatter]")
{
    field_entries fields = {{"field", static_cast<double>(42.0)}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"42.0"})");
}

TEST_CASE("json multiple", "[json_formatter]")
{
    field_entries fields = {
        {"field1", 42},
        {"field2", 43},
        {"field3", 44}
    };
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field1":"42", "field2":"43", "field3":"44"})");
}

TEST_CASE("json pattern", "[json_formatter]")
{
    field_entries fields = {{"field", "%l", true}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"info"})");
}

TEST_CASE("json msg pattern", "[json_formatter]")
{
    field_entries fields = {{"msg", "%v", true}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"msg":"msg value"})");
}


TEST_CASE("json non-pattern", "[json_formatter]")
{
    field_entries fields = {{"field", "%l", false}};
    REQUIRE(log_to_str(fields, spdlog::pattern_time_type::local, "") == R"({"field":"%l"})");
}

TEST_CASE("json scoped 0", "[json_formatter]")
{
    field_entries shared_fields = {};
    field_entries fields = {};
    REQUIRE(scoped_log_to_str(shared_fields, fields, spdlog::pattern_time_type::local, "") == R"({})");
}

TEST_CASE("json scoped 1", "[json_formatter]")
{
    field_entries shared_fields = {{"field", 42}};
    field_entries fields = {};
    REQUIRE(scoped_log_to_str(shared_fields, fields, spdlog::pattern_time_type::local, "") == R"({"field":"42"})");
}

TEST_CASE("json scoped both", "[json_formatter]")
{
    field_entries shared_fields = {{"field2", 43}};
    field_entries fields = {{"field1", 42}};
    REQUIRE(scoped_log_to_str(shared_fields, fields, spdlog::pattern_time_type::local, "") == R"({"field1":"42", "field2":"43"})");
}

// TODO(bhansen): implement these tests

// TEST_CASE("json clone-default-formatter", "[json_formatter]")
// {
//     auto formatter_1 = std::make_shared<spdlog::json_formatter>();
//     auto formatter_2 = formatter_1->clone();
//     std::string logger_name = "test";
//     spdlog::field_entries entries;
//     spdlog::details::log_msg msg(logger_name, spdlog::level::info, entries, "some message");

//     memory_buf_t formatted_1;
//     memory_buf_t formatted_2;
//     formatter_1->format(msg, formatted_1);
//     formatter_2->format(msg, formatted_2);

//     REQUIRE(fmt::to_string(formatted_1) == fmt::to_string(formatted_2));
// }

// TEST_CASE("json clone-default-formatter2", "[json_formatter]")
// {
//     auto formatter_1 = std::make_shared<spdlog::json_formatter>("%+");
//     auto formatter_2 = formatter_1->clone();
//     std::string logger_name = "test";
//     spdlog::field_entries entries;
//     spdlog::details::log_msg msg(logger_name, spdlog::level::info, entries, "some message");

//     memory_buf_t formatted_1;
//     memory_buf_t formatted_2;
//     formatter_1->format(msg, formatted_1);
//     formatter_2->format(msg, formatted_2);

//     REQUIRE(fmt::to_string(formatted_1) == fmt::to_string(formatted_2));
// }

// TEST_CASE("json clone-formatter", "[json_formatter]")
// {
//     auto formatter_1 = std::make_shared<spdlog::json_formatter>("%D %X [%] [%n] %v");
//     auto formatter_2 = formatter_1->clone();
//     std::string logger_name = "test";
//     spdlog::field_entries entries;
//     spdlog::details::log_msg msg(logger_name, spdlog::level::info, entries, "some message");

//     memory_buf_t formatted_1;
//     memory_buf_t formatted_2;
//     formatter_1->format(msg, formatted_1);
//     formatter_2->format(msg, formatted_2);
//     REQUIRE(fmt::to_string(formatted_1) == fmt::to_string(formatted_2));
// }

// TEST_CASE("json clone-formatter-2", "[json_formatter]")
// {
//     using spdlog::pattern_time_type;
//     auto formatter_1 = std::make_shared<spdlog::json_formatter>("%D %X [%] [%n] %v", pattern_time_type::utc, "xxxxxx\n");
//     auto formatter_2 = formatter_1->clone();
//     std::string logger_name = "test2";
//     spdlog::field_entries entries;
//     spdlog::details::log_msg msg(logger_name, spdlog::level::info, entries, "some message");

//     memory_buf_t formatted_1;
//     memory_buf_t formatted_2;
//     formatter_1->format(msg, formatted_1);
//     formatter_2->format(msg, formatted_2);
//     REQUIRE(fmt::to_string(formatted_1) == fmt::to_string(formatted_2));
// }

