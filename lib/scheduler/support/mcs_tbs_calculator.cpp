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

#include "mcs_tbs_calculator.h"
#include "dmrs_helpers.h"
#include "sch_pdu_builder.h"
#include "srsran/ran/pdsch/dlsch_info.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/ran/uci/uci_info.h"
#include "srsran/ran/uci/uci_mapping.h"

using namespace srsran;

static constexpr unsigned NOF_BITS_PER_BYTE = 8U;

// Helper that generates the ulsch_configuration object necessary to compute the Effective Code Rate.
static ulsch_configuration build_ulsch_info(const pusch_config_params& pusch_cfg,
                                            const bwp_config&          active_bwp_cfg,
                                            unsigned                   tbs_bytes,
                                            sch_mcs_description        mcs_info,
                                            unsigned                   nof_prbs,
                                            bool                       contains_dc)
{
  ulsch_configuration ulsch_info{.tbs                = static_cast<units::bits>(tbs_bytes * NOF_BITS_PER_BYTE),
                                 .mcs_descr          = mcs_info,
                                 .nof_harq_ack_bits  = static_cast<units::bits>(pusch_cfg.nof_harq_ack_bits),
                                 .nof_csi_part1_bits = static_cast<units::bits>(pusch_cfg.nof_csi_part1_bits),
                                 .nof_csi_part2_bits = static_cast<units::bits>(pusch_cfg.max_nof_csi_part2_bits),
                                 .nof_rb             = nof_prbs,
                                 .start_symbol_index = pusch_cfg.symbols.start(),
                                 .nof_symbols        = pusch_cfg.symbols.length(),
                                 .dmrs_type          = pusch_cfg.dmrs.config_type,
                                 .dmrs_symbol_mask   = pusch_cfg.dmrs.dmrs_symb_pos,
                                 .nof_cdm_groups_without_data =
                                     static_cast<unsigned>(pusch_cfg.dmrs.num_dmrs_cdm_grps_no_data),
                                 .nof_layers  = pusch_cfg.nof_layers,
                                 .contains_dc = contains_dc};

  const uci_on_pusch* uci_cfg =
      active_bwp_cfg.ul_ded.has_value() and active_bwp_cfg.ul_ded->pusch_cfg->uci_cfg.has_value()
          ? &active_bwp_cfg.ul_ded->pusch_cfg->uci_cfg.value()
          : nullptr;
  srsran_assert(uci_cfg != nullptr or (pusch_cfg.nof_harq_ack_bits == 0 and pusch_cfg.nof_csi_part1_bits == 0 and
                                       pusch_cfg.max_nof_csi_part2_bits == 0),
                "No UCI bits should be present for fallback UEs.");

  // For fallback UEs, no dedicated PUSCH config exists; in that case, we set this to a default value of
  // alpha_scaling_opt::f1, as this is not used.
  ulsch_info.alpha_scaling = alpha_scaling_to_float(uci_cfg != nullptr ? uci_cfg->scaling : alpha_scaling_opt::f1);

  // HARQ-ACK beta offset. Which of the beta_offset to be used among the indices 1, 2, 3 is determined as per TS38.213,
  // Section 9.3 and TS38.331, \c BetaOffsets.
  // If no HARQ-ACK reporting, this value won't be used.
  if (pusch_cfg.nof_harq_ack_bits == 0) {
    ulsch_info.beta_offset_harq_ack = 0;
  } else {
    const auto& beta_offsets = std::get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg->beta_offsets_cfg.value());

    // Use \c betaOffsetACK-Index1 for up to 2 bits HARQ-ACK reporting.
    if (pusch_cfg.nof_harq_ack_bits < 3) {
      ulsch_info.beta_offset_harq_ack = beta_harq_ack_to_float(beta_offsets.beta_offset_ack_idx_1.value());
    }
    // Use \c betaOffsetACK-Index1 for up to 11 bits HARQ-ACK reporting.
    else if (pusch_cfg.nof_harq_ack_bits < 12) {
      ulsch_info.beta_offset_harq_ack = beta_harq_ack_to_float(beta_offsets.beta_offset_ack_idx_2.value());
    }
    // Use \c betaOffsetACK-Index1 for more than 11 bits HARQ-ACK reporting.
    else {
      ulsch_info.beta_offset_harq_ack = beta_harq_ack_to_float(beta_offsets.beta_offset_ack_idx_3.value());
    }
  }

  // CSI Part 1 beta offset. Which of the beta_offset to be used among the indices 1, 2 is determined as per TS38.213,
  // Section 9.3 and TS38.331, \c BetaOffsets.
  // If no CSI Part 1 reporting, this value won't be used.
  if (pusch_cfg.nof_csi_part1_bits == 0) {
    ulsch_info.beta_offset_csi_part1 = 0;
  } else {
    const auto& beta_offsets = std::get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg->beta_offsets_cfg.value());

    // Use \c betaOffsetCSI-Part1-Index1 for up to 11 bits CSI Part 1 reporting.
    if (pusch_cfg.nof_csi_part1_bits < 12) {
      ulsch_info.beta_offset_csi_part1 = beta_csi_to_float(beta_offsets.beta_offset_csi_p1_idx_1.value());
    }
    // Use \c betaOffsetCSI-Part1-Index2 for more than 11 bits CSI Part 1 reporting.
    else {
      ulsch_info.beta_offset_csi_part1 = beta_csi_to_float(beta_offsets.beta_offset_csi_p1_idx_2.value());
    }
  }

  // CSI Part 2 beta offset. The beta_offset to be used among the indices 1, 2 is determined as per TS38.213, Section
  // 9.3 and TS38.331, \c BetaOffsets.
  // If no CSI Part 2 reporting, this value won't be used.
  if (pusch_cfg.max_nof_csi_part2_bits == 0) {
    ulsch_info.beta_offset_csi_part2 = 0;
  } else {
    const auto& beta_offsets = std::get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg->beta_offsets_cfg.value());

    // Use \c betaOffsetCSI-Part2-Index1 for up to 11 bits CSI Part 2 reporting.
    if (pusch_cfg.max_nof_csi_part2_bits < 12) {
      ulsch_info.beta_offset_csi_part2 = beta_csi_to_float(beta_offsets.beta_offset_csi_p2_idx_1.value());
    }
    // Use \c betaOffsetCSI-Part2-Index2 for more than 11 bits CSI Part 2 reporting.
    else {
      ulsch_info.beta_offset_csi_part2 = beta_csi_to_float(beta_offsets.beta_offset_csi_p2_idx_2.value());
    }
  }

  return ulsch_info;
}

