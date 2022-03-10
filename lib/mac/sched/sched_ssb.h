#ifndef SRSGNB_SCHED_SSB_H
#define SRSGNB_SCHED_SSB_H

#include "sched_prb.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Enum based on O-RAN .WG8.AAD.0-v0, Section 9.2.3.3.8
enum ssb_tx_mode_opt { no_transmission, ssb_transmission, ssb_repetition, ssb_tx_mode_invalid };

/// Enum based on SSB cases, TS 38.213, Section 4.1 - Only Case A, B, C are supported
enum class ssb_alloc_case : unsigned { A = 0, B, C_paired, C_unpaired, invalid};

/// Enum based on O-RAN .WG8.AAD.0-v0, Section 9.2.3.3.8
struct ssb_info_t {
  uint8_t ssb_idx;
  /// The interval below replaces an additional struct with StartSymbolNumber and NumberOfSymbols
  ofdm_symb_interval ofdm_symbols;
  /// The interval below replace an additional struct with StartPRB and NumberOfPRBs
  prb_interval prb_alloc;
};

/// Enum based on O-RAN .WG8.AAD.0-v0, Section 9.2.3.3.8
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

} /// namespace srsgnb

#endif /// SRSGNB_SCHED_SSB_H
