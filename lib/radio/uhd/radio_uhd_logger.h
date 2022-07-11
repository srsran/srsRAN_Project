/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__
#include <uhd/utils/log.hpp>
#pragma GCC diagnostic pop

#ifdef UHD_LOG_INFO
#define Error(message) UHD_LOG_ERROR("UHD RF", message)
#define Warning(message) UHD_LOG_WARNING("UHD RF", message)
#define Info(message) UHD_LOG_INFO("UHD RF", message)
#define Debug(message) UHD_LOG_DEBUG("UHD RF", message)
#define Trace(message) UHD_LOG_TRACE("UHD RF", message)
#else
#define Error(message) UHD_LOG << message << std::endl
#define Warning(message) UHD_LOG << message << std::endl
#define Info(message) UHD_LOG << message << std::endl
#define Debug(message) UHD_LOG << message << std::endl
#define Trace(message) UHD_LOG << message << std::endl
#endif
