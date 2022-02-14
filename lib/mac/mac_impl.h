
#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "mac_ctxt.h"
#include "mac_logical_channel.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/optional_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/task_executor.h"
#include "ue_creation_procedure.h"
#include "ue_delete_procedure.h"
#include <mutex>

namespace srsgnb {

struct mac_ue {
  du_cell_index_t pcell_idx;
  du_ue_index_t   du_ue_index;
  rnti_t          rnti;
};

/// Class to manage MAC UE control/metric context
class mac_ue_map
{
public:
  mac_ue_map();

  bool insert(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index);
  bool erase(du_ue_index_t ue_index);

  mac_ue* find(du_ue_index_t ue_index);
  mac_ue* find_by_rnti(rnti_t rnti);

private:
  struct element {
    bool   present;
    mac_ue ue;
  };

  std::array<element, MAX_NOF_UES>       ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index_map;
};

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(mac_config_notifier&     listener,
                    mac_northbound_notifier& ul_sdu_notifier,
                    task_executor&           ul_exec,
                    span<task_executor*>     dl_execs);

  void ue_create_request(const mac_ue_create_request_message& cfg) override;
  void ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) override {}
  void ue_delete_request(const mac_ue_delete_request_message& cfg) override;

  void push_ul_pdu(rnti_t rnti, du_cell_index_t cell_index, byte_buffer pdu) override;

  void slot_indication(slot_point sl_tx, du_cell_index_t cc) override;

  // Sched->MAC interface
  void sched_ue_delete_response(rnti_t rnti);
  void sched_ue_config_response(rnti_t rnti);

private:
  srslog::basic_logger& logger;

  std::unique_ptr<sched_cfg_notifier> sched_notifier;
  sched                               sched_obj;
  mac_context                         ctxt;
  mac_northbound_notifier&            northbound_notifier;

  std::unique_ptr<mac_ue_create_request_procedure> ue_create_proc;
  std::unique_ptr<mac_ue_delete_procedure>         ue_delete_proc;

  mac_ue_map ue_db;

  std::mutex dl_mutex;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
