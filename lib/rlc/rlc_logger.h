/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_LOGGER_H
#define SRSGNB_RLC_LOGGER_H

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

/// Class used to store common logging parameters for all types RLC entities.
/// It provides logging helpers, so that the UE index and LCID are always logged.
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_logger
{
public:
  rlc_logger(du_ue_index_t du_index, lcid_t lcid) :
    du_index(du_index), lcid(lcid), logger(srslog::fetch_basic_logger("RLC", false))
  {}

  template <typename... Args>
  void log_debug(const char* fmt, Args&&... args)
  {
    log_helper(logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(const char* fmt, Args&&... args)
  {
    log_helper(logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(const char* fmt, Args&&... args)
  {
    log_helper(logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(const char* fmt, Args&&... args)
  {
    log_helper(logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename It, typename... Args>
  void log_debug(It it_begin, It it_end, const char* fmt, Args&&... args)
  {
    log_helper(it_begin, it_end, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_info(It it_begin, It it_end, const char* fmt, Args&&... args)
  {
    log_helper(it_begin, it_end, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_warning(It it_begin, It it_end, const char* fmt, Args&&... args)
  {
    log_helper(it_begin, it_end, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args>
  void log_error(It it_begin, It it_end, const char* fmt, Args&&... args)
  {
    log_helper(it_begin, it_end, logger.error, fmt, std::forward<Args>(args)...);
  }

  const du_ue_index_t du_index;
  const lcid_t        lcid;

private:
  srslog::basic_logger& logger;

  template <typename... Args>
  void log_helper(srslog::log_channel& channel, const char* fmt, Args&&... args)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel("UE={}, LCID={}: {}", du_index, lcid, fmt::to_string(buffer));
  }

  template <typename It, typename... Args>
  void log_helper(It it_begin, It it_end, srslog::log_channel& channel, const char* fmt, Args&&... args)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel(it_begin, it_end, "UE={}, LCID={}: {}", du_index, lcid, fmt::to_string(buffer));
  }
};

} // namespace srsgnb
#endif

