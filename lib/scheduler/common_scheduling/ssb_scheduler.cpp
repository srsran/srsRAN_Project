/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ssb_scheduler.h"
#include "../cell/resource_grid.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/ssb_mapping.h"

using namespace srsgnb;

subcarrier_spacing srsgnb::ssb_case_to_scs(ssb_pattern_case ssb_case)
{
  switch (ssb_case) {
    case ssb_pattern_case::A:
      return subcarrier_spacing::kHz15;
    case ssb_pattern_case::B:
    case ssb_pattern_case::C:
      return subcarrier_spacing::kHz30;
    case ssb_pattern_case::D:
      return subcarrier_spacing::kHz120;
    case ssb_pattern_case::E:
      return subcarrier_spacing::kHz240;
    default:
      srsran_terminate("Invalid SSB pattern");
  }
  return subcarrier_spacing::invalid;
}

static ofdm_symbol_range ssb_symbols_to_slot_symbols(ofdm_symbol_range ssb_symbols)
{
  // TODO: Account for mixed numerologies.
  ofdm_symbol_range symbols{ssb_symbols.start() % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_symbols.stop() % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP};
  return symbols;
}

static void fill_ssb_parameters(ssb_information_list& ssb_list,
                                uint32_t              offset_to_point_A,
                                uint8_t               ssb_subcarrier_offset,
                                uint8_t               ofdm_sym_idx,
                                uint8_t               ssb_idx)
{
  ssb_information ssb_msg = {};

  ssb_msg.ssb_index = ssb_idx;
  ssb_msg.symbols.set(ofdm_sym_idx, ofdm_sym_idx + NOF_SSB_OFDM_SYMBOLS);
  // As per TS 38.211, Section 7.4.3.1, the SSB occupies 240 subcarriers, or 20 PRBs. In the case of FR1, and SSB SCS ==
  // SCScommon, if k_SSB > 1, the SSB PRBs will be shifted with respect to the Common RBs grid; this means that the SSB
  // will overlap over 1 additional CRB (at the end of the SS/PBCH Block).
  uint32_t last_ssb_crb =
      ssb_subcarrier_offset > 0 ? offset_to_point_A + NOF_SSB_PRBS + 1 : offset_to_point_A + NOF_SSB_PRBS;
  ssb_msg.crbs.set(offset_to_point_A, last_ssb_crb);
  ssb_list.push_back(ssb_msg);
}

