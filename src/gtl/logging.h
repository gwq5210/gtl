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
#define GTL_TRACE(...) GTL_LOG(gtl::kLogLevelTrace, __VA_ARGS__)
#define GTL_DEBUG(...) GTL_LOG(gtl::kLogLevelDebug, __VA_ARGS__)
#define GTL_INFO(...) GTL_LOG(gtl::kLogLevelInfo, __VA_ARGS__)
#define GTL_WARN(...) GTL_LOG(gtl::kLogLevelWarn, __VA_ARGS__)
#define GTL_ERROR(...) GTL_LOG(gtl::kLogLevelError, __VA_ARGS__)
#define GTL_CRITICAL(CONDITION, ...) GTL_LOG(gtl::kLogLevelCritical, __VA_ARGS__)
#define GTL_TRACE_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelTrace, __VA_ARGS__)
#define GTL_DEBUG_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelDebug, __VA_ARGS__)
#define GTL_INFO_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelInfo, __VA_ARGS__)
#define GTL_WARN_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelWarn, __VA_ARGS__)
#define GTL_ERROR_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelError, __VA_ARGS__)
#define GTL_CRITICAL_IF(CONDITION, ...) GTL_LOG_IF(CONDITION, gtl::kLogLevelCritical, __VA_ARGS__)
#define GTL_CHECK(CONDITION, ...)               \
  do {                                          \
    GTL_CRITICAL_IF(!(CONDITION), __VA_ARGS__); \
    std::abort();                               \
  } while (false)

namespace gtl {

enum LogLevel : int {
  kLogLevelTrace = GTL_LEVEL_TRACE,
  kLogLevelDebug = GTL_LEVEL_DEBUG,
  kLogLevelInfo = GTL_LEVEL_INFO,
  kLogLevelWarn = GTL_LEVEL_WARN,
  kLogLevelError = GTL_LEVEL_ERROR,
  kLogLevelCritical = GTL_LEVEL_CRITICAL,
  kLogLevelOff = GTL_LEVEL_OFF,
  kLogLevelCount,
};

}  // namespace gtl