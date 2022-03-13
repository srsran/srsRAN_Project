
#ifndef SRSGNB_SCHED_UE_FEEDBACK_H
#define SRSGNB_SCHED_UE_FEEDBACK_H

namespace srsgnb {

enum class bsr_type { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

/// Logical Channel Group
using logical_channel_group = uint32_t;

const logical_channel_group MAX_LOGICAL_CHANNEL_GROUP = 4;

struct ul_lcg_report {
  logical_channel_group lcg_id;
  uint32_t              nof_bytes;
};

using ul_bcg_report_list = static_vector<ul_lcg_report, MAX_LOGICAL_CHANNEL_GROUP>;

struct ul_bsr_indication_message {
  du_cell_index_t    cell_index;
  rnti_t             rnti;
  bsr_type           type;
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
