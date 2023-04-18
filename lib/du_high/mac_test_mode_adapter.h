/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../mac/mac_impl.h"

namespace srsran {

class mac_test_mode_adapter final : public mac_interface
{
public:
  explicit mac_test_mode_adapter(std::unique_ptr<mac_interface>                mac_ptr,
                                 const srs_du::du_test_config::test_ue_config& test_ue_cfg);
  ~mac_test_mode_adapter() override;

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) override
  {
    return mac_ptr->get_rach_handler(cell_index);
  }

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override
  {
    return mac_ptr->get_slot_handler(cell_index);
  }

  mac_cell_manager& get_cell_manager() override { return mac_ptr->get_cell_manager(); }

  mac_ue_control_information_handler& get_ue_control_info_handler() override { return *ue_control_info_handler; }

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index) override { return mac_ptr->get_pdu_handler(cell_index); }

  mac_paging_information_handler& get_cell_paging_info_handler() override
  {
    return mac_ptr->get_cell_paging_info_handler();
  }

  mac_ue_configurator& get_ue_configurator() override { return *ue_configurator; }

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override;

private:
  srs_du::du_test_config::test_ue_config test_ue;

  std::unique_ptr<mac_interface> mac_ptr;

  std::unique_ptr<mac_ue_control_information_handler>                ue_control_info_handler;
  std::unique_ptr<mac_ue_configurator>                               ue_configurator;
  std::vector<std::unique_ptr<mac_cell_control_information_handler>> cell_info_handler;

  du_ue_index_t test_ue_index = INVALID_DU_UE_INDEX;
};

} // namespace srsran