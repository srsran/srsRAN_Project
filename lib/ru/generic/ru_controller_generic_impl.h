/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/adapters/phy_metrics_adapter.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/lower_phy_metrics_notifier.h"
#include "srsran/ru/ru_controller.h"
#include <vector>

namespace srsran {

class lower_phy_controller;
class lower_phy_metrics_notifier;
class radio_session;
class lower_phy_cfo_controller;

/// Radio Unit controller generic implementation.
class ru_controller_generic_impl : public ru_controller
{
public:
  ru_controller_generic_impl(std::vector<lower_phy_controller*>     low_phy_crtl_,
                             std::vector<phy_metrics_adapter*>      low_phy_metrics_,
                             std::vector<lower_phy_cfo_controller*> tx_cfo_control_,
                             std::vector<lower_phy_cfo_controller*> rx_cfo_control_,
                             radio_session&                         radio_,
                             double                                 srate_MHz_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override;

  // See interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override;

  // See interface for documentation.
  bool set_tx_cfo(unsigned port_id, float cfo_hz) override;

  // See interface for documentation.
  bool set_rx_cfo(unsigned port_id, float cfo_hz) override;

  // See interface for documentation.
  void print_metrics() override;

private:
  std::vector<lower_phy_controller*>     low_phy_crtl;
  std::vector<phy_metrics_adapter*>      low_phy_metrics;
  std::vector<lower_phy_cfo_controller*> tx_cfo_control;
  std::vector<lower_phy_cfo_controller*> rx_cfo_control;
  radio_session&                         radio;
  const double                           srate_MHz;
};

} // namespace srsran
