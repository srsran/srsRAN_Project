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

#include "upper_phy_impl.h"
#include "upper_phy_rx_symbol_handler_printer_decorator.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"

using namespace srsran;

namespace {

/// Dummy implementation of an upper PHY timing notifier.
class upper_phy_timing_notifier_dummy : public upper_phy_timing_notifier
{
public:
  void on_tti_boundary(slot_point slot) override {}
};

} // namespace

static upper_phy_timing_notifier_dummy notifier_dummy;

upper_phy_impl::upper_phy_impl(upper_phy_impl_config&& config) :
  logger(srslog::fetch_basic_logger("Upper PHY", true)),
  sector_id(config.sector_id),
  rx_buf_pool(std::move(config.rx_buf_pool)),
  dl_rg_pool(std::move(config.dl_rg_pool)),
  ul_rg_pool(std::move(config.ul_rg_pool)),
  pdu_repository(config.nof_slots_ul_pdu_repository),
  prach_pool(std::move(config.prach_pool)),
  dl_processor_pool(std::move(config.dl_processor_pool)),
  ul_processor_pool(std::move(config.ul_processor_pool)),
  dl_pdu_validator(std::move(config.dl_pdu_validator)),
  ul_pdu_validator(std::move(config.ul_pdu_validator)),
  ul_request_processor(*config.rx_symbol_request_notifier, *prach_pool),
  rx_symbol_handler(std::make_unique<upper_phy_rx_symbol_handler_impl>(*ul_processor_pool,
                                                                       pdu_repository,
                                                                       rx_buf_pool->get_pool(),
                                                                       rx_results_notifier,
                                                                       logger)),
  timing_handler(notifier_dummy)
{
  srsran_assert(dl_processor_pool, "Invalid downlink processor pool");
  srsran_assert(dl_rg_pool, "Invalid downlink resource grid pool");
  srsran_assert(ul_rg_pool, "Invalid uplink resource grid pool");
  srsran_assert(ul_processor_pool, "Invalid uplink processor pool");
  srsran_assert(prach_pool, "Invalid PRACH buffer pool");
  srsran_assert(rx_buf_pool, "Invalid receive buffer pool");
  srsran_assert(dl_pdu_validator, "Invalid downlink PDU validator");
  srsran_assert(ul_pdu_validator, "Invalid uplink PDU validator");

  logger.set_level(config.log_level);

  // Configure RX symbol handler for printing the resource grid.
  if (!config.rx_symbol_printer_filename.empty()) {
    interval<unsigned> ul_ports(0, config.nof_rx_ports);
    if (config.rx_symbol_printer_port.has_value()) {
      ul_ports.set(config.rx_symbol_printer_port.value(), config.rx_symbol_printer_port.value() + 1);
    }
    rx_symbol_handler =
        std::make_unique<upper_phy_rx_symbol_handler_printer_decorator>(std::move(rx_symbol_handler),
                                                                        logger,
                                                                        config.rx_symbol_printer_filename,
                                                                        config.ul_bw_rb,
                                                                        ul_ports,
                                                                        config.rx_symbol_printer_prach);
  }

  // :TODO: Add a logger here.
  (void)sector_id;
}

upper_phy_error_handler& upper_phy_impl::get_error_handler()
{
  return error_handler;
}

upper_phy_rx_symbol_handler& upper_phy_impl::get_rx_symbol_handler()
{
  return *rx_symbol_handler;
}

upper_phy_timing_handler& upper_phy_impl::get_timing_handler()
{
  return timing_handler;
}

downlink_processor_pool& upper_phy_impl::get_downlink_processor_pool()
{
  return *dl_processor_pool;
}

resource_grid_pool& upper_phy_impl::get_downlink_resource_grid_pool()
{
  return *dl_rg_pool;
}

resource_grid_pool& upper_phy_impl::get_uplink_resource_grid_pool()
{
  return *ul_rg_pool;
}

uplink_request_processor& upper_phy_impl::get_uplink_request_processor()
{
  return ul_request_processor;
}

uplink_slot_pdu_repository& upper_phy_impl::get_uplink_slot_pdu_repository()
{
  return pdu_repository;
}

void upper_phy_impl::set_error_notifier(upper_phy_error_notifier& notifier)
{
  error_handler.set_error_notifier(notifier);
}

void upper_phy_impl::set_timing_notifier(srsran::upper_phy_timing_notifier& notifier)
{
  timing_handler.set_upper_phy_notifier(notifier);
}

void upper_phy_impl::set_rx_results_notifier(upper_phy_rx_results_notifier& notifier)
{
  rx_results_notifier.connect(notifier);
}

const uplink_pdu_validator& upper_phy_impl::get_uplink_pdu_validator() const
{
  return *ul_pdu_validator;
}

const downlink_pdu_validator& upper_phy_impl::get_downlink_pdu_validator() const
{
  return *dl_pdu_validator;
}

void upper_phy_impl::stop()
{
  rx_buf_pool->stop();
}
