
#include "adapters.h"
#include "du_high.h"
#include "srsgnb/du_manager/du_manager_factory.h"
#include "srsgnb/f1ap/f1ap_du_factory.h"
#include "srsgnb/mac/mac_factory.h"
#include "srsgnb/rlc/rlc_factory.h"

namespace srsgnb {

du_high::du_high()
{
  const size_t task_worker_queue_size = 10000;

  // Create worker threads
  workers.push_back(std::make_unique<task_worker>("DU-CTRL", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-UL", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-DL", task_worker_queue_size, true));
  ctrl_exec = make_task_executor(*workers[0]);
  ul_exec   = make_task_executor(*workers[1]);
  dl_execs.push_back(make_task_executor(*workers[2]));

  // Create layers
  std::vector<task_executor*> execs;
  for (auto& w : dl_execs) {
    execs.push_back(w.get());
  }
  mac        = create_mac(mac_cfg_notifier, mac_ul_ccch_notifier, *ul_exec, execs, *ctrl_exec);
  du_manager = create_du_manager(*mac, f1ap_cfg_notifier, rlc_sdu_notifier, *ctrl_exec);
  f1ap       = create_f1ap_du(f1ap_pdu_adapter, *du_manager);

  // Connect DU blocks
  mac_cfg_notifier.connect(*du_manager);
  mac_ul_ccch_notifier.connect(*f1ap);
  rlc_cfg_notifier.connect(*du_manager);
  f1ap_cfg_notifier.connect(*f1ap);
}

du_high::~du_high()
{
  stop();
}

void du_high::start()
{
  for (auto& w : workers) {
    w->start();
  }
}

void du_high::stop()
{
  for (auto& w : workers) {
    w->stop();
  }
  workers.clear();
}

void du_high::push_pusch(mac_rx_data_indication pdu)
{
  ul_exec->execute([this, pdu]() { mac->push_ul_pdu(std::move(pdu)); });
}

std::string du_high::query(const std::string& s)
{
  if (s == "ues") {
    return du_manager->get_ues();
  }
  return "";
}

} // namespace srsgnb
