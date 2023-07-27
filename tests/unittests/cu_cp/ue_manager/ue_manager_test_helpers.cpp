/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

ue_manager_test::ue_manager_test() : ue_mng(ue_config)
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

ue_index_t ue_manager_test::create_ue(du_index_t du_index, pci_t pci, rnti_t rnti)
{
  ue_index_t ue_index = ue_mng.allocate_new_ue_index(du_index);
  auto*      ue       = ue_mng.add_ue(ue_index, pci, rnti);
  if (ue == nullptr) {
    test_logger.error("Failed to create UE with pci={} and rnti={}", pci, rnti);
    return ue_index_t::invalid;
  }

  return ue->get_ue_index();
}
