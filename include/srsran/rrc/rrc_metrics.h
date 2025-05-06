/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  unsigned get_count(establishment_cause_t cause) { return counters_by_cause[static_cast<unsigned>(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  establishment_cause_t get_cause(unsigned index) const { return establishment_cause_t(index); }

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
