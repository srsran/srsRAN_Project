
#ifndef SRSGNB_DU_H
#define SRSGNB_DU_H

#include "adapters.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/task_executor.h"
#include "srsgnb/support/task_worker.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {

class du
{
public:
  du();
  ~du();

  void start();
  void stop();

  void push_pusch(rnti_t rnti, lcid_t lcid, byte_buffer pdu);

  std::string query(const std::string& s);

private:
  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1ap_du_interface>    f1ap;
  std::unique_ptr<rlc_interface>        rlc;
  std::unique_ptr<mac_interface>        mac;

  rlc_cfg_adapter        rlc_cfg_notifier;
  mac_cfg_adapter        mac_cfg_notifier;
  mac_northbound_adapter mac_ul_sdu_notifier;
  du_f1ap_cfg_adapter    f1ap_cfg_notifier;
  f1ap_du_rlc_connector  f1ap_pdu_adapter;

  std::vector<std::unique_ptr<task_worker> >   workers;
  std::unique_ptr<task_executor>               ul_exec, ctrl_exec;
  std::vector<std::unique_ptr<task_executor> > dl_execs;
};

} // namespace srsgnb

#endif // SRSGNB_DU_H
