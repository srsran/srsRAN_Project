/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/srslog/srslog.h"
#include "srsran/support/format_utils.h"

namespace srsran {

/// Class used to store common logging parameters for all types RLC entities.
/// It provides logging helpers, so that the UE index and LCID are always logged.
///
/// \param log_name name of the logger we want to use (e.g. RLC, PDCP, etc.)
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
template <typename Prefix>
class prefixed_logger
{
public:
  prefixed_logger(const std::string& log_name, Prefix prefix_) :
    logger(srslog::fetch_basic_logger(log_name, false)), prefix(prefix_)
  {
  }

  void   set_prefix(Prefix prefix_) { prefix = prefix_; }
  Prefix get_prefix() const { return prefix; }

  template <typename... Args>
  void log_debug(const char* fmt, Args&&... args) const
  {
    log_helper(logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(const char* fmt, Args&&... args) const
  {
    log_helper(logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(const char* fmt, Args&&... args) const
  {
    log_helper(logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(const char* fmt, Args&&... args) const
  {
    log_helper(logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void log(const srslog::basic_levels level, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case srslog::basic_levels::debug:
        log_debug(fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::info:
        log_info(fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::warning:
        log_warning(fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::error:
        log_error(fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::none:
        // skip
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  template <typename It, typename... Args>
  void log_debug(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_info(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_warning(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_error(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename It, typename... Args>
  void log(const srslog::basic_levels level, It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case srslog::basic_levels::debug:
        log_debug(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::info:
        log_info(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::warning:
        log_warning(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::error:
        log_error(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::none:
        // skip
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  template <typename... Args>
  void log_debug(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void log(const srslog::basic_levels level, uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case srslog::basic_levels::debug:
        log_debug(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::info:
        log_info(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::warning:
        log_warning(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::error:
        log_error(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case srslog::basic_levels::none:
        // skip
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  srslog::basic_logger& get_basic_logger() { return logger; }

private:
  srslog::basic_logger& logger;
  Prefix                prefix;

  template <typename... Args>
  void log_helper(srslog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    if (!channel.enabled()) {
      return;
    }
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel("{}{}", prefix, to_c_str(buffer));
  }

  template <typename It, typename... Args>
  void log_helper(It it_begin, It it_end, srslog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    if (!channel.enabled()) {
      return;
    }
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel(it_begin, it_end, "{}{}", prefix, to_c_str(buffer));
  }

  template <typename... Args>
  void log_helper(const uint8_t* msg, size_t len, srslog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    if (!channel.enabled()) {
      return;
    }
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel(msg, len, "{}{}", prefix, to_c_str(buffer));
  }
};

} // namespace srsran
