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

#pragma once

#include "../test_helpers.h"
#include "lib/cu_cp/ue_manager_impl.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"

#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for UE manager creation
class ue_manager_test : public ::testing::Test
{
protected:
  ue_manager_test();
  ~ue_manager_test() override;

  ue_index_t create_ue(du_index_t du_index, pci_t pci, rnti_t rnti);

  srslog::basic_logger& test_logger   = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& ue_mng_logger = srslog::fetch_basic_logger("CU-UEMNG");

  ue_configuration        ue_config;
  up_resource_manager_cfg up_config;
  ue_manager              ue_mng;

  // DU processor to RRC UE adapters
  slotted_id_vector<ue_index_t, dummy_du_processor_rrc_ue_control_message_notifier> rrc_ue_adapters;
  dummy_ngap_rrc_ue_notifier                                                        rrc_ue_pdu_notifier;
  dummy_ngap_du_processor_notifier                                                  du_processor_ctrl_notifier;
};

} // namespace srs_cu_cp
} // namespace srsran
