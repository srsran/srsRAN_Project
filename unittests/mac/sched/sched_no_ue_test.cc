
#include "../../../lib/mac/sched/sched.h"
#include "config_generators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class sched_cfg_dummy_notifier : public sched_cfg_notifier
{
public:
  void on_ue_config_complete(rnti_t) override {}
  void on_ue_delete_response(rnti_t rnti) override {}
};

void test_no_ues()
{
  sched_cfg_dummy_notifier cfg_notif;
  sched                    sch{cfg_notif};

  cell_configuration_request_message cell_cfg_msg = make_cell_cfg_req();
  sch.handle_cell_configuration_request(make_cell_cfg_req());

  slot_point sl_tx{0, 0};

  // Action 1: Run slot 0
  const dl_sched_result* res = sch.get_dl_sched(sl_tx, 0);
  TESTASSERT(res != nullptr);
  TESTASSERT_EQ(cell_cfg_msg.pci, res->pci);
  TESTASSERT(res->ue_grants.empty());
}

int main()
{
  test_no_ues();
}
