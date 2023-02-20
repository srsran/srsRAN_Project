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

#include "uplink_request_processor_impl.h"
#include "upper_phy_pdu_validators.h"
#include "upper_phy_rx_results_notifier_wrapper.h"
#include "upper_phy_rx_symbol_handler_impl.h"
#include "srsgnb/phy/support/prach_buffer_pool.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/uplink_processor.h"
#include "srsgnb/phy/upper/upper_phy.h"
#include "srsgnb/phy/upper/upper_phy_timing_handler.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"

namespace srsgnb {

/// Upper PHY implementation configuration.
struct upper_phy_impl_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Uplink bandwidth in resource blocks.
  unsigned ul_bw_rb;
  /// Downlink processor pool.
  std::unique_ptr<downlink_processor_pool> dl_processor_pool;
  /// Downlink resource grid pool.
  std::unique_ptr<resource_grid_pool> dl_rg_pool;
  /// Uplink resource grid pool.
  std::unique_ptr<resource_grid_pool> ul_rg_pool;
  /// Uplink processor pool.
  std::unique_ptr<uplink_processor_pool> ul_processor_pool;
  /// PRACH buffer pool.
  std::unique_ptr<prach_buffer_pool> prach_pool;
  /// Softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> softbuffer_pool;
  /// Symbol request notifier.
  upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier;
  /// Log level.
  srslog::basic_levels log_level;
  /// Receive symbol printer. Leave empty to disable.
  std::string rx_symbol_printer_filename;
  /// Number of slots supported by the uplink PDU repository.
  size_t nof_slots_ul_pdu_repository;
  /// Downlink PDU validator.
  std::unique_ptr<downlink_pdu_validator> dl_pdu_validator;
  /// Uplink PDU validator.
  std::unique_ptr<uplink_pdu_validator> ul_pdu_validator;
};

/// \brief Implementation of the upper PHY interface.
///
/// Instances of this class will handle the ownership of the upper PHY components.
class upper_phy_impl : public upper_phy
{
  /// Upper PHY timing handler implementation class.
  class upper_phy_timing_handler_impl : public upper_phy_timing_handler
  {
    std::reference_wrapper<upper_phy_timing_notifier> notifier;
    rx_softbuffer_pool&                               softbuffer_pool;

  public:
    upper_phy_timing_handler_impl(upper_phy_timing_notifier& notifier_, rx_softbuffer_pool& softbuffer_pool_) :
      notifier(notifier_), softbuffer_pool(softbuffer_pool_)
    {
    }

    // See interface for documentation.
    void handle_tti_boundary(const upper_phy_timing_context& context) override
    {
      // Advance the timing in the softbuffer pool.
      softbuffer_pool.run_slot(context.slot);
      // Propagate the event.
      notifier.get().on_tti_boundary(context.slot);
    }

    // See interface for documentation.
    void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) override {}

    // See interface for documentation.
    void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) override {}

    void set_upper_phy_notifier(upper_phy_timing_notifier& n) { notifier = std::ref(n); }
  };

public:
  /// Constructs an upper PHY implementation object with the given configuration.
  explicit upper_phy_impl(upper_phy_impl_config&& config);

  // See interface for documentation.
  upper_phy_rx_symbol_handler& get_rx_symbol_handler() override;

  // See interface for documentation.
  upper_phy_timing_handler& get_timing_handler() override;

  // See interface for documentation.
  downlink_processor_pool& get_downlink_processor_pool() override;

  // See interface for documentation.
  resource_grid_pool& get_downlink_resource_grid_pool() override;

  // See interface for documentation.
  resource_grid_pool& get_uplink_resource_grid_pool() override;

  // See interface for documentation.
  uplink_request_processor& get_uplink_request_processor() override;

  // See interface for documentation.
  uplink_slot_pdu_repository& get_uplink_slot_pdu_repository() override;

  // See interface for documentation.
  const downlink_pdu_validator& get_downlink_pdu_validator() const override;

  // See interface for documentation.
  const uplink_pdu_validator& get_uplink_pdu_validator() const override;

  // See interface for documentation.
  void set_timing_notifier(upper_phy_timing_notifier& notifier) override;

  // See interface for documentation.
  void set_rx_results_notifier(upper_phy_rx_results_notifier& notifier) override;

private:
  /// Upper PHY logger.
  srslog::basic_logger& logger;
  /// Base station sector identifier.
  const unsigned sector_id;
  /// Downlink processor pool.
  std::unique_ptr<downlink_processor_pool> dl_processor_pool;
  /// Downlink resource grid pool.
  std::unique_ptr<resource_grid_pool> dl_rg_pool;
  /// Uplink resource grid pool.
  std::unique_ptr<resource_grid_pool> ul_rg_pool;
  /// Uplink processor pool.
  std::unique_ptr<uplink_processor_pool> ul_processor_pool;
  /// PRACH buffer pool.
  std::unique_ptr<prach_buffer_pool> prach_pool;
  /// Softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> softbuffer_pool;
  /// Downlink processor PDUs validator.
  std::unique_ptr<downlink_pdu_validator> dl_pdu_validator;
  /// Uplink processor PDUs validator.
  std::unique_ptr<uplink_pdu_validator> ul_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor_impl ul_request_processor;
  /// Uplink slot PDU registry.
  uplink_slot_pdu_repository pdu_repository;
  /// Upper PHY results notifier.
  upper_phy_rx_results_notifier_wrapper rx_results_notifier;
  /// Received symbols handler.
  std::unique_ptr<upper_phy_rx_symbol_handler> rx_symbol_handler;
  /// Timing events handler.
  upper_phy_timing_handler_impl timing_handler;
};

} // namespace srsgnb
