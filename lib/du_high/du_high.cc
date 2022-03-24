
#include "du_high.h"
#include "adapters.h"
#include "du_high_executor_strategies.h"
#include "srsgnb/du_manager/du_manager_factory.h"
#include "srsgnb/f1_interface/f1ap_du_factory.h"
#include "srsgnb/mac/mac_factory.h"

namespace srsgnb {

du_high::du_high(f1c_du_gateway& f1_gw)
{
  const size_t task_worker_queue_size = 10000;

  // Create worker threads
  workers.push_back(std::make_unique<task_worker>("DU-CTRL", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-UL#0", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-UL#1", task_worker_queue_size, true));
  workers.push_back(std::make_unique<task_worker>("DU-DL", task_worker_queue_size, true));
  ctrl_exec = make_task_executor(*workers[0]);
  dl_execs.push_back(make_task_executor(*workers[2]));

  // Setup UL UE executor mapper.
  std::vector<std::unique_ptr<task_executor> > ul_execs;
  ul_execs.push_back(make_task_executor(*workers[1]));
  ul_execs.push_back(make_task_executor(*workers[2]));
  ul_exec_mapper = std::make_unique<pcell_ul_executor_mapper>(std::move(ul_execs));

  // Create layers
  std::vector<task_executor*> execs;
  execs.clear();
  for (auto& w : dl_execs) {
    execs.push_back(w.get());
  }
  mac        = create_mac(mac_ev_notifier, *ul_exec_mapper, execs, *ctrl_exec);
  f1ap       = create_f1ap_du(f1ap_pdu_adapter, f1_gw);
  du_manager = create_du_manager(*mac, *f1ap, *f1ap, rlc_sdu_notifier, *ctrl_exec);

  // Connect Layer->DU manager notifiers.
  mac_ev_notifier.connect(*du_manager);
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
  mac->handle_rx_data_indication(std::move(pdu));
}

} // namespace srsgnb