// Helper that updates the ULSCH info with TBS and MCS.
static void update_ulsch_info(ulsch_configuration& ulsch_cfg, unsigned tbs_bytes, sch_mcs_description mcs_info)
{
  ulsch_cfg.tbs       = static_cast<units::bits>(tbs_bytes * NOF_BITS_PER_BYTE);
  ulsch_cfg.mcs_descr = mcs_info;
}

// Helper that determines the minimum of rate-matched bits for a certain UCI message multiplexed in PUSCH.
static unsigned calculate_min_uci_bits(unsigned          nof_payload_bits,
                                       bool              contains_dc,
                                       unsigned          nof_uci_re,
                                       unsigned          nof_prbs,
                                       modulation_scheme modulation,
                                       unsigned          nof_layers)
{
  // Number of rate-matched bits required for the physical layer to detect or decode correctly a UCI field.
  unsigned min_encoded_bits = calculate_uci_min_encoded_bits(nof_payload_bits);

  // Account the number of bits that could be potentially used by the DC.
  if (contains_dc) {
    unsigned nof_ofdm_symbols = divide_ceil(nof_uci_re, nof_prbs * NOF_SUBCARRIERS_PER_RB);
    min_encoded_bits += nof_ofdm_symbols * get_bits_per_symbol(modulation) * nof_layers;
  }

  return min_encoded_bits;
}

