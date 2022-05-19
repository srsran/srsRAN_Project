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

#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include <cstdint>

namespace srsgnb {

/// Aggregation Level of PDCCH allocation.
enum class aggregation_level : uint8_t { n1 = 0, n2, n4, n8 };

inline unsigned to_cce_size(aggregation_level lvl)
{
  return 1U << static_cast<uint8_t>(lvl);
}

enum class dci_dl_format { f1_0, f2_0 };
enum class dci_ul_format { f0_0, f0_1 };

/// \remark See TS 38.212, 7.3.1.2.1 - "Format 1_0".
struct dci_format1_0_info {
  /// Frequency domain resource assignment.
  uint32_t freq_domain_assigment;
  /// Time domain resource assignment. \remark See TS 38.214, 5.1.2.
  uint32_t time_domain_assigment;
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
/// \remark See TS 38.212, 7.3.1.2. DCI formats for scheduling of PDSCH.
struct dci_dl_info {
  rnti_t            rnti;
  uint8_t           cce_index;
  aggregation_level aggr_level;
  dci_dl_format     format_type;
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
