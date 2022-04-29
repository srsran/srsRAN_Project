
#include "config_generators.h"
#include "lib/scheduler/sched.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

void test_no_ues()
{
  sched_cfg_dummy_notifier cfg_notif;
  sched                    sch{cfg_notif};

  // Action 1: Add Cell.
  cell_configuration_request_message cell_cfg_msg = make_cell_cfg_req();
  sch.handle_cell_configuration_request(cell_cfg_msg);

  slot_point sl_tx{0, 0};

  // Action 2: Run slot 0.
  const dl_sched_result* res = sch.get_dl_sched(sl_tx, to_du_cell_index(0));
  TESTASSERT(res != nullptr);
  TESTASSERT(res->ue_grants.empty());
}

void test_rach_indication()
{
  sched_cfg_dummy_notifier cfg_notif;
  sched                    sch{cfg_notif};

  // Action 1: Add Cell.
  cell_configuration_request_message cell_cfg_msg = make_cell_cfg_req();
  sch.handle_cell_configuration_request(make_cell_cfg_req());

  const unsigned tx_delay = 2;
  slot_point     sl_rx{0, 0}, sl_tx = sl_rx + tx_delay;

  // Action 2: Add RACH indication.
  sch.handle_rach_indication(generate_rach_ind_msg(sl_rx, to_rnti(0x4601)));

  // Action 3: Run slot 0.
  const dl_sched_result* res = sch.get_dl_sched(sl_tx, to_du_cell_index(0));

  // TEST: Result exists. No Data allocated. A RAR has been allocated.
  TESTASSERT(res != nullptr);
  TESTASSERT(res->ue_grants.empty());
  TESTASSERT(not res->rar_grants.empty());
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_no_ues();
  test_rach_indication();
}
