#pragma once

#include <cstdlib>

#include "spdlog/spdlog.h"

#define GTL_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#define GTL_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#define GTL_LEVEL_INFO SPDLOG_LEVEL_INFO
#define GTL_LEVEL_WARN SPDLOG_LEVEL_WARN
#define GTL_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#define GTL_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL
#define GTL_LEVEL_OFF SPDLOG_LEVEL_OFF

#define GTL_DEFAULT_LOGGER (spdlog::default_logger_raw())

#define GTL_LOG(LEVEL, ...) SPDLOG_LOGGER_CALL(GTL_DEFAULT_LOGGER, spdlog::level::level_enum(LEVEL), __VA_ARGS__)
#define GTL_LOG_IF(CONDITION, LEVEL, ...) (CONDITION) ? GTL_LOG(LEVEL, __VA_ARGS__) : (void)0
#define GTL_TRACE(...) GTL_LOG(gtl::kTrace, __VA_ARGS__)
#define GTL_DEBUG(...) GTL_LOG(gtl::kDebug, __VA_ARGS__)
#define GTL_INFO(...) GTL_LOG(gtl::kInfo, __VA_ARGS__)
#define GTL_WARN(...) GTL_LOG(gtl::kWarn, __VA_ARGS__)
#define GTL_ERROR(...) GTL_LOG(gtl::kError, __VA_ARGS__)
#define GTL_CRITICAL(CONDITION, ...) GTL_LOG(gtl::kCritical, __VA_ARGS__)
#define GTL_TRACE_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kTrace, __VA_ARGS__)
#define GTL_DEBUG_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kDebug, __VA_ARGS__)
#define GTL_INFO_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kInfo, __VA_ARGS__)
#define GTL_WARN_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kWarn, __VA_ARGS__)
#define GTL_ERROR_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kError, __VA_ARGS__)
#define GTL_CRITICAL_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kCritical, __VA_ARGS__)
#define GTL_CHECK_LOG(CONDITION, FMT, ...)                                             \
  do {                                                                                 \
  } while (false)
    // GTL_CRITICAL_IF(!(CONDITION), "check (" #CONDITION ") failed, " FMT, __VA_ARGS__);
#define GTL_CHECK(CONDITION) GTL_CHECK_LOG(CONDITION, "z")
#define GTL_SET_LEVEL(LEVEL) spdlog::set_level(spdlog::level::level_enum(LEVEL))

namespace gtl {

enum LogLevel : int {
  kTrace = GTL_LEVEL_TRACE,
  kDebug = GTL_LEVEL_DEBUG,
  kInfo = GTL_LEVEL_INFO,
  kWarn = GTL_LEVEL_WARN,
  kError = GTL_LEVEL_ERROR,
  kCritical = GTL_LEVEL_CRITICAL,
  kOff = GTL_LEVEL_OFF,
  kCount,
};

}  // namespace gtl