/// Perform allocation for case A and C (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
static void ssb_alloc_case_A_C(ssb_information_list&     ssb_list,
                               uint32_t                  freq_arfcn_cut_off,
                               const slot_point&         sl_point_mod,
                               const cell_configuration& cell_cfg)
{
  uint32_t slot_idx = sl_point_mod.to_uint();

  // The OFDM symbols allocations for Case A and case C are identical; the only difference is the cutoff frequency,
  // which is 3GHz for case A and C paired, but 1.88GHz for case C unpaired.
  // For frequency lower than cutoff, SSB is allocated in slot 0 and 1 only.
  if (cell_cfg.dl_carrier.arfcn <= freq_arfcn_cut_off and slot_idx > 1) {
    return;
  }

  // Extract the 8 MSB bits from 64-bit ssb_in_burst_bitmap - Currently only L_max = 4 or 8 are supported.
  uint8_t in_burst_bitmap = static_cast<uint8_t>(cell_cfg.ssb_cfg.ssb_bitmap >> 56U);

  // In case A and C, the candidate OFDM symbols where to allocate the SSB within its period are indexed as follows:
  // n = 2, 8, 16, 22  for frequencies <= cutoff frequency
  // n = 2, 8, 16, 22, 30, 36, 44, 50  for frequencies > cutoff frequency
  // Cutoff frequency is: 3GHz for Case A and Case C paired, 1.88GHz for Case C unpaired.
  // Slot 0 has OFDM symbols 2,8; Slot n has symbols {2,8} + 14 * n;
  // TS 38.213 Section 4.1.
  if (slot_idx == 0 or slot_idx == 1 or slot_idx == 2 or slot_idx == 3) {
    // The SSB mask is used to choose only the SSB indices corresponding to the current slot.
    uint8_t                ssb_idx_mask = 0b10000000 >> (slot_idx * 2);
    std::array<uint8_t, 2> ofdm_symbols = {2, 8};

    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if (in_burst_bitmap & ssb_idx_mask) {
        srsran_assert(n < sizeof(ofdm_symbols), "SSB index exceeding OFDM symbols array size");
        uint8_t ssb_idx = n + slot_idx * 2;
        fill_ssb_parameters(ssb_list,
                            cell_cfg.ssb_cfg.ssb_offset_to_point_A,
                            cell_cfg.ssb_cfg.ssb_subcarrier_offset,
                            ofdm_symbols[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

/// Perform SSB allocation for case B (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
static void
ssb_alloc_case_B(ssb_information_list& ssb_list, const slot_point& sl_point_mod, const cell_configuration& cell_cfg)
{
  uint32_t slot_idx = sl_point_mod.to_uint();

  // For frequency lower than cutoff, SSB is allocated in slot 0 and 1 only.
  if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C and slot_idx > 1) {
    return;
  }

  // Extract the 8 MSB bits from 64-bit ssb_in_burst_bitmap - Currently only L_max = 4 or 8 are supported.
  uint8_t in_burst_bitmap = static_cast<uint8_t>(cell_cfg.ssb_cfg.ssb_bitmap >> 56U);

  // In case B, the candidate OFDM symbols where to allocate the SSB within its period are indexed as follows:
  // n = 4, 8, 16, 20  for frequencies <= cutoff frequency
  // n = 4, 8, 16, 20, 32, 36, 44, 48  for frequencies > cutoff frequency
  // Cutoff frequency is: 3GHz for Case B.
  // TS 38.213 Section 4.1.

  // Slot 0 has OFDM symbols 4,8; Slot 2 has symbols 32, 36.
  if (slot_idx == 0 or slot_idx == 2) {
    // The SSB mask is used to choose only the SSB indices corresponding to the current slot.
    uint8_t                ssb_idx_mask = 0b10000000 >> (slot_idx * 2);
    std::array<uint8_t, 2> ofdm_symbols = {4, 8};

    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if (in_burst_bitmap & ssb_idx_mask) {
        srsran_assert(n < sizeof(ofdm_symbols), "SSB index exceeding OFDM symbols array size");
        uint8_t ssb_idx = n + slot_idx * 2;
        fill_ssb_parameters(ssb_list,
                            cell_cfg.ssb_cfg.ssb_offset_to_point_A,
                            cell_cfg.ssb_cfg.ssb_subcarrier_offset,
                            ofdm_symbols[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }

  // Slot 0 has OFDM symbols 4,8; Slot 1 has symbols 16, 20; Slot 3 has symbols 32, 36; Slot 3 has symbols 44, 48.
  if (slot_idx == 1 or slot_idx == 3) {
    // The SSB mask is used to choose only the SSB indices corresponding to the current slot.
    uint8_t                ssb_idx_mask = 0b00100000 >> ((slot_idx - 1) * 2);
    std::array<uint8_t, 2> ofdm_symbols = {16, 20};

    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if (in_burst_bitmap & ssb_idx_mask) {
        srsran_assert(n < sizeof(ofdm_symbols), "SSB index exceeding OFDM symbols array size");
        uint8_t ssb_idx = n + slot_idx * 2;
        fill_ssb_parameters(ssb_list,
                            cell_cfg.ssb_cfg.ssb_offset_to_point_A,
                            cell_cfg.ssb_cfg.ssb_subcarrier_offset,
                            ofdm_symbols[n] + (slot_idx - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

void srsgnb::schedule_ssb(cell_slot_resource_allocator& res_grid,
                          const slot_point&             sl_point,
                          const cell_configuration&     cell_cfg)
{
  ssb_information_list& ssb_list = res_grid.result.dl.bc.ssb_info;

  if (ssb_list.full()) {
    srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate SSB");
    return;
  }

  // Only FR1 are supported in this implementation.
  uint32_t freq_arfcn = cell_cfg.dl_carrier.arfcn;
  srsran_assert(freq_arfcn < static_cast<uint32_t>(FR1_MAX_FREQUENCY_ARFCN),
                "Frenquencies in the range FR2 not supported");

  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval.
  uint8_t    ssb_period = ssb_periodicity_to_value(cell_cfg.ssb_cfg.ssb_period);
  slot_point sl_point_mod(sl_point.numerology(), sl_point.to_uint() % (ssb_period * sl_point.nof_slots_per_subframe()));

  // Select SSB case with reference to TS 38.213, Section 4.1.
  switch (cell_cfg.ssb_case) {
    case ssb_pattern_case::A:
    case ssb_pattern_case::C: {
      uint32_t ssb_cut_off_freq =
          cell_cfg.paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      ssb_alloc_case_A_C(ssb_list, ssb_cut_off_freq, sl_point_mod, cell_cfg);
      break;
    }
    case ssb_pattern_case::B:
      ssb_alloc_case_B(ssb_list, sl_point_mod, cell_cfg);
      break;
    default:
      srsran_assert(cell_cfg.ssb_case < ssb_pattern_case::invalid, "Only SSB case A, B and C are currently supported");
  }

  // Update the used DL PRBs with those allocated to the SSBs.
  for (auto& ssb : ssb_list) {
    grant_info grant{grant_info::channel::ssb,
                     cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                     ssb_symbols_to_slot_symbols(ssb.symbols),
                     ssb.crbs};
    res_grid.dl_res_grid.fill(grant);
  }
}
