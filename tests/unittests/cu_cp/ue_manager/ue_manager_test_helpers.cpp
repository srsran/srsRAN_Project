/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager_test_helpers.h"
#include <gtest/gtest.h>
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

ue_manager_test::ue_manager_test() : ue_mng(ue_config, up_config, sec_config, timers, cu_worker)
{
  test_logger.set_level(srslog::basic_levels::debug);
  ue_mng_logger.set_level(srslog::basic_levels::debug);
  srslog::init();
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
