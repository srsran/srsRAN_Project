/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/du/du_cell_config_validation.h"
#include "../du_manager/ran_resource_management/pucch_resource_generator.h"
#include "srsran/asn1/rrc_nr/serving_cell.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/ssb_mapping.h"
#include "srsran/scheduler/config/serving_cell_config_validator.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/config/validator_helpers.h"

using namespace srsran;

#define CHECK_TRUE(cond, ...)                                                                                          \
  if (not(cond)) {                                                                                                     \
    return {fmt::format(__VA_ARGS__)};                                                                                 \
  }

#define CHECK_COMPARE_(val, expected_val, comparison, ...)                                                             \
  CHECK_TRUE(                                                                                                          \
      (val comparison expected_val), "Invalid {} ({} " #comparison "{})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_EQ(val, expected_val, ...)                                                                               \
  CHECK_TRUE((val == expected_val), "Invalid {} ({} != {})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_NEQ(val, forbidden_val, ...)                                                                             \
  CHECK_TRUE((val != forbidden_val), "Invalid {} ({} == {})", fmt::format(__VA_ARGS__), val, forbidden_val)

/// Checks if "val" is below or equal to "max_val".
#define CHECK_EQ_OR_BELOW(val, max_val, ...)                                                                           \
  CHECK_TRUE((val <= max_val), "Invalid {} ({} > {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is below "max_val".
#define CHECK_BELOW(val, max_val, ...)                                                                                 \
  CHECK_TRUE((val < max_val), "Invalid {} ({} >= {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is above or equal to "max_val".
#define CHECK_EQ_OR_ABOVE(val, max_val, ...)                                                                           \
  CHECK_TRUE((val >= max_val), "Invalid {} ({} < {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is above "max_val".
#define CHECK_ABOVE(val, max_val, ...)                                                                                 \
  CHECK_TRUE((val > max_val), "Invalid {} ({} <= {})", fmt::format(__VA_ARGS__), val, max_val)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper typedef.
using check_outcome = error_type<std::string>;

/// Helper function to verify if a number belongs to the list of possible values of an ASN.1 enumerated type.
template <typename ASN1Enumerated, typename Number>
bool is_valid_enum_number(Number number)
{
  ASN1Enumerated e;
  return asn1::number_to_enum(e, number);
}

/// Checks whether CORESET#0 table index is valid as per TS38.213, Table 13-{1,...,10}.
static check_outcome is_coreset0_ss0_idx_valid(const du_cell_config& cell_cfg)
{
  // TODO: Check which table to use.
  // TODO: Add checks on minimum bandwidth.
  if (cell_cfg.scs_common == subcarrier_spacing::kHz15 and cell_cfg.ssb_cfg.scs == subcarrier_spacing::kHz15) {
    // As per TS38.213, Table 13-1.
    CHECK_BELOW(cell_cfg.coreset0_idx, 15, "CORESET#0 index table");
  } else if (cell_cfg.scs_common == subcarrier_spacing::kHz30 and cell_cfg.ssb_cfg.scs == subcarrier_spacing::kHz30) {
    // As per TS38.213, Table 13-4.
    CHECK_BELOW(cell_cfg.coreset0_idx, 16, "CORESET#0 index table");
  }
  // The remaining cases will trigger an assert in the SSB checks.

  // This constraint is implementation-defined and comes from the fact that our PDCCH scheduler only schedules PDCCH
  // starting from the symbol index 0.
  if (cell_cfg.scs_common == subcarrier_spacing::kHz15 and cell_cfg.ssb_cfg.scs == subcarrier_spacing::kHz15) {
    // As per TS38.213, Table 13-11.
    CHECK_EQ_OR_BELOW(cell_cfg.searchspace0_idx, 9, "SearchSpaceZero index table");
  }

  return {};
}

/// Checks whether CORESET#0 configuration matches the values specified in TS38.211-7.3.2.2.
static check_outcome is_coreset0_params_valid(const du_cell_config& cell_cfg)
{
  const coreset_configuration& cs_cfg = *cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
  CHECK_TRUE(cs_cfg.duration >= 1, "Invalid CORESET#0 slot duration ({})", cs_cfg.duration);
  // Implicit, invariant values for CORESET#0 as per TS38.211-7.3.2.2.
  CHECK_EQ(cs_cfg.id, 0, "CORESET#0 ID");
  CHECK_TRUE(cs_cfg.interleaved.has_value(), "CORESET#0 must be interleaved");
  CHECK_EQ(cs_cfg.interleaved->interleaver_sz, 2, "CORESET#0 interleaver size (R)");
  CHECK_EQ(cs_cfg.interleaved->reg_bundle_sz, 6, "CORESET#0 REG Bundle size (L)");
  CHECK_EQ(cs_cfg.interleaved->shift_index, cell_cfg.pci, "CORESET#0 shift index should be equal to PCI");
  CHECK_EQ(cs_cfg.precoder_granurality,
           coreset_configuration::precoder_granularity_type::same_as_reg_bundle,
           "CORESET#0 Precoder Granularity");

  pdcch_type0_css_coreset_description coreset0_param =
      pdcch_type0_css_coreset_get(cell_cfg.dl_carrier.band,
                                  cell_cfg.ssb_cfg.scs,
                                  cell_cfg.scs_common,
                                  cell_cfg.coreset0_idx,
                                  static_cast<uint8_t>(cell_cfg.ssb_cfg.k_ssb.to_uint()));

  // CRB (with reference to SCScommon carrier) pointed to by offset_to_point_A.
  unsigned crb_ssb = cell_cfg.scs_common == subcarrier_spacing::kHz15
                         ? cell_cfg.ssb_cfg.offset_to_point_A.to_uint()
                         : cell_cfg.ssb_cfg.offset_to_point_A.to_uint() / 2;

  // Verify that Coreset0 does not start before pointA.
  CHECK_TRUE(static_cast<unsigned>(coreset0_param.offset) <= crb_ssb, "CORESET#0 starts before pointA.");
  // Check if Coreset0 is within the Initial DL BWP CRBs.
  crb_interval initial_bwp_crbs{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs};
  crb_interval coreset0_crbs{crb_ssb - coreset0_param.offset,
                             crb_ssb - coreset0_param.offset + coreset0_param.nof_rb_coreset};
  CHECK_TRUE(initial_bwp_crbs.contains(coreset0_crbs),
             "The CORESET#0 CRBs [{}, {}) falls outside of the initial DL BWP CRBs [{}, {})",
             coreset0_crbs.start(),
             coreset0_crbs.stop(),
             initial_bwp_crbs.start(),
             initial_bwp_crbs.stop());

  return {};
}

static check_outcome is_search_space_valid(const search_space_configuration& ss_cfg)
{
  CHECK_EQ_OR_BELOW(ss_cfg.get_id(), srsran::MAX_SEARCH_SPACE_ID, "SearchSpace Id={}", ss_cfg.get_id());
  CHECK_EQ_OR_BELOW(ss_cfg.get_coreset_id(),
                    srsran::MAX_CORESET_ID,
                    "SearchSpace#{} CORESET Id={}",
                    ss_cfg.get_id(),
                    ss_cfg.get_coreset_id());
  const bool valid_period =
      is_valid_enum_number<asn1::rrc_nr::search_space_s::monitoring_slot_periodicity_and_offset_c_::types>(
          ss_cfg.get_monitoring_slot_periodicity());
  CHECK_TRUE(
      valid_period, "Invalid SearchSpace#{} slot period={}", ss_cfg.get_id(), ss_cfg.get_monitoring_slot_periodicity());
  CHECK_BELOW(ss_cfg.get_monitoring_slot_offset(),
              ss_cfg.get_monitoring_slot_periodicity(),
              "SearchSpace#{} monitoring slot offset",
              ss_cfg.get_id());
  CHECK_EQ_OR_BELOW(ss_cfg.get_duration(),
                    ss_cfg.get_monitoring_slot_periodicity(),
                    "SearchSpace#{} monitoring slot duration",
                    ss_cfg.get_id());
  CHECK_NEQ(ss_cfg.get_duration(), 0, "SearchSpace#{} monitoring slot duration", ss_cfg.get_id());
  return {};
}

static check_outcome check_dl_config_common(const du_cell_config& cell_cfg)
{
  const bwp_downlink_common& bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
  // PDCCH
  if (bwp.pdcch_common.coreset0.has_value()) {
    HANDLE_ERROR(is_coreset0_params_valid(cell_cfg));
  }
  if (bwp.pdcch_common.sib1_search_space_id != srsran::MAX_NOF_SEARCH_SPACES) {
    CHECK_EQ(bwp.pdcch_common.sib1_search_space_id, 0, "SearchSpaceSIB1 must be equal to 0 for initial DL BWP");
  }
  if (bwp.pdcch_common.common_coreset.has_value()) {
    CHECK_NEQ(bwp.pdcch_common.common_coreset->id, 0, "Common CORESET");
  }
  for (const search_space_configuration& ss : bwp.pdcch_common.search_spaces) {
    HANDLE_ERROR(is_search_space_valid(ss));
    CHECK_TRUE(ss.is_common_search_space(), "Common SearchSpace#{} type", ss.get_id());
    const auto dci_format_variant = ss.get_monitored_dci_formats();
    const auto dci_format         = variant_get<search_space_configuration::common_dci_format>(dci_format_variant);
    CHECK_TRUE(dci_format.f0_0_and_f1_0, "Common SearchSpace#{} must enable DCI format1_0 and format0_0", ss.get_id());
    if (ss.get_coreset_id() == 0) {
      CHECK_TRUE(bwp.pdcch_common.coreset0.has_value(),
                 "Common SearchSpace#{} points to CORESET#0 which is inactive",
                 ss.get_id());
    } else {
      CHECK_TRUE(bwp.pdcch_common.common_coreset.has_value() and
                     ss.get_coreset_id() == bwp.pdcch_common.common_coreset->id,
                 "common SearchSpace#{} points to CORESET#{} which is inactive",
                 ss.get_id(),
                 ss.get_coreset_id());
    }
  }
  // PDSCH
  CHECK_TRUE(not bwp.pdsch_common.pdsch_td_alloc_list.empty(), "Empty PDSCH-TimeDomainAllocationList");
  for (const auto& pdsch : bwp.pdsch_common.pdsch_td_alloc_list) {
    CHECK_EQ_OR_BELOW(pdsch.k0, 32, "PDSCH k0");
    // TODO: Remaining.
  }
  return {};
}

static check_outcome check_dl_config_dedicated(const du_cell_config& cell_cfg)
{
  const bwp_downlink_dedicated&     bwp = cell_cfg.ue_ded_serv_cell_cfg.init_dl_bwp;
  const search_space_configuration& ss2 = cell_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces.back();
  const bool                        fallback_dci_format_in_ss2 =
      ss2.is_common_search_space() or
      not(variant_get<search_space_configuration::ue_specific_dci_format>(ss2.get_monitored_dci_formats()) ==
          search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);

  if (fallback_dci_format_in_ss2) {
    CHECK_TRUE(cell_cfg.dl_carrier.nof_ant == 1,
               "Nof. DL antennas {} cannot be greater than 1 when using fallback DCI format\n",
               cell_cfg.dl_carrier.nof_ant);

    if (bwp.pdsch_cfg.has_value()) {
      CHECK_TRUE(bwp.pdsch_cfg->mcs_table != pdsch_mcs_table::qam256,
                 "256QAM MCS table cannot be used for PDSCH with fallback DCI format in SearchSpace#2");
    }
  }

  return {};
}

static check_outcome check_ssb_configuration(const du_cell_config& cell_cfg)
{
  const ssb_configuration& ssb_cfg = cell_cfg.ssb_cfg;

  // No mixed numerologies supported (yet).
  CHECK_EQ(
      ssb_cfg.scs, cell_cfg.scs_common, "SSB SCS must be equal to SCS common. Mixed numerologies are not supported.");

  // Only FR1 SCS supported (for now).
  CHECK_EQ_OR_BELOW(ssb_cfg.scs,
                    subcarrier_spacing::kHz30,
                    "SSB SCS must be kHz15 or kHz30.  FR2 frequencies are not supported yet in the SSB scheduler.");

  CHECK_EQ(ssb_cfg.ssb_bitmap,
           static_cast<uint64_t>(1U) << static_cast<uint64_t>(63U),
           "Multiple beams not supported for SSB.");

  // Checks that SSB does not get located outside the band.
  if (cell_cfg.scs_common == subcarrier_spacing::kHz15) {
    // Check if k_SSB is within limits, according to the SCScommon.
    CHECK_EQ_OR_BELOW(ssb_cfg.k_ssb.to_uint(), 11, "For SCS common 15kHz, k_SSB must be within the range [0, 11].");

    // In the following, we assume the SSB is located inside the Transmission Bandwidth Configuration of the specified
    // band. Refer to TS38.104, Section 5.3.1 for the definition of Transmission Bandwidth Configuration.
    // We assume the Initial DL BWP ranges over the whole Transmission Bandwidth Configuration.
    unsigned nof_crbs = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
    unsigned offset_p_A_upper_bound =
        ssb_cfg.k_ssb.to_uint() > 0 ? nof_crbs - NOF_SSB_PRBS - 1 : nof_crbs - NOF_SSB_PRBS;
    CHECK_EQ_OR_BELOW(
        ssb_cfg.offset_to_point_A.to_uint(),
        offset_p_A_upper_bound,
        "Offset to PointA must be such that the SSB is located inside the Initial DL BWP, i.e, offset_to_point_A <= {}",
        offset_p_A_upper_bound);

  } else if (cell_cfg.scs_common == subcarrier_spacing::kHz30) {
    // Check if k_SSB is within limits, according to the SCScommon.
    CHECK_EQ_OR_BELOW(ssb_cfg.k_ssb.to_uint(), 23, "For SCS common 30kHz, k_SSB must be within the range [0, 23].");
    // Check if k_SSB is an even number, as this is a requirement coming from PHY implementation limitation.
    CHECK_TRUE(ssb_cfg.k_ssb.to_uint() % 2 == 0, "For SCS common 30kHz, k_SSB must be an even number.");

    // In the following, we assume the SSB is located inside the Transmission Bandwidth Configuration of the specified
    // band. Refer to TS38.104, Section 5.3.1 for the definition of Transmission Bandwidth Configuration.
    // We assume the Initial DL BWP ranges over the whole Transmission Bandwidth Configuration.
    unsigned nof_crbs = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
    unsigned offset_p_A_upper_bound =
        ssb_cfg.k_ssb.to_uint() > 0 ? (nof_crbs - NOF_SSB_PRBS - 1) * 2 : (nof_crbs - NOF_SSB_PRBS) * 2;
    CHECK_EQ_OR_BELOW(
        ssb_cfg.offset_to_point_A.to_uint(),
        offset_p_A_upper_bound,
        "Offset to PointA must be such that the SSB is located inside the Initial DL BWP, i.e, offset_to_point_A <= {}",
        offset_p_A_upper_bound);
    CHECK_TRUE(ssb_cfg.offset_to_point_A.to_uint() % 2 == 0,
               "With SCScommon 30kHz, Offset to PointA must be an even number",
               offset_p_A_upper_bound);
  }

  ssb_pattern_case ssb_case   = band_helper::get_ssb_pattern(cell_cfg.dl_carrier.band, ssb_cfg.scs);
  uint8_t          ssb_bitmap = static_cast<uint64_t>(ssb_cfg.ssb_bitmap) << static_cast<uint64_t>(56U);
  bool             is_paired  = band_helper::is_paired_spectrum(cell_cfg.dl_carrier.band);
  uint8_t          L_max      = ssb_get_L_max(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn, cell_cfg.dl_carrier.band);
  double           cutoff_freq_mhz_case_a_b_c      = band_helper::nr_arfcn_to_freq(cell_cfg.dl_carrier.arfcn) / 1e6;
  double           cutoff_freq_mhz_case_c_unpaired = band_helper::nr_arfcn_to_freq(cell_cfg.dl_carrier.arfcn) / 1e6;

  // Check whether the SSB beam bitmap and L_max are compatible with SSB case and DL band.
  if (ssb_case == ssb_pattern_case::C and not is_paired) {
    if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED) {
      CHECK_EQ(L_max, 4, "For SSB case C and frequency <= {}MHz, L_max must be 4", cutoff_freq_mhz_case_c_unpaired);
      CHECK_TRUE((ssb_bitmap & 0b00001111) == 0,
                 "For SSB case C and frequency <= {}MHz, only the 4 MSBs of SSB bitmap can be set",
                 cutoff_freq_mhz_case_c_unpaired);
    } else {
      CHECK_EQ(L_max, 8, "For SSB case C and frequency > {}MHz, L_max must be 8", cutoff_freq_mhz_case_c_unpaired);
    }
  } else {
    if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C) {
      CHECK_EQ(L_max, 4, "For SSB case A and B and frequency <= {}MHz, L_max must be 4", cutoff_freq_mhz_case_a_b_c);
      CHECK_TRUE((ssb_bitmap & 0b00001111) == 0,
                 "For SSB case C and frequency <= {}MHz, only the 4 MSBs of SSB bitmap can be set",
                 cutoff_freq_mhz_case_a_b_c);
    } else {
      CHECK_EQ(L_max, 8, "For SSB case A and B and frequency > {}MHz, L_max must be 8", cutoff_freq_mhz_case_a_b_c);
    }
  }

  return {};
}

static check_outcome check_ul_config_common(const du_cell_config& cell_cfg)
{
  const bwp_uplink_common& bwp = cell_cfg.ul_cfg_common.init_ul_bwp;
  if (bwp.pusch_cfg_common.has_value()) {
    const pusch_config_common& pusch = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value();
    CHECK_TRUE(pusch.msg3_delta_power.valid(),
               "msg3_delta_power{} in pucch_config_common not in range [-6, 8]",
               pusch.msg3_delta_power.to_int());
    CHECK_TRUE(pusch.msg3_delta_power.to_int() % 2 == 0,
               "The value set {} for msg3_delta_power must be a multiple of 2",
               pusch.msg3_delta_power.to_int());
  }
  return {};
}

static check_outcome check_ul_config_dedicated(const du_cell_config& cell_cfg)
{
  if (not cell_cfg.ue_ded_serv_cell_cfg.ul_config.has_value()) {
    return {};
  }

  const bwp_uplink_dedicated& bwp = cell_cfg.ue_ded_serv_cell_cfg.ul_config->init_ul_bwp;
  if (bwp.pusch_cfg.has_value()) {
    const search_space_configuration& ss2 = cell_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces.back();
    const bool                        fallback_dci_format_in_ss2 =
        ss2.is_common_search_space() or
        not(variant_get<search_space_configuration::ue_specific_dci_format>(ss2.get_monitored_dci_formats()) ==
            search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
    if (fallback_dci_format_in_ss2) {
      CHECK_TRUE(bwp.pusch_cfg->mcs_table != pusch_mcs_table::qam256,
                 "256QAM MCS table cannot be used for PUSCH with fallback DCI format in SearchSpace#2");
    }
  }
  return {};
}

static check_outcome check_tdd_ul_dl_config(const du_cell_config& cell_cfg)
{
  if (not cell_cfg.tdd_ul_dl_cfg_common.has_value()) {
    return {};
  }

  // See TS 38.214, Table 5.1.2.1-1: Valid S and L combinations.
  static const unsigned pdsch_mapping_typeA_min_L_value = 3;

  const pdcch_config_common&            common_pdcch_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common;
  const pdcch_config&                   ded_pdcch_cfg    = cell_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg.value();
  const optional<coreset_configuration> coreset0         = common_pdcch_cfg.coreset0;
  const optional<coreset_configuration> common_coreset   = common_pdcch_cfg.common_coreset;
  const pdcch_type0_css_occasion_pattern1_description ss0_occasion = pdcch_type0_css_occasions_get_pattern1(
      pdcch_type0_css_occasion_pattern1_configuration{.is_fr2        = false,
                                                      .ss_zero_index = static_cast<uint8_t>(cell_cfg.searchspace0_idx),
                                                      .nof_symb_coreset = coreset0->duration});

  const auto& tdd_cfg                  = cell_cfg.tdd_ul_dl_cfg_common.value();
  const auto  pattern1_period_slots    = tdd_cfg.pattern1.dl_ul_tx_period_nof_slots;
  auto        pattern1_additional_slot = tdd_cfg.pattern1.nof_ul_symbols + tdd_cfg.pattern1.nof_dl_symbols > 0 ? 1 : 0;
  pattern1_additional_slot += tdd_cfg.pattern1.nof_ul_symbols + tdd_cfg.pattern1.nof_dl_symbols > 14 ? 1 : 0;
  CHECK_EQ_OR_BELOW(tdd_cfg.pattern1.nof_dl_slots + tdd_cfg.pattern1.nof_ul_slots + pattern1_additional_slot,
                    pattern1_period_slots,
                    "TDD UL DL pattern 1 configuration. UL(slots + symbols) + DL(slots + symbols) configuration "
                    "exceeds TDD pattern period.");

  if (tdd_cfg.pattern2.has_value()) {
    const auto pattern2_period_slots = tdd_cfg.pattern2.value().dl_ul_tx_period_nof_slots;
    auto       pattern2_additional_slot =
        tdd_cfg.pattern2.value().nof_ul_symbols + tdd_cfg.pattern2.value().nof_dl_symbols > 0 ? 1 : 0;
    pattern2_additional_slot +=
        tdd_cfg.pattern2.value().nof_ul_symbols + tdd_cfg.pattern2.value().nof_dl_symbols > 14 ? 1 : 0;
    CHECK_EQ_OR_BELOW(tdd_cfg.pattern2.value().nof_dl_slots + tdd_cfg.pattern2.value().nof_ul_slots +
                          pattern2_additional_slot,
                      pattern2_period_slots,
                      "TDD UL DL pattern 2 configuration. UL(slots + symbols) + DL(slots + symbols) configuration "
                      "exceeds TDD pattern period.");
  }

  // NOTE1: Number of DL PDSCH symbols must be atleast greater than SearchSpace monitoring start symbol index + CORESET
  // duration for PDSCH allocation in partial slot. Otherwise, it can be used only for PDCCH allocations.
  // NOTE2: We don't support multiple monitoring occasions in a slot belonging to a SearchSpace.
  // TODO: Consider SearchSpace periodicity while validating DL symbols in special slots.
  optional<unsigned> cs_duration;
  unsigned           ss_start_symbol_idx;
  // SearchSpaces in Common PDCCH configuration.
  for (const search_space_configuration& ss_cfg : common_pdcch_cfg.search_spaces) {
    cs_duration         = {};
    ss_start_symbol_idx = 0;
    if (coreset0.has_value()) {
      if (ss_cfg.get_id() == to_search_space_id(0) and ss_cfg.get_coreset_id() == coreset0->id) {
        cs_duration = coreset0->duration;
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (ss_cfg.get_id() != to_search_space_id(0) and ss_cfg.get_coreset_id() == coreset0->id) {
        cs_duration         = coreset0->duration;
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }
    if (common_coreset.has_value()) {
      if (ss_cfg.get_id() == to_search_space_id(0) and ss_cfg.get_coreset_id() == common_coreset->id) {
        cs_duration = common_coreset->duration;
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (ss_cfg.get_id() != to_search_space_id(0) and ss_cfg.get_coreset_id() == common_coreset->id) {
        cs_duration         = common_coreset->duration;
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }

    if (tdd_cfg.pattern1.nof_dl_symbols > 0) {
      // Ensuring there is enough DL symbols for PDCCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value(),
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                        "{}) for SearchSpace#{}.",
                        ss_start_symbol_idx,
                        ss_start_symbol_idx + cs_duration.value(),
                        ss_cfg.get_id());

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        ss_cfg.get_id());
    }

    if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_dl_symbols > 0) {
      // Ensuring there is enough DL symbols for PDCCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value(),
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                        "{}) for SearchSpace#{}.",
                        ss_start_symbol_idx,
                        ss_start_symbol_idx + cs_duration.value(),
                        ss_cfg.get_id());

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        ss_cfg.get_id());
    }
  }
  // SearchSpaces in Dedicated PDCCH configuration.
  for (const search_space_configuration& ss_cfg : ded_pdcch_cfg.search_spaces) {
    cs_duration         = {};
    ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
    if (coreset0.has_value()) {
      if (ss_cfg.get_coreset_id() == coreset0->id) {
        cs_duration = coreset0->duration;
      }
    }
    if ((not cs_duration.has_value()) and common_coreset.has_value()) {
      if (ss_cfg.get_coreset_id() == common_coreset->id) {
        cs_duration = common_coreset->duration;
      }
    }

    if (not cs_duration.has_value()) {
      for (const coreset_configuration& cs_cfg : ded_pdcch_cfg.coresets) {
        if (ss_cfg.get_coreset_id() == cs_cfg.id) {
          cs_duration = cs_cfg.duration;
          break;
        }
      }
    }

    CHECK_TRUE(cs_duration.has_value(), "CORESET not configured for SearchSpace#{}", ss_cfg.get_id());

    if (tdd_cfg.pattern1.nof_dl_symbols > 0) {
      // Ensuring there is atleast 1 OFDM symbol for PDSCH.
      CHECK_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                  ss_start_symbol_idx + cs_duration.value(),
                  "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                  "{}) for SearchSpace#{}.",
                  ss_start_symbol_idx,
                  ss_start_symbol_idx + cs_duration.value(),
                  ss_cfg.get_id());

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        ss_cfg.get_id());
    }

    if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_dl_symbols > 0) {
      // Ensuring there is atleast 1 OFDM symbol for PDSCH.
      CHECK_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                  ss_start_symbol_idx + cs_duration.value(),
                  "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                  "{}) for SearchSpace#{}.",
                  ss_start_symbol_idx,
                  ss_start_symbol_idx + cs_duration.value(),
                  ss_cfg.get_id());

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        ss_cfg.get_id());
    }
  }

  return {};
}

