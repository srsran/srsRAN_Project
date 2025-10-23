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

#include "srsran/ran/rnti.h"
#include <optional>

namespace srsran {
namespace srs_du {

/// Configuration that enable DU test mode operation.
struct du_test_mode_config {
  struct test_mode_ue_config {
    rnti_t   rnti    = to_rnti(0x4444);
    uint16_t nof_ues = 1;
    /// Number of slots between consecutive test mode UE creations for a given cell.
    unsigned                ue_creation_stagger_slots = 10;
    std::optional<unsigned> auto_ack_indication_delay;
    bool                    pdsch_active;
    bool                    pusch_active;
    unsigned                cqi;
    unsigned                ri;
    unsigned                pmi;
    unsigned                i_1_1;
    std::optional<unsigned> i_1_3;
    unsigned                i_2;
  };

  /// Creation of a phantom UE for test mode operation.
  std::optional<test_mode_ue_config> test_ue;
};

} // namespace srs_du
} // namespace srsran
