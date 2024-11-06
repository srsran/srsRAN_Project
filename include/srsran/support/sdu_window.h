/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/circular_map.h"
#include "srsran/support/srsran_assert.h"
#include <cstddef>
#include <cstdint>

namespace srsran {

/// \brief This class provides a container for the Tx/Rx windows holding SDU info objects that are indexed by Sequence
/// Numbers (SN).
///
/// \tparam T               Storage Type.
/// \tparam PREFIXED_LOGGER An implementation of a prefixed_logger<type> for logging.
template <typename T, typename PREFIXED_LOGGER>
class sdu_window
{
public:
  sdu_window(PREFIXED_LOGGER& logger_, size_t size) : logger(logger_), window(size) {}

  T& add_sn(size_t sn)
  {
    if (has_sn(sn)) {
      logger.log_error("sn={} already present in window, overwriting.", sn);
      srsran_assertion_failure("sn={} already present in window.", sn);
    } else {
      logger.log_debug("Adding sn={} to window.", sn);
    }

    window.overwrite(sn, T());

    return window[sn];
  }

  void remove_sn(size_t sn)
  {
    if (not has_sn(sn)) {
      logger.log_error("Cannot remove sn={} because not contained in the window.", sn);
      srsran_assertion_failure("Cannot remove sn={} because not contained in the window.", sn);
      return;
    }

    logger.log_debug("Removing sn={} from window", sn);
    window.erase(sn);
  }

  T&       operator[](size_t sn) { return window[sn]; }
  const T& operator[](size_t sn) const { return window[sn]; }

  size_t size() const { return window.size(); }

  bool full() const { return window.full(); }

  bool empty() const { return window.empty(); }

  void clear() { window.clear(); }

  bool has_sn(uint32_t sn) const { return window.contains(sn); }

private:
  PREFIXED_LOGGER&                logger;
  circular_map<uint32_t, T, true> window;
};

} // namespace srsran
