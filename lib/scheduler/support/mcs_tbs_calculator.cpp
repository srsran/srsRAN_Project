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

#include "mcs_tbs_calculator.h"
#include "dmrs_helpers.h"
#include "sch_pdu_builder.h"
#include "tbs_calculator.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/pdsch/dlsch_info.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/uci/uci_mapping.h"

using namespace srsran;

constexpr unsigned NOF_BITS_PER_BYTE = 8U;

// Helper that generates the ulsch_configuration object necessary to compute the Effective Code Rate.
static ulsch_configuration build_ulsch_info(const pusch_config_params&   pusch_cfg,
                                            const ue_cell_configuration& ue_cell_cfg,
                                            unsigned                     tbs_bytes,
                                            sch_mcs_description          mcs_info,
                                            unsigned                     nof_prbs)
{
  ulsch_configuration ulsch_info{.tbs                = static_cast<units::bits>(tbs_bytes * NOF_BITS_PER_BYTE),
                                 .mcs_descr          = mcs_info,
                                 .nof_harq_ack_bits  = static_cast<units::bits>(pusch_cfg.nof_harq_ack_bits),
                                 .nof_csi_part1_bits = static_cast<units::bits>(pusch_cfg.nof_csi_part1_bits),
                                 .nof_csi_part2_bits = static_cast<units::bits>(pusch_cfg.max_nof_csi_part2_bits),
                                 .nof_rb             = nof_prbs,
                                 .start_symbol_index = pusch_cfg.symbols.start(),
                                 .nof_symbols        = static_cast<unsigned>(pusch_cfg.symbols.length()),
                                 .dmrs_type          = pusch_cfg.dmrs.config_type,
                                 .dmrs_symbol_mask   = pusch_cfg.dmrs.dmrs_symb_pos,
                                 .nof_cdm_groups_without_data =
                                     static_cast<unsigned>(pusch_cfg.dmrs.num_dmrs_cdm_grps_no_data),
                                 .nof_layers = pusch_cfg.nof_layers};

  ulsch_info.alpha_scaling = alpha_scaling_to_float(
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value().scaling);

  // HARQ-ACK beta offset. Which of the beta_offset to be used among the indices 1, 2, 3 is determined as per TS38.213,
  // Section 9.3 and TS38.331, \c BetaOffsets.
  // If no HARQ-ACK reporting, this value won't be used.
  if (pusch_cfg.nof_harq_ack_bits == 0) {
    ulsch_info.beta_offset_harq_ack = 0;
  }
  // Use \c betaOffsetACK-Index1 for up to 2 bits HARQ-ACK reporting.
  else if (pusch_cfg.nof_harq_ack_bits < 3) {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_1.value());
  }
  // Use \c betaOffsetACK-Index1 for up to 11 bits HARQ-ACK reporting.
  else if (pusch_cfg.nof_harq_ack_bits < 12) {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_2.value());
  }
  // Use \c betaOffsetACK-Index1 for more than 11 bits HARQ-ACK reporting.
  else {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_3.value());
  }

  // CSI Part 1 beta offset. Which of the beta_offset to be used among the indices 1, 2 is determined as per TS38.213,
  // Section 9.3 and TS38.331, \c BetaOffsets.
  // If no CSI Part 1 reporting, this value won't be used.
  if (pusch_cfg.nof_csi_part1_bits == 0) {
    ulsch_info.beta_offset_csi_part1 = 0;
  }
  // Use \c betaOffsetCSI-Part1-Index1 for up to 11 bits CSI Part 1 reporting.
  else if (pusch_cfg.nof_csi_part1_bits < 12) {
    ulsch_info.beta_offset_csi_part1 =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_1.value());
  }
  // Use \c betaOffsetCSI-Part1-Index2 for more than 11 bits CSI Part 1 reporting.
  else {
    ulsch_info.beta_offset_csi_part1 =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_2.value());
  }

  // CSI Part 2 beta offset. The beta_offset to be used among the indices 1, 2 is determined as per TS38.213, Section
  // 9.3 and TS38.331, \c BetaOffsets.
  // If no CSI Part 2 reporting, this value won't be used.
  if (pusch_cfg.max_nof_csi_part2_bits == 0) {
    ulsch_info.beta_offset_csi_part2 = 0;
  }
  // Use \c betaOffsetCSI-Part2-Index1 for up to 11 bits CSI Part 2 reporting.
  else if (pusch_cfg.max_nof_csi_part2_bits < 12) {
    ulsch_info.beta_offset_csi_part2 =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p2_idx_1.value());
  }
  // Use \c betaOffsetCSI-Part2-Index2 for more than 11 bits CSI Part 2 reporting.
  else {
    ulsch_info.beta_offset_csi_part2 =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p2_idx_2.value());
  }

  return ulsch_info;
}

// Helper that updates the ULSCH info with TBS and MCS.
static void update_ulsch_info(ulsch_configuration& ulsch_cfg, unsigned tbs_bytes, sch_mcs_description mcs_info)
{
  ulsch_cfg.tbs       = static_cast<units::bits>(tbs_bytes * NOF_BITS_PER_BYTE);
  ulsch_cfg.mcs_descr = mcs_info;
}

