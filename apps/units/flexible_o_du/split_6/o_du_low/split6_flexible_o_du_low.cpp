/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_flexible_o_du_low.h"
#include "srsran/fapi_adaptor/mac/operation_controller.h"

using namespace srsran;

split6_flexible_o_du_low::split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies dependencies) :
  odu_low_session_manager(std::move(dependencies.odu_low_session_factory)),
  mac_p5_adaptor(std::move(dependencies.mac_p5_adaptor)),
  phy_p5_adaptor(std::move(dependencies.phy_p5_adaptor))
{
  srsran_assert(mac_p5_adaptor, "Invalid MAC-FAPI P5 sector adaptor");
  srsran_assert(phy_p5_adaptor, "Invalid PHY-FAPI P5 sector adaptor");

  // Connect MAC-PHY FAPI P5 adaptors.
  phy_p5_adaptor->set_cell_operation_request_notifier(odu_low_session_manager);
  phy_p5_adaptor->set_error_message_notifier(mac_p5_adaptor->get_error_message_notifier());
  phy_p5_adaptor->set_config_message_notifier(mac_p5_adaptor->get_config_message_notifier());
}

void split6_flexible_o_du_low::start()
{
  mac_p5_adaptor->get_operation_controller().start();
}

void split6_flexible_o_du_low::stop()
{
  mac_p5_adaptor->get_operation_controller().stop();

  // Clear running session.
  odu_low_session_manager.on_stop_request();
}
