/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/phy/upper/uplink_slot_pdu_repository.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsgnb/srslog/srslog.h"
#include <vector>

namespace srsgnb {

class uplink_processor;
class uplink_processor_pool;
class upper_phy_rx_results_notifier;

/// Payload buffer pool for received data.
class rx_payload_buffer_pool
{
  /// Maximum number of payloads contained by the pool.
  static const size_t MAX_NUM_PAYLOAD = 4096U;

public:
  /// Returns the next available buffer from the pool.
  span<uint8_t> acquire_payload_buffer(size_t size)
  {
    unsigned i = index++ % MAX_NUM_PAYLOAD;
    pool[i].resize(size);
    return pool[i];
  }

private:
  /// Buffer pool.
  circular_array<std::vector<uint8_t>, MAX_NUM_PAYLOAD> pool;
  /// Index used to retrieve the next container.
  unsigned index = 0;
};

/// \brief Implementation of the upper PHY handler of receive symbols events.
///
/// In this implementation, handling the newly received symbol event takes the following steps.
///     1. Get an uplink processor from the pool.
///     2. Call the corresponding method from the uplink processor based on the type of the received symbol (PRACH, SRS,
///        PUSCH).
class upper_phy_rx_symbol_handler_impl : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_impl(uplink_processor_pool&         ul_processor_pool,
                                   uplink_slot_pdu_repository&    ul_pdu_repository,
                                   rx_softbuffer_pool&            softbuffer_pool,
                                   upper_phy_rx_results_notifier& rx_results_notifier,
                                   srslog::basic_logger&          logger);

  // See interface for documentation.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) override;

private:
  /// Process the given PUSCH PDU using the given uplink processor, grid and slot.
  void process_pusch(const uplink_processor::pusch_pdu& pdu,
                     uplink_processor&                  ul_processor,
                     const resource_grid_reader&        grid,
                     slot_point                         slot);

private:
  /// Uplink processor pool.
  uplink_processor_pool& ul_processor_pool;
  /// Uplink slot PDU registry.
  uplink_slot_pdu_repository& ul_pdu_repository;
  /// Softbuffer pool.
  rx_softbuffer_pool& softbuffer_pool;
  /// Upper PHY results notifier.
  upper_phy_rx_results_notifier& rx_results_notifier;
  /// Upper PHY logger.
  srslog::basic_logger& logger;
  /// Pool of containers for the payload.
  rx_payload_buffer_pool rx_payload_pool;
};

} // namespace srsgnb
