/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_DCI_H
#define SRSGNB_SCHEDULER_DCI_H

#include "srsgnb/ran/pdcch/aggregation_level.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include <cstdint>

namespace srsgnb {

struct cce_position {
  /// CCE start index. Values: (0..135).
  unsigned ncce;
  /// Aggregation level of the DCI.
  aggregation_level aggr_lvl;
};

enum class dci_dl_format { f1_0, f1_1, f2_0 };
enum class dci_ul_format { f0_0, f0_1 };

/// \brief DCI format 1_0 unpacked content. It is used for scheduling PDSCH in one cell.
/// It can be scrambled by C-RNTI, RA-RNTI, TC-RNTI, SI-RNTI and P-RNTI.
/// \remark See TS 38.212, 7.3.1.2.1 - "Format 1_0".
struct dci_format1_0_info {
  /// Frequency domain resource assignment.
  uint8_t freq_domain_assignment;
  /// Time domain resource assignment. \remark See TS 38.214, 5.1.2.
  uint8_t time_domain_assignment;
  /// Values: (0..31).
  uint8_t mcs;
  /// New data indicator.
  bool ndi;
  /// Redundancy version.
  uint8_t rv;
  /// Values: (0..15).
  uint8_t harq_id;
  uint8_t dai;
};

/// \brief Describes an unpacked DL DCI message.
/// \remark See FAPI DCI PDU and ORAN WG8 DL-DCI Configuration.
struct dci_dl_info {
  /// DCI format type used.
  dci_dl_format format_type;
  union {
    dci_format1_0_info f1_0;
  };
};

/// \remark See TS 38.212, 7.3.1.1.1 - "Format 0_0".
struct dci_format0_0_info {
  uint32_t freq_domain_assigment;
  uint32_t time_domain_assigment;
  bool     freq_hopping;
  uint8_t  mcs;
  bool     ndi;
  uint8_t  rv;
  uint8_t  harq_id;
  int8_t   tpc;
  bool     ul_sul_indicator;
};

struct dci_ul_info {
  bool          ndi;
  uint32_t      pid;
  uint32_t      mcs;
  uint32_t      rv;
  dci_ul_format format_type;
  union {
    dci_format0_0_info f0_0;
  };
};

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_DCI_H
