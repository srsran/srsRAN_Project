/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "pdu_session_manager_test.h"
#include "cu_up_test_helpers.h"
#include "lib/cu_up/pdu_session_manager_impl.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, when_valid_pdu_session_setup_item_session_can_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  pdu_session_id_t psi    = uint_to_pdu_session_id(1);
  drb_id_t         drb_id = uint_to_drb_id(1);
  qos_flow_id_t    qfi    = uint_to_qos_flow_id(8);

  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item = generate_pdu_session_res_to_setup_item(psi, drb_id, qfi);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.gtp_tunnel.gtp_teid.value(), 1);
  ASSERT_EQ(setup_result.drb_setup_results[0].gtp_tunnel.gtp_teid.value(), 0);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to remove non-existing session
  pdu_session_mng->remove_pdu_session(uint_to_pdu_session_id(2));

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to remove existing session
  pdu_session_mng->remove_pdu_session(uint_to_pdu_session_id(1));

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);
}

TEST_F(pdu_session_manager_test, when_pdu_session_with_same_id_is_setup_session_cant_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  pdu_session_id_t psi    = uint_to_pdu_session_id(1);
  drb_id_t         drb_id = uint_to_drb_id(1);
  qos_flow_id_t    qfi    = uint_to_qos_flow_id(8);

  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item = generate_pdu_session_res_to_setup_item(psi, drb_id, qfi);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to add the same session again
  setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check unsuccessful outcome
  ASSERT_FALSE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
}

TEST_F(pdu_session_manager_test, when_unexisting_pdu_session_is_modified_operation_failed)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // attempt to modify unexisting PDU session
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item;
  pdu_session_modify_item.pdu_session_id = uint_to_pdu_session_id(1);

  pdu_session_modification_result modification_result =
      pdu_session_mng->modify_pdu_session(pdu_session_modify_item, false);

  // check successful outcome
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);
}

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, drb_create_modify_remove)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  pdu_session_id_t psi    = uint_to_pdu_session_id(1);
  drb_id_t         drb_id = uint_to_drb_id(1);
  qos_flow_id_t    qfi    = uint_to_qos_flow_id(8);

  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item = generate_pdu_session_res_to_setup_item(psi, drb_id, qfi);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.pdu_session_id, psi);
  ASSERT_EQ(setup_result.drb_setup_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->drb_id, drb_id);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.begin()->qos_flow_id, qfi);

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_FALSE(gtpu_rx_demux->created_teid_list.empty());
  gtpu_rx_demux->created_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->created_teid_list.empty());

  ASSERT_FALSE(f1u_gw->created_ul_teid_list.empty());
  gtpu_teid_t ul_teid = f1u_gw->created_ul_teid_list.front();
  f1u_gw->created_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->created_ul_teid_list.empty());

  // prepare modification request (to remove bearers)
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item =
      generate_pdu_session_res_to_modify_item_to_remove_drb(psi, drb_id);

  // Add invalid DRB to remove
  drb_id_t invalid_drb_to_remove = uint_to_drb_id(0x0f);
  pdu_session_modify_item.drb_to_rem_list_ng_ran.push_back(invalid_drb_to_remove);

  // attempt to remove bearers
  pdu_session_modification_result modification_result =
      pdu_session_mng->modify_pdu_session(pdu_session_modify_item, false);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);

  // validate pdu session is not disconnected from GTP-U gateway
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_TRUE(gtpu_rx_demux->removed_teid_list.empty());

  // validate bearer is disconnected from F1-U gateway
  ASSERT_FALSE(f1u_gw->removed_ul_teid_list.empty());
  ASSERT_EQ(f1u_gw->removed_ul_teid_list.front(), ul_teid);
  f1u_gw->removed_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->removed_ul_teid_list.empty());
}

TEST_F(pdu_session_manager_test, dtor_rm_all_sessions_and_bearers)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  pdu_session_id_t psi    = uint_to_pdu_session_id(1);
  drb_id_t         drb_id = uint_to_drb_id(1);
  qos_flow_id_t    qfi    = uint_to_qos_flow_id(8);

  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item = generate_pdu_session_res_to_setup_item(psi, drb_id, qfi);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.pdu_session_id, psi);
  ASSERT_EQ(setup_result.drb_setup_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->drb_id, drb_id);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.begin()->qos_flow_id, qfi);

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_FALSE(gtpu_rx_demux->created_teid_list.empty());
  gtpu_teid_t teid = gtpu_rx_demux->created_teid_list.front();
  gtpu_rx_demux->created_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->created_teid_list.empty());

  ASSERT_FALSE(f1u_gw->created_ul_teid_list.empty());
  gtpu_teid_t ul_teid = f1u_gw->created_ul_teid_list.front();
  f1u_gw->created_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->created_ul_teid_list.empty());

  // delete pdu_session_mng, all remaining sessions and bearers shall be removed and detached from all gateways
  pdu_session_mng.reset();

  // validate pdu session is disconnected from GTP-U gateway
  ASSERT_FALSE(gtpu_rx_demux->removed_teid_list.empty());
  ASSERT_EQ(gtpu_rx_demux->removed_teid_list.front(), teid);
  gtpu_rx_demux->removed_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->removed_teid_list.empty());

  // validate bearer is disconnected from F1-U gateway
  ASSERT_FALSE(f1u_gw->removed_ul_teid_list.empty());
  ASSERT_EQ(f1u_gw->removed_ul_teid_list.front(), ul_teid);
  f1u_gw->removed_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->removed_ul_teid_list.empty());
}

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, when_new_ul_info_is_requested_f1u_is_disconnected)
{
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  pdu_session_id_t psi    = uint_to_pdu_session_id(1);
  drb_id_t         drb_id = uint_to_drb_id(1);
  qos_flow_id_t    qfi    = uint_to_qos_flow_id(8);

  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item = generate_pdu_session_res_to_setup_item(psi, drb_id, qfi);

  pdu_session_setup_result set_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  drb_setup_result drb_setup_res = set_result.drb_setup_results[0];
  ASSERT_EQ(drb_setup_res.gtp_tunnel.gtp_teid, 0x0);

  // prepare modification request (request new UL TNL info)
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item =
      generate_pdu_session_res_to_modify_item_to_modify_drb(psi, drb_id);

  pdu_session_modification_result mod_result  = pdu_session_mng->modify_pdu_session(pdu_session_modify_item, true);
  drb_setup_result                drb_mod_res = mod_result.drb_modification_results[0];
  ASSERT_EQ(drb_mod_res.gtp_tunnel.gtp_teid, 0x1);

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
}
