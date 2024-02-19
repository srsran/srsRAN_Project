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

#include "ue_manager_test_helpers.h"
#include <gtest/gtest.h>
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

ue_manager_test::ue_manager_test() : ue_mng(ue_config, up_config, timers, cu_worker)
{
  test_logger.set_level(srslog::basic_levels::debug);
  ue_mng_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  du_processor_ctrl_notifier = std::make_unique<dummy_ngap_du_processor_notifier>(ngap_ue_removal_handler);
}

ue_manager_test::~ue_manager_test()
{
  // flush logger after each test
  srslog::flush();
}

ue_index_t ue_manager_test::create_ue(du_index_t du_index, gnb_du_id_t du_id, pci_t pci, rnti_t rnti)
{
  ue_index_t ue_index = ue_mng.add_ue(du_index);
  auto*      ue       = ue_mng.set_ue_du_context(ue_index, du_id, pci, rnti);
  if (ue == nullptr) {
    test_logger.error("Failed to create UE with pci={} and rnti={}", pci, rnti);
    return ue_index_t::invalid;
  }

  return ue->get_ue_index();
}
