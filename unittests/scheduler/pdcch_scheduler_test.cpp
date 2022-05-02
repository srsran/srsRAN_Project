
#include "../../lib/scheduler/cell/pdcch_scheduler.h"
#include "config_generators.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_pdcch_sched_sib1()
{
  test_delimit_logger delimiter{"Test PDCCH Allocation in CORESET#0"};

  sched_cell_configuration_request_message msg =
      make_scheduler_cell_configuration_request(test_helpers::make_default_mac_cell_creation_request());
  cell_configuration cfg{msg};
  cell_resource_grid res_grid{cfg};

  pdcch_scheduler pdcch_sch(res_grid);
  TESTASSERT_EQ(0, pdcch_sch.nof_dcis());

  slot_point sl_tx{0, 0};

  pdcch_sch.slot_indication(sl_tx);

  // SIB1 allocation should be successful.
  pdcch_sch.alloc_dl_pdcch(sl_tx);
}

int main()
{
  test_pdcch_sched_sib1();
}