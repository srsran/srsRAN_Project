/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_generic_impl.h"

using namespace srsran;

ru_generic_impl::ru_generic_impl(ru_generic_impl_config&& config) :
  phy_err_printer(std::move(config.phy_err_printer)),
  phy_metric_printer(std::move(config.phy_metrics_printer)),
  ru_rx_adapter(std::move(config.ru_rx_adapter)),
  ru_time_adapter(std::move(config.ru_time_adapter)),
  radio(std::move(config.radio)),
  low_phy(std::move(config.low_phy)),
  ru_ctrl(
      [](span<std::unique_ptr<lower_phy>> sectors) {
        std::vector<lower_phy_controller*> out;
        for (auto& sector : sectors) {
          out.push_back(&sector->get_controller());
          srsran_assert(out.back(), "Invalid lower PHY controller");
        }
        return out;
      }(low_phy),
      [](span<std::unique_ptr<phy_metrics_adapter>> metric_printers) {
        std::vector<phy_metrics_adapter*> out;
        for (auto& metric_printer : metric_printers) {
          out.push_back(metric_printer.get());
          srsran_assert(out.back(), "Invalid lower PHY metric printer");
        }
        return out;
      }(phy_metric_printer),
      [](span<std::unique_ptr<lower_phy>> sectors) {
        std::vector<lower_phy_cfo_controller*> out;
        for (auto& sector : sectors) {
          out.push_back(&sector->get_tx_cfo_control());
          srsran_assert(out.back(), "Invalid lower PHY controller");
        }
        return out;
      }(low_phy),
      [](span<std::unique_ptr<lower_phy>> sectors) {
        std::vector<lower_phy_cfo_controller*> out;
        for (auto& sector : sectors) {
          out.push_back(&sector->get_rx_cfo_control());
          srsran_assert(out.back(), "Invalid lower PHY controller");
        }
        return out;
      }(low_phy),
      *radio,
      config.srate_MHz),
  ru_downlink_hdlr([](span<std::unique_ptr<lower_phy>> sectors) {
    std::vector<lower_phy_rg_handler*> out;
    for (auto& sector : sectors) {
      out.push_back(&sector->get_rg_handler());
      srsran_assert(out.back(), "Invalid lower PHY resource grid handler");
    }
    return out;
  }(low_phy)),
  ru_uplink_request_hdlr([](span<std::unique_ptr<lower_phy>> sectors) {
    std::vector<lower_phy_request_handler*> out;
    for (auto& sector : sectors) {
      out.push_back(&sector->get_request_handler());
      srsran_assert(out.back(), "Invalid lower PHY request handler");
    }
    return out;
  }(low_phy))
{
  srsran_assert(ru_rx_adapter, "Invalid Radio Unit receiver adapter");
  srsran_assert(radio, "Invalid radio session");
}

ru_controller& ru_generic_impl::get_controller()
{
  return ru_ctrl;
}

ru_downlink_plane_handler& ru_generic_impl::get_downlink_plane_handler()
{
  return ru_downlink_hdlr;
}

ru_uplink_plane_handler& ru_generic_impl::get_uplink_plane_handler()
{
  return ru_uplink_request_hdlr;
}
