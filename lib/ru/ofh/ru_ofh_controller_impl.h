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

#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

/// \brief RU controller implementation for the Open Fronthaul interface.
///
/// Manages the timing controller that is common to all sectors and the individual controller of each sector.
class ru_ofh_controller_impl : public ru_controller
{
public:
  ru_ofh_controller_impl(srslog::basic_logger& logger_, std::vector<ofh::controller*> sector_controllers_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override { return false; }

  // See interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override { return false; }

  // See interface for documentation.
  bool set_tx_cfo(unsigned port_id, float cfo_Hz) override { return false; }

  // See interface for documentation.
  bool set_rx_cfo(unsigned port_id, float cfo_Hz) override { return false; }

  // See interface for documentation.
  void print_metrics() override;

private:
  srslog::basic_logger&         logger;
  std::vector<ofh::controller*> sector_controllers;
};

} // namespace srsran
