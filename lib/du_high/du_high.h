
#ifndef SRSGNB_DU_HIGH_H
#define SRSGNB_DU_HIGH_H

#include "adapters.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include <memory>

namespace srsgnb {

struct du_high_configuration {
  task_executor*       du_mng_executor = nullptr;
  f1c_pdu_handler*     f1c_pdu_hdl     = nullptr;
  mac_result_notifier* phy_adapter     = nullptr;
};

class du_high
{
public:
  du_high(du_high_configuration cfg_);
  ~du_high();

  void start();
  void stop();

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index);

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index);

private:
  du_high_configuration cfg;

  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1_du_interface>      f1ap;
  std::unique_ptr<mac_interface>        mac;

  rlc_ul_sdu_adapter rlc_sdu_notifier;

  du_manager_mac_event_indicator mac_ev_notifier;

  std::vector<std::unique_ptr<task_worker> >   workers;
  task_executor*                               ctrl_exec;
  std::vector<std::unique_ptr<task_executor> > dl_execs;
  std::unique_ptr<du_l2_ul_executor_mapper>    ul_exec_mapper;
};

} // namespace srsgnb

#endif // SRSGNB_DU_HIGH_H
