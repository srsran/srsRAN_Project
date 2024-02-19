/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "lib/cu_cp/cu_cp_impl.h"
#include "test_helpers.h"
#include "tests/unittests/e1ap/common/test_helpers.h"
#include "tests/unittests/e1ap/cu_cp/e1ap_cu_cp_test_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for CU-CP test
class cu_cp_test : public ::testing::Test
{
protected:
  cu_cp_test();
  ~cu_cp_test() override;

  void test_amf_connection();

  void test_e1ap_attach();
  void test_du_attach(du_index_t du_index, gnb_du_id_t gnb_du_id, unsigned nrcell_id, pci_t pci);

  void attach_ue(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id, rnti_t crnti, du_index_t du_index);
  void authenticate_ue(amf_ue_id_t         amf_ue_id,
                       ran_ue_id_t         ran_ue_id,
                       du_index_t          du_index,
                       gnb_du_ue_f1ap_id_t du_ue_id,
                       gnb_cu_ue_f1ap_id_t cu_ue_id);
  void setup_security(amf_ue_id_t         amf_ue_id,
                      ran_ue_id_t         ran_ue_id,
                      du_index_t          du_index,
                      gnb_du_ue_f1ap_id_t du_ue_id,
                      gnb_cu_ue_f1ap_id_t cu_ue_id);
  void test_preamble_all_connected(du_index_t du_index, pci_t pci);
  void test_preamble_ue_creation(du_index_t          du_index,
                                 gnb_du_ue_f1ap_id_t du_ue_id,
                                 gnb_cu_ue_f1ap_id_t cu_ue_id,
                                 pci_t               pci,
                                 rnti_t              crnti,
                                 amf_ue_id_t         amf_ue_id,
                                 ran_ue_id_t         ran_ue_id);
  void test_preamble_ue_full_attach(du_index_t             du_index,
                                    gnb_du_ue_f1ap_id_t    du_ue_id,
                                    gnb_cu_ue_f1ap_id_t    cu_ue_id,
                                    pci_t                  pci,
                                    rnti_t                 crnti,
                                    amf_ue_id_t            amf_ue_id,
                                    ran_ue_id_t            ran_ue_id,
                                    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                    gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);
  bool check_minimal_paging_result();
  bool check_paging_result();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  dummy_ngap_amf_notifier        ngap_amf_notifier;
  std::unique_ptr<timer_manager> timers = std::make_unique<timer_manager>(256);
  manual_task_worker             ctrl_worker{128};

  std::unique_ptr<ngap_message_handler> dummy_amf;

  std::unique_ptr<cu_cp_impl> cu_cp_obj;

  dummy_cu_cp_f1c_gateway  f1c_gw;
  dummy_cu_cp_e1ap_gateway e1ap_gw;
};

} // namespace srs_cu_cp
} // namespace srsran
