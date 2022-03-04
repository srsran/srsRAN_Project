#include "sched_ssb.h"

#define NOF_SLOTS_PER_SUBFRAME 1
#define NOF_SSB_OFDM_SYMBOLS 4
#define NOF_SSB_PRBS 20

namespace srsgnb {

void sched_ssb(const slot_point& sl_point,
               uint16_t          ssb_periodicity,
               uint8_t           k_ssb,
               uint32_t          offset_to_point_A,
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
    ssb_t ssb_msg            = {};
    ssb_msg.tx_mode          = ssb_transmission;
    ssb_msg.ssb_info.ssb_idx = 0; // This corresponds to "Position in Burst" = 1000
    // This value is hard-coded, but needs to be changed
    ssb_msg.ssb_info.ofdm_symbols.set(4, 4 + NOF_SSB_OFDM_SYMBOLS);
    // The value below is common for all SSB, regardless of the different parameters or cases.
    // NOTE: It assumes the reference grid is that of the SSB
    ssb_msg.ssb_info.prb_alloc.set(offset_to_point_A, offset_to_point_A + NOF_SSB_PRBS);
    // NOTE: until we understand whether the time and frequency allocation refer to the common grid or SSB grid, we
    // leave this extra line (commented) below, which will be used if the reference is the common grid;
    // ssb_msg.ssb_info.prb_alloc.set(offset_to_point_A, NOF_SSB_PRBS + floor(k_ssb % MAX_K_SSB));
    ssb_list.push_back(ssb_msg);
  }
}

} // namespace srsgnb