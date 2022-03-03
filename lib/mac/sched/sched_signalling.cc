//
// Created by carlo on 28/2/22.
//

#include "sched_signalling.h"

#define NOF_SLOTS_PER_SUBFRAME 1
#define NOF_SSB_OFDM_SYMBOLS 4
#define NOF_SSB_PRBS 20

namespace srsgnb {

void sched_ssb(const slot_point& sl_point,
               uint16_t          ssb_periodicity,
               uint8_t           k_ssb,
               uint16_t          offset_to_point_A,
               uint64_t          in_burst_bitmap,
               ssb_list_t&       ssb_list)
{
  if (ssb_list.full()) {
    srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate SSB");
    return;
  }

  // If the periodicity is 0, it means that the parameter was not passed by the upper layers.
  // In that case, we use default value of 5ms (see Clause 4.1, TS 38.213)
  if (ssb_periodicity == 0) {
    ssb_periodicity = DEFAULT_SSB_PERIODICITY;
  }

  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval
  uint32_t sl_point_mod = sl_point % (ssb_periodicity * NOF_SLOTS_PER_SUBFRAME);
  // code below is simplified, it assumes 15kHz subcarrier spacing and sub 3GHz carrier
  if (sl_point_mod == 0) {
    ssb_t ssb_msg                             = {};
    ssb_msg.tx_mode                           = ssb_transmission;
    ssb_msg.support_ssb_idx                   = L_max_4;
    ssb_msg.ssb_info.ssb_idx                  = 0; // This corresponds to "Position in Burst" = 1000
    ssb_msg.ssb_info.time_alloc.start_sym_num = 2; // This value is hard-coded, but needs to be changed
    // The value below is common for all SSB, regardless of the different parameters or cases
    ssb_msg.ssb_info.time_alloc.nof_symbols   = NOF_SSB_OFDM_SYMBOLS;
    ssb_msg.ssb_info.freq_alloc.start_prb     = offset_to_point_A;
    ssb_msg.ssb_info.freq_alloc.nof_prbs      = NOF_SSB_PRBS + floor(k_ssb % MAX_K_SSB);
    ssb_list.push_back(ssb_msg);
  }
}

} // namespace srsgnb