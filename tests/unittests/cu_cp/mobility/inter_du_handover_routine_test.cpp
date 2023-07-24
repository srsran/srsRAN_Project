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

// #include "du_processor_test_helpers.h"
#include "inter_du_handover_routine_test_helpers.h"
#include "lib/cu_cp/routine_managers/du_processor_routine_manager.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "mobility_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class inter_du_handover_routine_test : public mobility_test
{
protected:
  inter_du_handover_routine_test() {}

  void create_dus_and_attach_ue()
  {
    du_processor_config_t du_cfg;
    du_cfg.du_index = source_du_index;
    source_du       = create_du(du_cfg);
    ASSERT_NE(source_du, nullptr);

    attach_du_to_cu(*source_du, source_du_id, source_nrcell_id, source_pci);

    du_cfg.du_index = target_du_index;
    target_du       = create_du(du_cfg);
    ASSERT_NE(target_du, nullptr);

    attach_du_to_cu(*target_du, target_du_id, target_nrcell_id, source_pci);

    source_ue_index = attach_ue(*source_du, source_nrcell_id).ue_index;
    ASSERT_NE(source_ue_index, ue_index_t::invalid);
  }

  void start_procedure(const cu_cp_inter_du_handover_request& msg)
  {
    t = source_du->du_processor_obj->handle_inter_du_handover_request(
        msg, target_du->du_processor_obj->get_du_processor_f1ap_ue_context_notifier());
    t_launcher.emplace(t);
  }

  bool procedure_ready() const { return t.ready(); }

  const cu_cp_inter_du_handover_response& get_result() { return t.get(); }

  ue_index_t get_source_ue() { return source_ue_index; }

  unsigned get_target_pci() { return target_pci; }

  du_index_t get_target_du_index() { return target_du_index; }

private:
  // source DU parameters.
  du_index_t source_du_index  = uint_to_du_index(1);
  unsigned   source_du_id     = 0x11;
  unsigned   source_nrcell_id = 411;
  unsigned   source_pci       = 1;

  // target DU parameters.
  du_index_t target_du_index  = uint_to_du_index(2);
  unsigned   target_du_id     = 0x22;
  unsigned   target_nrcell_id = 0x11;
  unsigned   target_pci       = 2;

  // Handler to DU objects.
  du_wrapper* source_du = nullptr;
  du_wrapper* target_du = nullptr;

  ue_index_t source_ue_index = ue_index_t::invalid;

  async_task<cu_cp_inter_du_handover_response>                   t;
  optional<lazy_task_launcher<cu_cp_inter_du_handover_response>> t_launcher;
};

TEST_F(inter_du_handover_routine_test, when_invalid_pci_is_used_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.neighbor_pci                    = INVALID_PCI;
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);
}

TEST_F(inter_du_handover_routine_test, when_ue_context_setup_fails_ho_fails)
{
  create_dus_and_attach_ue();

  // Test Preamble.
  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.neighbor_pci                    = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);
}