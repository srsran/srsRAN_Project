/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/srslog/srslog.h"
#include <linux/sctp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace srsran {

/// Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the maximum re-transmission timeout (rto_max).
inline bool sctp_set_rto_opts(int                   fd,
                              optional<int>         rto_initial,
                              optional<int>         rto_min,
                              optional<int>         rto_max,
                              srslog::basic_logger& logger)
{
  if (not rto_initial.has_value() && not rto_min.has_value() && not rto_max.has_value()) {
    // no need to set RTO
    return true;
  }

  // Set RTO_MAX to quickly detect broken links.
  sctp_rtoinfo rto_opts  = {};
  socklen_t    rto_sz    = sizeof(sctp_rtoinfo);
  rto_opts.srto_assoc_id = 0;
  if (getsockopt(fd, SOL_SCTP, SCTP_RTOINFO, &rto_opts, &rto_sz) < 0) {
    logger.error("Error getting RTO_INFO sockopts.", strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }

  if (rto_initial.has_value()) {
    rto_opts.srto_initial = rto_initial.value();
  }
  if (rto_min.has_value()) {
    rto_opts.srto_min = rto_min.value();
  }
  if (rto_max.has_value()) {
    rto_opts.srto_max = rto_max.value();
  }

  logger.debug(
      "Setting RTO_INFO options on SCTP socket. Association {}, Initial RTO {}, Minimum RTO {}, Maximum RTO {}",
      rto_opts.srto_assoc_id,
      rto_opts.srto_initial,
      rto_opts.srto_min,
      rto_opts.srto_max);

  if (::setsockopt(fd, SOL_SCTP, SCTP_RTOINFO, &rto_opts, rto_sz) < 0) {
    logger.error("Error setting RTO_INFO sockopts. errno={}", strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }
  return true;
}

/// Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the SCTP_INITMSG parameters (to control the timeout of the connect() syscall)
inline bool sctp_set_init_msg_opts(int                   fd,
                                   optional<int>         init_max_attempts,
                                   optional<int>         max_init_timeo,
                                   srslog::basic_logger& logger)
{
  if (not init_max_attempts.has_value() && not max_init_timeo.has_value()) {
    // No value set for init max attempts or max init_timeo,
    // no need to call set_sockopts()
    return true;
  }

  // Set SCTP INITMSG options to reduce blocking timeout of connect()
  sctp_initmsg init_opts = {};
  socklen_t    init_sz   = sizeof(sctp_initmsg);
  if (getsockopt(fd, SOL_SCTP, SCTP_INITMSG, &init_opts, &init_sz) < 0) {
    logger.error("Error getting sockopts. errno={}", strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }

  if (init_max_attempts.has_value()) {
    init_opts.sinit_max_attempts = init_max_attempts.value();
  }
  if (max_init_timeo.has_value()) {
    init_opts.sinit_max_init_timeo = max_init_timeo.value();
  }

  logger.debug("Setting SCTP_INITMSG options on SCTP socket. Max attempts {}, Max init attempts timeout {}",
               init_opts.sinit_max_attempts,
               init_opts.sinit_max_init_timeo);
  if (::setsockopt(fd, SOL_SCTP, SCTP_INITMSG, &init_opts, init_sz) < 0) {
    logger.error("Error setting SCTP_INITMSG sockopts. errno={}\n", strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }
  return true;
}
} // namespace srsran
