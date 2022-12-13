/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_procedure_test_helpers.h"
#include "lib/du_manager/procedures/ue_creation_procedure.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

du_ue& du_manager_proc_tester::create_ue(du_ue_index_t ue_index)
{
  ul_ccch_indication_message ul_ccch_msg;
  ul_ccch_msg.cell_index = to_du_cell_index(0);
  ul_ccch_msg.crnti      = to_rnti(0x4601);
  ul_ccch_msg.slot_rx    = {0, test_rgen::uniform_int<unsigned>(0, 10240)};
  ul_ccch_msg.subpdu     = test_rgen::random_vector<uint8_t>(6);

  f1ap.next_ue_create_response.result = true;
  f1ap.next_ue_create_response.f1c_bearers_added.resize(2);
  f1ap.next_ue_create_response.f1c_bearers_added[0] = nullptr;
  f1ap.next_ue_create_response.f1c_bearers_added[1] = nullptr;
  mac.wait_ue_create.result.result                  = true;
  mac.wait_ue_create.result.ue_index                = ue_index;
  mac.wait_ue_create.result.cell_index              = ul_ccch_msg.cell_index;
  mac.wait_ue_create.ready_ev.set();

  async_task<void> t = launch_async<ue_creation_procedure>(
      ue_index, ul_ccch_msg, ue_mng, params.services, params.mac, params.rlc, params.f1ap);

  lazy_task_launcher<void> launcher{t};

  srsgnb_assert(launcher.ready(), "The UE creation procedure should have completed by now");

  return *ue_mng.ues[ue_index];
}
