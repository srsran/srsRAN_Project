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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/scheduler/result/pucch_format.h"

namespace srsran {

/// Information about the number of UCI bits of each type.
struct uci_bits {
  sr_nof_bits sr_bits;
  unsigned    harq_ack_nof_bits;
  unsigned    csi_part1_bits;
};

/// Info about PUCCH used resource.
struct pucch_info {
  /// This information only is used by the scheduler and not passed to the PHY.
  struct context {
    /// Identifier of the PUCCH PDU within the list of PUCCH PDUs for a given slot. The ID is only meaningful for a
    /// given UE; i.e., different UEs can reuse the same ID, but a UE cannot reuse the same ID for different PDUs.
    unsigned id = MAX_PUCCH_PDUS_PER_SLOT;
    /// Determines whether the PUCCH PDU uses common resources.
    bool is_common = false;
  };

  rnti_t                   crnti;
  const bwp_configuration* bwp_cfg;
  /// PRBs and symbols for this PUCCH resource.
  pucch_resources                                                                              resources;
  std::variant<pucch_format_0, pucch_format_1, pucch_format_2, pucch_format_3, pucch_format_4> format_params;
  uci_bits                                                                                     bits;
  /// In case the PUCCH will contain CSI bits, this struct contains information how those bits are to be decoded.
  std::optional<csi_report_configuration> csi_rep_cfg;

  context pdu_context;

  /// Returns the format of the PUCCH.
  constexpr pucch_format format() const
  {
    return std::visit(
        [](const auto& params) -> pucch_format {
          using T = std::decay_t<decltype(params)>;
          if constexpr (std::is_same_v<T, pucch_format_0>) {
            return pucch_format::FORMAT_0;
          } else if constexpr (std::is_same_v<T, pucch_format_1>) {
            return pucch_format::FORMAT_1;
          } else if constexpr (std::is_same_v<T, pucch_format_2>) {
            return pucch_format::FORMAT_2;
          } else if constexpr (std::is_same_v<T, pucch_format_3>) {
            return pucch_format::FORMAT_3;
          } else if constexpr (std::is_same_v<T, pucch_format_4>) {
            return pucch_format::FORMAT_4;
          }
          return pucch_format::NOF_FORMATS;
        },
        format_params);
  }

  /// Sets the format parameters to the appropiate type for the PUCCH format.
  void set_format(pucch_format format)
  {
    switch (format) {
      case pucch_format::FORMAT_0:
        format_params.emplace<pucch_format_0>();
        break;
      case pucch_format::FORMAT_1:
        format_params.emplace<pucch_format_1>();
        break;
      case pucch_format::FORMAT_2:
        format_params.emplace<pucch_format_2>();
        break;
      case pucch_format::FORMAT_3:
        format_params.emplace<pucch_format_3>();
        break;
      case pucch_format::FORMAT_4:
        format_params.emplace<pucch_format_4>();
        break;
      default:
        srsran_assertion_failure("Invalid PUCCH format");
    }
  }
};

} // namespace srsran
