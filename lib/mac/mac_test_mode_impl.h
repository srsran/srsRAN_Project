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

#include "mac_impl.h"

namespace srsran {

class mac_test_mode_impl : public mac_impl
{
public:
  explicit mac_test_mode_impl(const mac_config& mac_cfg);
  ~mac_test_mode_impl() override;

  mac_ue_configurator& get_ue_configurator() override;

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override;

  void handle_dl_buffer_state_update_required(const mac_dl_buffer_state_indication_message& dl_bs) override;

private:
  struct test_ue_adapter {
    std::unique_ptr<mac_ue_configurator>                               ue_configurator;
    std::vector<std::unique_ptr<mac_cell_control_information_handler>> cell_info_handler;
  };

  srs_du::du_test_config::test_ue_config test_ue;

  test_ue_adapter adapter;

  du_ue_index_t test_ue_index = INVALID_DU_UE_INDEX;
};

} // namespace srsran