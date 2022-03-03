//
// Created by carlo on 28/2/22.
//

#ifndef SRSGNB_SCHED_SIGNALLING_H
#define SRSGNB_SCHED_SIGNALLING_H

#include "sched_prb.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"

#define MAX_K_SSB 23

namespace srsgnb {

enum ssb_tx_mode_opt { no_transmission, ssb_transmission, ssb_repetition, ssb_tx_mode_invalid };

enum ssb_L_max_opt { L_max_4, L_max_8, L_max_64, L_max_invalid };

struct time_domain_allocation_t {
  /// There can be at most 14 symbols per slot
  uint8_t start_sym_num;
  uint8_t nof_symbols;
};

struct freq_domain_allocation_t {
  /// There can be at most 14 symbols per slot
  uint16_t start_prb;
  uint8_t  nof_prbs;
};

struct ssb_info_t {
  uint8_t                  ssb_idx;
  time_domain_allocation_t time_alloc;
  freq_domain_allocation_t freq_alloc;
};

struct ssb_t {
  ssb_tx_mode_opt tx_mode;
  ssb_L_max_opt   support_ssb_idx;
  ssb_info_t      ssb_info;
};

using ssb_list_t = srsgnb::static_vector<ssb_t, MAX_NOF_SSB>;

/// @brief Schedule grant for SSB.
///
/// The functions schedules the SSB according to a given periodicity. This function is a simplified version of an
/// SSB scheduler and has several hard-coded parameters.
///
/// @param[in]  sl_point            Slot point carrying information about current slot.
/// @param[in]  ssb_periodicity     Periodicity of SSB in ms.
/// @param[in]  k_ssb               k_ssb (see TS 38.213, Section 4.1)
/// @param[in]  offset_to_point_A   (see TS 38.211, Section 4.4.4.2)
/// @param[in]  in_burst_bitmap     Bitmap for ssb-PositionsInBurst (see TS 38.331)
/// @param[out]  ssb_list           List of SSB messages to be sent to MAC.
///
/// @remark This function a is basic scheduling function that uses the following simplified assumption:
/// 1) Subcarrier spacing for both SSB and subCarrierSpacingCommon: 15kHz
/// 2) Frequency below 3GHz
/// 3) Position in Burst is 1000, i.e., Only the first SSB of the 4 opportunities gets scheduled
void sched_ssb(const slot_point& sl_point,
               uint16_t          ssb_periodicity,
               uint8_t           k_ssb,
               uint16_t          offset_to_point_A,
               uint64_t          in_burst_bitmap,
               ssb_list_t&       ssb_list);

} // namespace srsgnb

#endif // SRSGNB_SCHED_SIGNALLING_H