// Helper that determines if the effective code rate for data and UCI is valid.
static compute_ul_mcs_tbs_error is_pusch_effective_rate_valid(const pusch_config_params& config,
                                                              const ulsch_information&   ulsch_info,
                                                              const sch_mcs_description& mcs_info,
                                                              unsigned                   nof_prbs)
{
  using namespace units::literals;

  // The maximum supported code rate is 0.95, as per TS 38.214, Section 5.1.3. The maximum code rate is defined for DL,
  // but we consider the same value for UL.
  static const double max_supported_code_rate = 0.95;

  // Make sure the rate-matched bits for two HARQ-ACK feedback bits plus the DC overhead does not exceed the number of
  // rate-matched bits for UL-SCH.
  units::bits nof_harq_dc_overhead = ulsch_info.nof_dc_overlap_bits;
  if (ulsch_info.nof_harq_ack_rvd != 0_bits) {
    nof_harq_dc_overhead += ulsch_info.nof_harq_ack_bits;
  }
  if (nof_harq_dc_overhead > ulsch_info.nof_ul_sch_bits) {
    return compute_ul_mcs_tbs_error::two_bit_harq_ack_and_dc_overhead;
  }

  // Check the effective code rate for data does not exceed the maximum.
  if (ulsch_info.get_effective_code_rate() > max_supported_code_rate) {
    return compute_ul_mcs_tbs_error::effective_code_rate_exceeds_maximum;
  }

  // Check the effective code rate for data is not zero.
  if ((ulsch_info.nof_ul_sch_bits == 0_bits) || (ulsch_info.get_effective_code_rate() == 0)) {
    return compute_ul_mcs_tbs_error::invalid_ulsch_information;
  }

  // Check the number of HARQ-ACK feedback rate-matched bits reaches the minimum.
  if (config.nof_harq_ack_bits != 0) {
    unsigned min_nof_encoded_bits = calculate_min_uci_bits(config.nof_harq_ack_bits,
                                                           ulsch_info.nof_dc_overlap_bits.value() > 0,
                                                           ulsch_info.nof_harq_ack_re,
                                                           nof_prbs,
                                                           mcs_info.modulation,
                                                           config.nof_layers);
    if (min_nof_encoded_bits > ulsch_info.nof_harq_ack_bits.value()) {
      return compute_ul_mcs_tbs_error::insufficient_nof_rm_harq_ack;
    }
  }

  // Check the number of CSI Part 1 report number of rate-matched bits reaches the minimum.
  if (config.nof_csi_part1_bits != 0) {
    unsigned min_nof_encoded_bits = calculate_min_uci_bits(config.nof_csi_part1_bits,
                                                           ulsch_info.nof_dc_overlap_bits.value() > 0,
                                                           ulsch_info.nof_csi_part1_re,
                                                           nof_prbs,
                                                           mcs_info.modulation,
                                                           config.nof_layers);
    if (min_nof_encoded_bits > ulsch_info.nof_csi_part1_bits.value()) {
      return compute_ul_mcs_tbs_error::insufficient_nof_rm_csi1;
    }
  }

  return compute_ul_mcs_tbs_error::none;
}

std::optional<sch_mcs_tbs> srsran::compute_dl_mcs_tbs(const pdsch_config_params& pdsch_params,
                                                      sch_mcs_index              max_mcs,
                                                      unsigned                   nof_prbs,
                                                      bool                       contains_dc)
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
                                 .nof_symbols        = pdsch_params.symbols.length(),
                                 .dmrs_type          = pdsch_params.dmrs.config_type,
                                 .dmrs_symbol_mask   = pdsch_params.dmrs.dmrs_symb_pos,
                                 .nof_cdm_groups_without_data =
                                     static_cast<unsigned>(pdsch_params.dmrs.num_dmrs_cdm_grps_no_data),
                                 .nof_layers  = pdsch_params.nof_layers,
                                 .contains_dc = contains_dc};

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

    dlsch_info.tbs         = static_cast<units::bits>(tbs_bits);
    dlsch_info.mcs_descr   = mcs_info;
    dlsch_info.contains_dc = contains_dc;
    effective_code_rate    = get_dlsch_information(dlsch_info).get_effective_code_rate();
  }

  // If no MCS such that effective code rate <= 0.95, return an empty optional object.
  if (effective_code_rate > max_supported_code_rate and mcs == 0) {
    return std::nullopt;
  }

  const unsigned tbs_bytes = tbs_bits / NOF_BITS_PER_BYTE;
  return std::optional<sch_mcs_tbs>{sch_mcs_tbs{.mcs = mcs, .tbs = tbs_bytes}};
}

expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> srsran::compute_ul_mcs_tbs(const pusch_config_params& pusch_cfg,
                                                                           const bwp_config&          active_bwp_cfg,
                                                                           sch_mcs_index              max_mcs,
                                                                           unsigned                   nof_prbs,
                                                                           bool                       contains_dc)
{
  const unsigned      dmrs_prbs = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  sch_mcs_description mcs_info  = pusch_mcs_get_config(
      pusch_cfg.mcs_table, max_mcs, pusch_cfg.use_transform_precoder, pusch_cfg.tp_pi2bpsk_present);
  const unsigned nof_symbols = pusch_cfg.symbols.length();

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
  ulsch_configuration ulsch_cfg =
      build_ulsch_info(pusch_cfg, active_bwp_cfg, tbs_bytes, mcs_info, nof_prbs, contains_dc);
  ulsch_information info = get_ulsch_information(ulsch_cfg);

  // > Decrease the MCS and recompute TBS until the effective code rate is not above the 0.95 threshold.
  sch_mcs_index            mcs   = max_mcs;
  compute_ul_mcs_tbs_error error = compute_ul_mcs_tbs_error::undefined;
  while (((error = ::is_pusch_effective_rate_valid(pusch_cfg, info, mcs_info, nof_prbs)) !=
          compute_ul_mcs_tbs_error::none) and
         mcs > 0) {
    --mcs;
    mcs_info  = pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, false);
    tbs_bytes = tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                                      .nof_dmrs_prb     = dmrs_prbs,
                                                                      .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                                      .mcs_descr        = mcs_info,
                                                                      .nof_layers       = pusch_cfg.nof_layers,
                                                                      .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                                      .n_prb            = nof_prbs}) /
                NOF_BITS_PER_BYTE;

    update_ulsch_info(ulsch_cfg, tbs_bytes, mcs_info);
    info = get_ulsch_information(ulsch_cfg);
  }

  // If the function did not converge with a valid effective code rate, return the latest error.
  if (error != compute_ul_mcs_tbs_error::none) {
    return make_unexpected(error);
  }

  return sch_mcs_tbs{.mcs = mcs, .tbs = tbs_bytes};
}

std::optional<unsigned> srsran::compute_ul_tbs(const pusch_config_params& pusch_cfg,
                                               const bwp_config&          active_bwp_cfg,
                                               sch_mcs_index              mcs,
                                               unsigned                   nof_prbs,
                                               bool                       contains_dc)
{
  const unsigned            dmrs_prbs = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  const sch_mcs_description mcs_info =
      pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, pusch_cfg.tp_pi2bpsk_present);
  const unsigned nof_symbols = pusch_cfg.symbols.length();

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
  const ulsch_configuration ulsch_cfg =
      build_ulsch_info(pusch_cfg, active_bwp_cfg, tbs_bytes, mcs_info, nof_prbs, contains_dc);
  const ulsch_information info = get_ulsch_information(ulsch_cfg);

  if (::is_pusch_effective_rate_valid(pusch_cfg, info, mcs_info, nof_prbs) == compute_ul_mcs_tbs_error::none) {
    return tbs_bytes;
  }
  return std::nullopt;
}

bool srsran::is_pusch_effective_rate_valid(const pusch_config_params& pusch_cfg,
                                           const bwp_config&          active_bwp_cfg,
                                           sch_mcs_index              mcs,
                                           unsigned                   nof_prbs,
                                           bool                       contains_dc)
{
  const unsigned            dmrs_prbs = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  const sch_mcs_description mcs_info =
      pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, pusch_cfg.tp_pi2bpsk_present);
  unsigned tbs_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = pusch_cfg.symbols.length(),
                                                            .nof_dmrs_prb     = dmrs_prbs,
                                                            .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                            .mcs_descr        = mcs_info,
                                                            .nof_layers       = pusch_cfg.nof_layers,
                                                            .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                            .n_prb            = nof_prbs}) /
      NOF_BITS_PER_BYTE;

  const ulsch_configuration ulsch_cfg =
      build_ulsch_info(pusch_cfg, active_bwp_cfg, tbs_bytes, mcs_info, nof_prbs, contains_dc);
  const ulsch_information info = get_ulsch_information(ulsch_cfg);
  return ::is_pusch_effective_rate_valid(pusch_cfg, info, mcs_info, nof_prbs) == compute_ul_mcs_tbs_error::none;
}

unsigned srsran::compute_ul_tbs_unsafe(const pusch_config_params& pusch_cfg, sch_mcs_index mcs, unsigned nof_prbs)
{
  const unsigned            dmrs_prbs = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  const sch_mcs_description mcs_info =
      pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, pusch_cfg.tp_pi2bpsk_present);
  const unsigned nof_symbols = pusch_cfg.symbols.length();

  return tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                               .nof_dmrs_prb     = dmrs_prbs,
                                                               .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                               .mcs_descr        = mcs_info,
                                                               .nof_layers       = pusch_cfg.nof_layers,
                                                               .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                               .n_prb            = nof_prbs}) /
         NOF_BITS_PER_BYTE;
}