optional<sch_mcs_tbs> srsran::compute_dl_mcs_tbs(const pdsch_config_params&   pdsch_params,
                                                 const ue_cell_configuration& ue_cell_cfg,
                                                 sch_mcs_index                max_mcs,
                                                 unsigned                     nof_prbs)
{
  // The maximum supported code rate is 0.95, as per TS38.214, Section 5.1.3. The maximum code rate is defined for DL,
  // but we consider the same value for UL.
  static const double max_supported_code_rate = 0.95;
  const unsigned      dmrs_prbs               = calculate_nof_dmrs_per_rb(pdsch_params.dmrs);
  sch_mcs_description mcs_info                = pdsch_mcs_get_config(pdsch_params.mcs_table, max_mcs);
  unsigned            nof_symbols             = pdsch_params.symbols.length();

  unsigned tbs_bits =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                            .nof_dmrs_prb     = dmrs_prbs,
                                                            .nof_oh_prb       = pdsch_params.nof_oh_prb,
                                                            .mcs_descr        = mcs_info,
                                                            .nof_layers       = pdsch_params.nof_layers,
                                                            .tb_scaling_field = pdsch_params.tb_scaling_field,
                                                            .n_prb            = nof_prbs});

  // > Compute the effective code rate.
  dlsch_configuration dlsch_info{.tbs                = static_cast<units::bits>(tbs_bits),
                                 .mcs_descr          = mcs_info,
                                 .nof_rb             = nof_prbs,
                                 .start_symbol_index = pdsch_params.symbols.start(),
                                 .nof_symbols        = static_cast<unsigned>(pdsch_params.symbols.length()),
                                 .dmrs_type          = pdsch_params.dmrs.config_type,
                                 .dmrs_symbol_mask   = pdsch_params.dmrs.dmrs_symb_pos,
                                 .nof_cdm_groups_without_data =
                                     static_cast<unsigned>(pdsch_params.dmrs.num_dmrs_cdm_grps_no_data),
                                 .nof_layers = pdsch_params.nof_layers};

  float effective_code_rate = get_dlsch_information(dlsch_info).get_effective_code_rate();

  // > Decrease the MCS and recompute TBS until the effective code rate is not above the 0.95 threshold.
  sch_mcs_index mcs = max_mcs;
  while (effective_code_rate > max_supported_code_rate and mcs > 0) {
    --mcs;
    mcs_info = pdsch_mcs_get_config(pdsch_params.mcs_table, mcs);
    tbs_bits = tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                                     .nof_dmrs_prb     = dmrs_prbs,
                                                                     .nof_oh_prb       = pdsch_params.nof_oh_prb,
                                                                     .mcs_descr        = mcs_info,
                                                                     .nof_layers       = pdsch_params.nof_layers,
                                                                     .tb_scaling_field = pdsch_params.tb_scaling_field,
                                                                     .n_prb            = nof_prbs});

    dlsch_info.tbs       = static_cast<units::bits>(tbs_bits);
    dlsch_info.mcs_descr = mcs_info;
    effective_code_rate  = get_dlsch_information(dlsch_info).get_effective_code_rate();
  }

  // If no MCS such that effective code rate <= 0.95, return an empty optional object.
  if (effective_code_rate > max_supported_code_rate and mcs == 0) {
    return nullopt;
  }

  const unsigned tbs_bytes = tbs_bits / NOF_BITS_PER_BYTE;
  return optional<sch_mcs_tbs>{sch_mcs_tbs{.mcs = mcs, .tbs = tbs_bytes}};
}

optional<sch_mcs_tbs> srsran::compute_ul_mcs_tbs(const pusch_config_params&   pusch_cfg,
                                                 const ue_cell_configuration& ue_cell_cfg,
                                                 sch_mcs_index                max_mcs,
                                                 unsigned                     nof_prbs)
{
  // The maximum supported code rate is 0.95, as per TS38.214, Section 5.1.3. The maximum code rate is defined for DL,
  // but we consider the same value for UL.
  static const double max_supported_code_rate = 0.95;
  const unsigned      dmrs_prbs               = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  sch_mcs_description mcs_info    = pusch_mcs_get_config(pusch_cfg.mcs_table, max_mcs, pusch_cfg.tp_pi2bpsk_present);
  unsigned            nof_symbols = pusch_cfg.symbols.length();

  unsigned tbs_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                            .nof_dmrs_prb     = dmrs_prbs,
                                                            .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                            .mcs_descr        = mcs_info,
                                                            .nof_layers       = pusch_cfg.nof_layers,
                                                            .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                            .n_prb            = nof_prbs}) /
      NOF_BITS_PER_BYTE;

  // > Compute the effective code rate.
  ulsch_configuration ulsch_cfg           = build_ulsch_info(pusch_cfg, ue_cell_cfg, tbs_bytes, mcs_info, nof_prbs);
  float               effective_code_rate = get_ulsch_information(ulsch_cfg).get_effective_code_rate();

  // > Decrease the MCS and recompute TBS until the effective code rate is not above the 0.95 threshold.
  sch_mcs_index mcs = max_mcs;
  while (effective_code_rate > max_supported_code_rate and mcs > 0) {
    --mcs;
    mcs_info  = pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, false);
    tbs_bytes = tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                                      .nof_dmrs_prb     = dmrs_prbs,
                                                                      .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                                      .mcs_descr        = mcs_info,
                                                                      .nof_layers       = pusch_cfg.nof_layers,
                                                                      .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                                      .n_prb            = nof_prbs}) /
                NOF_BITS_PER_BYTE;

    update_ulsch_info(ulsch_cfg, tbs_bytes, mcs_info);
    effective_code_rate = get_ulsch_information(ulsch_cfg).get_effective_code_rate();
  }

  // If no MCS such that effective code rate <= 0.95, return an empty optional object.
  if (effective_code_rate > max_supported_code_rate and mcs == 0) {
    return nullopt;
  }

  // If no MCS such that nof bits for PUSCH > 0, return an empty optional object.
  if (effective_code_rate == 0) {
    return nullopt;
  }

  return optional<sch_mcs_tbs>{sch_mcs_tbs{.mcs = mcs, .tbs = tbs_bytes}};
}
