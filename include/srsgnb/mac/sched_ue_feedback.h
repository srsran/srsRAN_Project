
#ifndef SRSGNB_SCHED_UE_FEEDBACK_H
#define SRSGNB_SCHED_UE_FEEDBACK_H

#include "bsr_format.h"
#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// \brief Scheduling Request.
/// \remark See ORAN WG8, 9.2.3.2.17 Scheduling Request Indication.
struct sr_indication_message {
  du_cell_index_t   cell_index;
  du_ue_index_t     ue_index;
  rnti_t            crnti;
  bounded_bitset<8> sr_payload;
};

/// UL Buffer Status Report for a single logical channel group.
struct ul_bsr_lcg_report {
  lcg_id_t lcg_id;
  uint32_t nof_bytes;
};

using ul_bsr_lcg_report_list = static_vector<ul_bsr_lcg_report, MAX_LOGICAL_CHANNEL_GROUP>;

/// \brief UL Buffer Status Report.
/// \remark See ORAN WG8, 9.2.3.2.18 UL Buffer Status Report Indication.
struct ul_bsr_indication_message {
  du_cell_index_t        cell_index;
  du_ue_index_t          ue_index;
  rnti_t                 crnti;
  bsr_format             type;
  ul_bsr_lcg_report_list reported_lcgs;
};

class sched_ue_feedback
{
public:
  virtual ~sched_ue_feedback()                              = default;
  virtual void ul_sr_info(const sr_indication_message& sr)  = 0;
  virtual void ul_bsr(const ul_bsr_indication_message& bsr) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_UE_FEEDBACK_H
