/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/adt/tiny_optional.h"

namespace srsgnb {

/// \brief Default paging cycle, used to derive 'T' in TS 38.304. Value rf32 corresponds to 32 radio frames and so on.
/// \remark See TS 38.331, PagingCycle.
enum class paging_cycle { rf32 = 32, rf64 = 64, rf128 = 128, rf256 = 256 };

/// \brief Used to paging related configuration.
/// \remark See TS 38.331, PCCH-Config.
struct pcch_config {
  /// \brief Maximum number of paging occasion per paging frame.
  /// \remark See TS 38.331, maxPO-perPF.
  static constexpr unsigned MAX_PO_PER_PF = 4;

  /// \brief The number of PDCCH monitoring occasions corresponding to an SSB within a Paging Occasion.
  /// \remark See TS 38.331, nrofPDCCH-MonitoringOccasionPerSSB-InPO. This field is part of PCCH_Config IE and is
  /// present only if cell operates with shared spectrum channel access.
  /// \remark See TS 38.304, clause 7.1, if nrofPDCCH-MonitoringOccasionPerSSB-InPO is not configured its value is equal
  /// to 1.
  static constexpr unsigned NR_OF_PDCCH_MONITORING_OCCASION_PER_SSB_IN_PO = 1;

  /// \brief Number of paging occasions per paging frame.
  enum class nof_po_per_pf { four = 4, two = 2, one = 1 };
  /// Number of paging frames per DRX cycle. nAndPagingFrameOffset in TS 38.331 in divided into \c nof_pf and \c
  /// paging_frame_offset. e.g. for value oneEighthT nof. paging frames per DRX cycle is 8.
  enum class nof_pf_per_drx_cycle { oneT = 1, halfT = 2, quarterT = 4, oneEighthT = 8, oneSixteethT = 16 };

  /// \brief Points out the type of first PDCCH monitoring occasion for paging of each PO of the PF.
  enum class first_pdcch_monitoring_occasion_of_po_type {
    scs15khzOneT,
    scs30khzOneT_scs15khzHalfT,
    scs60khzOneT_scs30khzHalfT_scs15khzQuarterT,
    scs120khzOneT_scs60khzHalfT_scs30khzQuarterT_scs15khzOneEighthT,
    scs120khzHalfT_scs60khzQuarterT_scs30khzOneEighthT_scs15khzOneSixteenthT,
    scs120khzQuarterT_scs60khzOneEighthT_scs30khzOneSixteenthT,
    scs120khzOneEighthT_scs60khzOneSixteenthT,
    scs120khzOneSixteenthT,
  };

  paging_cycle default_paging_cycle;

  nof_pf_per_drx_cycle nof_pf;
  /// Values {0,...,(T/n) - 1}. e.g. if n = quarterT, then values range from 0 to 3.
  unsigned paging_frame_offset{0};

  nof_po_per_pf ns;

  /// See TS 38.331, firstPDCCH-MonitoringOccasionOfPO.
  tiny_optional<first_pdcch_monitoring_occasion_of_po_type> first_pdcch_mo_of_po_type;
  static_vector<unsigned, MAX_PO_PER_PF>                    first_pdcch_monitoring_occasion_of_po_value;
};

} // namespace srsgnb
