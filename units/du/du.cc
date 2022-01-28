
#include "du.h"
#include "adapters.h"
#include "srsgnb/du_manager/du_manager_factory.h"
#include "srsgnb/f1ap/f1ap_du_factory.h"
#include "srsgnb/mac/mac_factory.h"
#include "srsgnb/rlc/rlc_factory.h"

namespace srsgnb {

du::du()
{
  const size_t task_worker_queue_size = 10000;

  // Create worker threads
  workers.push_back(std::make_unique<task_worker>("DU-CTRL", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-UL", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-DL", task_worker_queue_size, true));
  ctrl_exec = make_task_executor(*workers[0]);
  ul_exec   = make_task_executor(*workers[1]);
  dl_exec   = make_task_executor(*workers[2]);

  // Create layers
  rlc        = create_rlc(rlc_cfg_notifier);
  mac        = create_mac(mac_cfg_notifier, mac_ul_sdu_notifier);
  du_manager = create_du_manager(*mac, *rlc, f1ap_cfg_notifier, *ctrl_exec);
  f1ap       = create_f1ap_du(f1ap_pdu_adapter, *du_manager);

  // Connect DU blocks
  mac_cfg_notifier.connect(*du_manager);
  mac_ul_sdu_notifier.connect(*f1ap);
  rlc_cfg_notifier.connect(*du_manager);
  f1ap_cfg_notifier.connect(*f1ap);
}

du::~du()
{
  stop();
}

void du::start()
{
  for (auto& w : workers) {
    w->start();
  }
}

void du::stop()
{
  for (auto& w : workers) {
    w->stop();
  }
  workers.clear();
}

void du::push_pusch(rnti_t rnti, lcid_t lcid, byte_buffer pdu)
{
  ul_exec->execute([this, rnti, lcid, pdu]() { mac->push_ul_pdu(rnti, lcid, std::move(pdu)); });
}

std::string du::query(const std::string& s)
{
  if (s == "ues") {
    return du_manager->get_ues();
  }
  return "";
}

} // namespace srsgnb
