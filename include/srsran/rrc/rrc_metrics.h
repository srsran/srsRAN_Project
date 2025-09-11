/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/cause/common.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

struct rrc_connection_establishment_counter_with_cause {
  rrc_connection_establishment_counter_with_cause() = default;

  void increase(establishment_cause_t cause)
  {
    srsran_assert(static_cast<unsigned>(cause) < counters_by_cause.size(), "Invalid cause");
    counters_by_cause[static_cast<unsigned>(cause)]++;
  }

  unsigned size() const { return counters_by_cause.size(); }

  unsigned get_count(establishment_cause_t cause) const { return counters_by_cause[static_cast<unsigned>(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  establishment_cause_t get_cause(unsigned index) const { return establishment_cause_t(index); }

  /// Returns a const iterator to the beginning of the container.
  std::array<unsigned, 10>::const_iterator begin() const { return counters_by_cause.begin(); }
  std::array<unsigned, 10>::const_iterator cbegin() const { return counters_by_cause.cbegin(); }

  /// Returns a const iterator to the end of the container.
  std::array<unsigned, 10>::const_iterator end() const { return counters_by_cause.end(); }
  std::array<unsigned, 10>::const_iterator cend() const { return counters_by_cause.cend(); }

private:
  // The RRC setup request/complete counters indexed by the establishment cause.
  std::array<unsigned, 10> counters_by_cause = {};
};

struct rrc_du_metrics {
  /// \brief RRC connection metrics, see TS 28.552 section 5.1.1.4.
  unsigned mean_nof_rrc_connections;
  unsigned max_nof_rrc_connections;
  /// \brief RRC connection establishment metrics, see TS 28.552 section 5.1.1.15.
  rrc_connection_establishment_counter_with_cause attempted_rrc_connection_establishments;
  rrc_connection_establishment_counter_with_cause successful_rrc_connection_establishments;
  /// \brief RRC connection re-establishment metrics, see TS 28.552 section 5.1.1.17.
  unsigned attempted_rrc_connection_reestablishments;
  unsigned successful_rrc_connection_reestablishments_with_ue_context;
  unsigned successful_rrc_connection_reestablishments_without_ue_context;
};

} // namespace srsran
