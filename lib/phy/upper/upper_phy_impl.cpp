/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_impl.h"
#include "upper_phy_rx_symbol_handler_printer_decorator.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"

using namespace srsgnb;

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
  dl_processor_pool(std::move(config.dl_processor_pool)),
  dl_rg_pool(std::move(config.dl_rg_pool)),
  ul_rg_pool(std::move(config.ul_rg_pool)),
  ul_processor_pool(std::move(config.ul_processor_pool)),
  prach_pool(std::move(config.prach_pool)),
  softbuffer_pool(std::move(config.softbuffer_pool)),
  dl_pdu_validator(std::move(config.dl_pdu_validator)),
  ul_pdu_validator(std::move(config.ul_pdu_validator)),
  ul_request_processor(*config.rx_symbol_request_notifier, *prach_pool),
  pdu_repository(config.nof_slots_ul_pdu_repository),
  rx_results_notifier(*softbuffer_pool),
  rx_symbol_handler(std::make_unique<upper_phy_rx_symbol_handler_impl>(*ul_processor_pool,
                                                                       pdu_repository,
                                                                       *softbuffer_pool,
                                                                       rx_results_notifier,
                                                                       logger)),
  timing_handler(notifier_dummy, *softbuffer_pool)
{
  srsgnb_assert(dl_processor_pool, "Invalid downlink processor pool");
  srsgnb_assert(dl_rg_pool, "Invalid downlink resource grid pool");
  srsgnb_assert(ul_rg_pool, "Invalid uplink resource grid pool");
  srsgnb_assert(ul_processor_pool, "Invalid uplink processor pool");
  srsgnb_assert(prach_pool, "Invalid PRACH buffer pool");
  srsgnb_assert(softbuffer_pool, "Invalid softbuffer pool");
  srsgnb_assert(dl_pdu_validator, "Invalid downlink PDU validator");
  srsgnb_assert(ul_pdu_validator, "Invalid uplink PDU validator");

  logger.set_level(config.log_level);

  // Configure RX symbol handler for printing the resource grid.
  if (!config.rx_symbol_printer_filename.empty()) {
    rx_symbol_handler = std::make_unique<upper_phy_rx_symbol_handler_printer_decorator>(
        std::move(rx_symbol_handler), logger, config.rx_symbol_printer_filename, config.ul_bw_rb);
  }

  // :TODO: Add a logger here.
  (void)sector_id;
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

void upper_phy_impl::set_timing_notifier(srsgnb::upper_phy_timing_notifier& notifier)
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
