
#ifndef SRSGNB_SCHED_DCI_H
#define SRSGNB_SCHED_DCI_H

#include <cstdint>

namespace srsgnb {

enum class dci_format { f1_0, f2_0 };

struct dci_dl_ctxt_t {
  dci_format format;
};

struct dci_dl_t {
  bool          ndi;
  uint32_t      pid;
  uint32_t      mcs;
  uint32_t      rv;
  uint32_t      harq_feedback;
  dci_dl_ctxt_t ctx;
};

struct dci_ul_t {
  bool          ndi;
  uint32_t      pid;
  uint32_t      mcs;
  uint32_t      rv;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_DCI_H
