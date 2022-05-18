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

#include "srsgnb/ran/slot_point.h"
#include <cstdint>

namespace srsgnb {

enum class dci_format { f1_0, f2_0 };

struct dci_dl_context {
  dci_format format;
};

struct dci_dl_info {
  bool           ndi;
  uint32_t       pid;
  uint32_t       mcs;
  uint32_t       rv;
  uint32_t       harq_feedback;
  dci_dl_context ctx;
};

struct dci_ul_info {
  bool     ndi;
  uint32_t pid;
  uint32_t mcs;
  uint32_t rv;
};

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_DCI_H
