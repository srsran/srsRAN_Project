#ifndef SRSGNB_SCHED_SSB_H
#define SRSGNB_SCHED_SSB_H

#include "sched_prb.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"

#define NOF_ODFM_SYMB_PER_SLOT 14

namespace srsgnb {

enum ssb_tx_mode_opt { no_transmission, ssb_transmission, ssb_repetition, ssb_tx_mode_invalid };

enum ssb_alloc_case { ssb_case_A, ssb_case_B, ssb_case_C_paired, ssb_case_C_unpaired, ssb_case_invalid };

struct ssb_info_t {
  uint8_t ssb_idx;
  // The interval below replaces an additional struct with StartSymbolNumber and NumberOfSymbols
  ofdm_symb_interval ofdm_symbols;
  // The interval below replace an additional struct with StartPRB and NumberOfPRBs
  prb_interval prb_alloc;
};

struct ssb_t {
  ssb_tx_mode_opt tx_mode;
  ssb_info_t      ssb_info;
};

using ssb_list_t = srsgnb::static_vector<ssb_t, MAX_NOF_SSB>;

/// @brief Schedule grant for SSB.
///
/// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case
///
/// @param[in]  sl_point            Slot point carrying information about current slot.
/// @param[in]  ssb_periodicity     Periodicity of SSB in ms.
/// @param[in]  offset_to_point_A   (see TS 38.213, Section 4.1)
/// @param[in]  freq_arfcn          Determines which OFDM symbols to use (see TS 38.214, Section 4.1)
/// @param[in]  in_burst_bitmap     Bitmap for ssb-PositionsInBurst (see TS 38.331)
/// @param[in]  ssb_case            Determines which OFDM symbols to use (see TS 38.214, Section 4.1)
/// @param[out]  ssb_list           List of SSB messages to be sent to MAC.
///
/// @remark This function a is basic scheduling function that uses the following simplified assumption:
void sched_ssb(const slot_point& sl_point,
               uint16_t          ssb_periodicity,
               uint32_t          offset_to_point_A,
               uint32_t          freq_arfcn,
               uint8_t           in_burst_bitmap,
               ssb_alloc_case    ssb_case,
               ssb_list_t&       ssb_list);

} // namespace srsgnb

#endif // SRSGNB_SCHED_SSB_H
