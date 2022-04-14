#include "sched_ssb.h"
#include "../../mac/sched/cell/resource_grid.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/ssb_mapping.h"

/// The cutoff frequency for case A, B and C paired is 3GHz, corresponding to 600000 ARFCN (TS 38.213, Section 4.1).
#define CUTOFF_FREQ_ARFCN_CASE_A_B_C 600000
/// The cutoff frequency for case C unpaired is 1.88GHz, corresponding to 376000 ARFCN (TS 38.213, Section 4.1).
#define CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED 376000

namespace srsgnb {

static void
fill_ssb_parameters(ssb_information_list& ssb_list, uint32_t offset_to_point_A, uint8_t ofdm_sym_idx, uint8_t ssb_idx)
{
  ssb_information ssb_msg = {};

  ssb_msg.ssb_index = ssb_idx;
  ssb_msg.symbols.set(ofdm_sym_idx, ofdm_sym_idx + NOF_SSB_OFDM_SYMBOLS);
  // The value below is common for all SSB, regardless of the different parameters or cases.
  // NOTE: It assumes the reference grid is that of the SSB
  ssb_msg.prbs.set(offset_to_point_A, offset_to_point_A + NOF_SSB_PRBS);
  // NOTE: until we understand whether the time and frequency allocation refer to the common grid or SSB grid, we
  // leave this extra line (commented) below, which will be used if the reference is the common grid;
  // ssb_msg.ssb_info.prb_alloc.set(offset_to_point_A, NOF_SSB_PRBS + floor(k_ssb % MAX_K_SSB)).
  ssb_list.push_back(ssb_msg);
}

/// Perform allocation for case A and C (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
static void ssb_alloc_case_A_C(ssb_information_list& ssb_list,
                               uint32_t              freq_arfcn,
                               uint32_t              freq_arfcn_cut_off,
                               const slot_point&     sl_point_mod,
                               uint32_t              offset_to_point_A,
                               uint8_t               in_burst_bitmap)
{
  uint32_t slot_idx = sl_point_mod.to_uint();

  // The OFDM symbols allocations for Case A and case C are identical; the only difference is the cutoff frequency,
  // which is 3GHz for case A and C paired, but 1.88GHz for case C unpaired.
  // For frequency lower than cutoff, SSB is allocated in slot 0 and 1 only.
  if (freq_arfcn <= freq_arfcn_cut_off and slot_idx > 1) {
    return;
  }

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
        fill_ssb_parameters(
            ssb_list, offset_to_point_A, ofdm_symbols[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, ssb_idx);
      }
      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

/// Perform SSB allocation for case B (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
static void ssb_alloc_case_B(ssb_information_list& ssb_list,
                             uint32_t              freq_arfcn,
                             const slot_point&     sl_point_mod,
                             uint32_t              offset_to_point_A,
                             uint8_t               in_burst_bitmap)
{
  uint32_t slot_idx = sl_point_mod.to_uint();

  // For frequency lower than cutoff, SSB is allocated in slot 0 and 1 only.
  if (freq_arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C and slot_idx > 1) {
    return;
  }

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
        fill_ssb_parameters(
            ssb_list, offset_to_point_A, ofdm_symbols[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, ssb_idx);
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
        fill_ssb_parameters(
            ssb_list, offset_to_point_A, ofdm_symbols[n] + (slot_idx - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, ssb_idx);
      }
      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

void sched_ssb(slot_resource_allocator slot_allocator,
               const slot_point&       sl_point,
               uint8_t                 ssb_periodicity,
               uint16_t                offset_to_point_A,
               uint32_t                freq_arfcn,
               uint64_t                ssb_in_burst_bitmap,
               ssb_pattern_case        ssb_case,
               bool                    paired_spectrum)
{
  ssb_information_list& ssb_list = slot_allocator.dl_res().bc.ssb_info;

  if (ssb_list.full()) {
    srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate SSB");
    return;
  }

  // If the periodicity is 0, it means that the parameter was not passed by the upper layers.
  // In that case, we use default value of 5ms (see Clause 4.1, TS 38.213).
  if (ssb_periodicity == 0) {
    ssb_periodicity = DEFAULT_SSB_PERIODICITY;
  }

  // Only FR1 are supported in this implementation
  srsran_assert(freq_arfcn < static_cast<uint32_t>(FR1_MAX_FREQUENCY_ARFCN),
                "Frenquencies in the range FR2 not supported");

  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval.
  slot_point sl_point_mod(sl_point.numerology(),
                          sl_point.to_uint() % (ssb_periodicity * sl_point.nof_slots_per_subframe()));

  // Extract the 8 MSB bits from 64-bit ssb_in_burst_bitmap - Currently only L_max = 4 or 8 are supported.
  uint8_t in_burst_bitmap = static_cast<uint8_t>(ssb_in_burst_bitmap >> 56U);

  // Select SSB case with reference to TS 38.213, Section 4.1.
  switch (ssb_case) {
    case ssb_pattern_case::A:
    case ssb_pattern_case::C: {
      uint32_t ssb_cut_off_freq = paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      ssb_alloc_case_A_C(ssb_list, freq_arfcn, ssb_cut_off_freq, sl_point_mod, offset_to_point_A, in_burst_bitmap);
      break;
    }
    case ssb_pattern_case::B:
      ssb_alloc_case_B(ssb_list, freq_arfcn, sl_point_mod, offset_to_point_A, in_burst_bitmap);
      break;
    default:
      srsran_assert(ssb_case < ssb_pattern_case::invalid, "Only SSB case A, B and C are currently supported");
  }

  // Update the used DL PRBs with those allocated to the SSBs.
  for (auto& ssb : ssb_list) {
    slot_allocator.used_bwp_dl_prbs() |= ssb.prbs;
  }
}

} // namespace srsgnb
