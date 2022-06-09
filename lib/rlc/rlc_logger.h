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

#include "../ran/gnb_format.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

/// Class used to store common logging parameters for all RLC bearer types.
/// It provides logging helpers, so that the UE index and LCID are always logged.
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_logger
{
public:
  rlc_logger(du_ue_index_t du_index, lcid_t lcid) :
    du_index(du_index), lcid(lcid), logger(srslog::fetch_basic_logger("RLC"))
  {}

protected:
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

  const du_ue_index_t du_index;
  const lcid_t        lcid;

private:
  srslog::basic_logger& logger;

  template <typename... Args>
  void log_helper(srslog::log_channel& channel, const char* fmt, Args&&... args)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    channel("UE={}, LCID={}: {}", du_index, lcid, to_c_str(buffer));
  }
};

} // namespace srsgnb
#endif

