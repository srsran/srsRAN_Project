/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// \brief Default paging cycle, used to derive 'T' in TS 38.304. Value rf32 corresponds to 32 radio frames and so on.
enum class paging_cycle { rf32, rf64, rf128, rf256 };

/// \brief Used to paging related configuration.
/// \remark See TS 38.311, PCCH-Config.
struct pcch_config {
  /// \brief Maximum number of paging occasion per paging frame.
  /// \remark See TS 38.311, maxPO-perPF.
  static constexpr unsigned MAX_PO_PER_PF = 4;

  /// \brief Number of paging occasions per paging frame.
  enum class nof_po_per_pf { four, two, one };
  /// Number of paging frames per DRX cycle. nAndPagingFrameOffset in TS 38.311 in divided into \c nof_pf and \c
  /// paging_frame_offset. e.g. for value oneEighthT nof. paging frames per DRX cycle is 8.
  enum class nof_pf_per_drx_cycle { oneT, halfT, quarterT, oneEighthT, oneSixteethT };

  /// \brief Points out the type of first PDCCH monitoring occasion for paging of each PO of the PF.
  enum class first_pdcch_monitoring_occasion_of_po_type {
    sCS15KHZoneT,
    sCS30KHZoneT_SCS15KHZhalfT,
    sCS60KHZoneT_SCS30KHZhalfT_SCS15KHZquarterT,
    sCS120KHZoneT_SCS60KHZhalfT_SCS30KHZquarterT_SCS15KHZoneEighthT,
    sCS120KHZhalfT_SCS60KHZquarterT_SCS30KHZoneEighthT_SCS15KHZoneSixteenthT,
    sCS120KHZquarterT_SCS60KHZoneEighthT_SCS30KHZoneSixteenthT,
    sCS120KHZoneEighthT_SCS60KHZoneSixteenthT,
    sCS120KHZoneSixteenthT,
    not_set
  };

  paging_cycle default_paging_cycle;

  nof_pf_per_drx_cycle nof_pf;
  /// Values {0,...,(T/n) - 1}. e.g. if n = quarterT, then values range from 0 to 3.
  unsigned paging_frame_offset;

  nof_po_per_pf ns;

  /// See TS 38.311, firstPDCCH-MonitoringOccasionOfPO.
  first_pdcch_monitoring_occasion_of_po_type first_pdcch_mo_of_po_type{
      first_pdcch_monitoring_occasion_of_po_type::not_set};
  static_vector<unsigned, MAX_PO_PER_PF> first_pdcch_monitoring_occasion_of_po_value;
};

} // namespace srsgnb
