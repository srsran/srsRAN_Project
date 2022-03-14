
#ifndef SRSGNB_SCHED_UE_FEEDBACK_H
#define SRSGNB_SCHED_UE_FEEDBACK_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"

namespace srsgnb {

/// TS 38.321 Sec 6.1.3.1
enum class bsr_format { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

/// Logical Channel Group
using lcg_id_t = uint32_t;

const lcg_id_t MAX_LOGICAL_CHANNEL_GROUP = 8;

struct ul_lcg_report {
  lcg_id_t lcg_id;
  uint32_t nof_bytes;
};

using ul_bcg_report_list = static_vector<ul_lcg_report, MAX_LOGICAL_CHANNEL_GROUP>;

struct ul_bsr_indication_message {
  du_cell_index_t    cell_index;
  rnti_t             rnti;
  bsr_format         type;
  ul_bcg_report_list reported_lcgs;
};

class sched_ue_feedback
{
public:
  virtual ~sched_ue_feedback()                              = default;
  virtual void ul_sr_info(rnti_t rnti)                      = 0;
  virtual void ul_bsr(const ul_bsr_indication_message& bsr) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_UE_FEEDBACK_H
