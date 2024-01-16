/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/circular_map.h"
#include "srsran/support/sdu_window.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief This class provides a container for the Tx/Rx windows holding SDU info objects that are indexed by
/// Sequence Numbers (SN)
/// @tparam T storage type
/// @tparam WINDOW_SIZE size of the window
/// @tparam PREFIXED_LOGGER an implementation of a prefixed_logger<type> for logging
template <class T, std::size_t WINDOW_SIZE, class PREFIXED_LOGGER>
class sdu_window_impl final : public sdu_window<T>
{
public:
  sdu_window_impl(PREFIXED_LOGGER& logger_) : logger(logger_) {}
  ~sdu_window_impl() = default;

  T& add_sn(size_t sn) override
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
  void remove_sn(size_t sn) override
  {
    if (not has_sn(sn)) {
      logger.log_error("Cannot remove sn={} because not contained in the window.", sn);
      srsran_assertion_failure("Cannot remove sn={} because not contained in the window.", sn);
      return;
    }
    logger.log_debug("Removing sn={} from window", sn);
    window.erase(sn);
  }
  T&     operator[](size_t sn) override { return window[sn]; }
  size_t size() const override { return window.size(); }
  bool   full() const override { return window.full(); }
  bool   empty() const override { return window.empty(); }
  void   clear() override { window.clear(); }

  bool has_sn(uint32_t sn) const override { return window.contains(sn); }

private:
  PREFIXED_LOGGER&                               logger;
  srsran::circular_map<uint32_t, T, WINDOW_SIZE> window;
};

} // namespace srsran