check_outcome srsran::is_du_cell_config_valid(const du_cell_config& cell_cfg)
{
  CHECK_EQ_OR_BELOW(cell_cfg.pci, MAX_PCI, "cell PCI");
  CHECK_EQ_OR_BELOW(cell_cfg.scs_common, subcarrier_spacing::kHz120, "SCS common");
  HANDLE_ERROR(is_coreset0_ss0_idx_valid(cell_cfg));
  HANDLE_ERROR(check_dl_config_common(cell_cfg));
  HANDLE_ERROR(check_ul_config_common(cell_cfg));
  HANDLE_ERROR(check_ssb_configuration(cell_cfg));
  HANDLE_ERROR(check_tdd_ul_dl_config(cell_cfg));
  const pucch_builder_params& pucch_cfg = cell_cfg.pucch_cfg;
  HANDLE_ERROR(srs_du::pucch_parameters_validator(pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint(),
                                                  pucch_cfg.nof_ue_pucch_f2_res_harq.to_uint(),
                                                  pucch_cfg.f1_params,
                                                  pucch_cfg.f2_params,
                                                  cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length()));
  HANDLE_ERROR(config_validators::validate_csi_meas_cfg(cell_cfg.ue_ded_serv_cell_cfg, cell_cfg.tdd_ul_dl_cfg_common));
  HANDLE_ERROR(check_dl_config_dedicated(cell_cfg));
  HANDLE_ERROR(check_ul_config_dedicated(cell_cfg));
  // TODO: Remaining.
  return {};
}
