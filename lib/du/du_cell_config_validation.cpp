/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/du/du_cell_config_validation.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/scheduler/sched_consts.h"

using namespace srsgnb;

#define HANDLE_RETURN(cond)                                                                                            \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (ret.is_error()) {                                                                                              \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

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

  // This constraint is implementation-defined and comes from the fact that our PDCCH schedule only schedules PDCCH
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

  static const unsigned               min_channel_bw = 5;
  pdcch_type0_css_coreset_description coreset0_param =
      pdcch_type0_css_coreset_get(min_channel_bw,
                                  cell_cfg.ssb_cfg.scs,
                                  cell_cfg.scs_common,
                                  cell_cfg.coreset0_idx,
                                  static_cast<uint8_t>(cell_cfg.ssb_cfg.k_ssb.to_uint()));

  // CRB (with reference to SCScommon carrier) pointed to by offset_to_point_A.
  unsigned crb_ssb = cell_cfg.scs_common == subcarrier_spacing::kHz15
                         ? cell_cfg.ssb_cfg.offset_to_point_A.to_uint()
                         : cell_cfg.ssb_cfg.offset_to_point_A.to_uint() / 2;

  // Check if Coreset0 is within the Initial DL BWP CRBs.
  crb_interval initial_bwp_crbs{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs};
  CHECK_TRUE(static_cast<unsigned>(coreset0_param.offset) <= crb_ssb, "Coreset0 falls outside the Initial DL BWP.");
  CHECK_TRUE(crb_ssb - static_cast<unsigned>(coreset0_param.offset) >= initial_bwp_crbs.start(),
             "Coreset0 falls outside the Initial DL BWP.");
  CHECK_TRUE(crb_ssb - static_cast<unsigned>(coreset0_param.offset) +
                     static_cast<unsigned>(coreset0_param.nof_rb_coreset) <=
                 initial_bwp_crbs.stop(),
             "Coreset0 falls outside the Initial DL BWP.");

  return {};
}

static check_outcome is_search_space_valid(const search_space_configuration& ss_cfg)
{
  CHECK_EQ_OR_BELOW(ss_cfg.id, srsgnb::MAX_SEARCH_SPACE_ID, "SearchSpaceId", ss_cfg.id);
  CHECK_EQ_OR_BELOW(ss_cfg.cs_id, srsgnb::MAX_CORESET_ID, "SearchSpace#{} CORESET ID", ss_cfg.id);
  bool valid_period =
      is_valid_enum_number<asn1::rrc_nr::search_space_s::monitoring_slot_periodicity_and_offset_c_::types>(
          ss_cfg.monitoring_slot_period);
  CHECK_TRUE(valid_period, "Invalid SearchSpace#{} slot period={}", ss_cfg.id, ss_cfg.monitoring_slot_period);
  CHECK_BELOW(
      ss_cfg.monitoring_slot_offset, ss_cfg.monitoring_slot_period, "SearchSpace#{} monitoring slot offset", ss_cfg.id);
  CHECK_EQ_OR_BELOW(
      ss_cfg.duration, ss_cfg.monitoring_slot_period, "SearchSpace#{} monitoring slot duration", ss_cfg.id);
  CHECK_NEQ(ss_cfg.duration, 0, "SearchSpace#{} monitoring slot duration", ss_cfg.id);
  return {};
}

static check_outcome check_dl_config_common(const du_cell_config& cell_cfg)
{
  const bwp_downlink_common& bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
  // PDCCH
  if (bwp.pdcch_common.coreset0.has_value()) {
    HANDLE_RETURN(is_coreset0_params_valid(cell_cfg));
  }
  if (bwp.pdcch_common.sib1_search_space_id != srsgnb::MAX_NOF_SEARCH_SPACES) {
    CHECK_EQ(bwp.pdcch_common.sib1_search_space_id, 0, "SearchSpaceSIB1 must be equal to 0 for initial DL BWP");
  }
  if (bwp.pdcch_common.common_coreset.has_value()) {
    CHECK_NEQ(bwp.pdcch_common.common_coreset->id, 0, "Common CORESET");
  }
  for (const search_space_configuration& ss : bwp.pdcch_common.search_spaces) {
    HANDLE_RETURN(is_search_space_valid(ss));
    CHECK_EQ(ss.type, search_space_configuration::type::common, "common SearchSpace#{} type", ss.id);
    CHECK_TRUE(ss.common.f0_0_and_f1_0, "common SearchSpace#{} must enable DCI format1_0 and format0_0", ss.id);
    if (ss.cs_id == 0) {
      CHECK_TRUE(
          bwp.pdcch_common.coreset0.has_value(), "common SearchSpace#{} points to CORESET#0 which is inactive", ss.id);
    } else {
      CHECK_TRUE(bwp.pdcch_common.common_coreset.has_value() and ss.cs_id == bwp.pdcch_common.common_coreset->id,
                 "common SearchSpace#{} points to CORESET#{} which is inactive",
                 ss.id,
                 ss.cs_id);
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

  ssb_pattern_case ssb_case   = ssb_get_ssb_pattern(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn);
  uint8_t          ssb_bitmap = static_cast<uint64_t>(ssb_cfg.ssb_bitmap) << static_cast<uint64_t>(56U);
  bool    is_paired   = band_helper::is_paired_spectrum(band_helper::get_band_from_dl_arfcn(cell_cfg.dl_carrier.arfcn));
  uint8_t L_max       = ssb_get_L_max(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn);
  double  dl_freq_mhz = band_helper::nr_arfcn_to_freq(cell_cfg.dl_carrier.arfcn) / 1e6;

  // Check whether the SSB beam bitmap and L_max are compatible with SSB case and DL band.
  if (ssb_case == ssb_pattern_case::C and not is_paired) {
    if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED) {
      CHECK_EQ(L_max, 4, "For SSB case C and frequency <= {}MHz, L_max must be 4", dl_freq_mhz);
      CHECK_TRUE((ssb_bitmap & 0b00001111) == 0,
                 "For SSB case C and frequency <= {}MHz, only the 4 MSBs of SSB bitmap can be set",
                 dl_freq_mhz);
    } else {
      CHECK_EQ(L_max, 8, "For SSB case C and frequency > {}MHz, L_max must be 8", dl_freq_mhz);
    }
  } else {
    if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C) {
      CHECK_EQ(L_max, 4, "For SSB case A and B and frequency <= {}MHz, L_max must be 4", dl_freq_mhz);
      CHECK_TRUE((ssb_bitmap & 0b00001111) == 0,
                 "For SSB case C and frequency <= {}MHz, only the 4 MSBs of SSB bitmap can be set",
                 dl_freq_mhz);
    } else {
      CHECK_EQ(L_max, 8, "For SSB case A and B and frequency > {}MHz, L_max must be 8", dl_freq_mhz);
    }
  }

  return {};
}

check_outcome srsgnb::is_du_cell_config_valid(const du_cell_config& cell_cfg)
{
  CHECK_EQ_OR_BELOW(cell_cfg.pci, MAX_PCI, "cell PCI");
  CHECK_EQ_OR_BELOW(cell_cfg.scs_common, subcarrier_spacing::kHz120, "SCS common");
  HANDLE_RETURN(is_coreset0_ss0_idx_valid(cell_cfg));
  HANDLE_RETURN(check_dl_config_common(cell_cfg));
  HANDLE_RETURN(check_ssb_configuration(cell_cfg));
  // TODO: Remaining.
  return {};
}
