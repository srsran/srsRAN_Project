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

#pragma once

#include "srsran/fapi/messages.h"
#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_repository.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/support/executors/task_executor.h"
#include <atomic>

namespace srsran {

class downlink_pdu_validator;
class downlink_processor;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class uplink_slot_pdu_repository;

namespace fapi_adaptor {

/// FAPI-to-PHY translator configuration.
struct fapi_to_phy_translator_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Request headroom size in slots.
  unsigned nof_slots_request_headroom;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// FAPI PRACH configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config* prach_cfg;
  /// FAPI carrier configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config* carrier_cfg;
  /// PRACH port list.
  std::vector<uint8_t> prach_ports;
};

/// FAPI-to-PHY translator dependencies.
struct fapi_to_phy_translator_dependencies {
  /// Logger.
  srslog::basic_logger* logger;
  /// Downlink processor pool.
  downlink_processor_pool* dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool* dl_rg_pool;
  /// Downlink PDU validator.
  const downlink_pdu_validator* dl_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor* ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool* ul_rg_pool;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository* ul_pdu_repository;
  /// Uplink PDU validator.
  const uplink_pdu_validator* ul_pdu_validator;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
};

/// \brief FAPI-to-PHY message translator.
///
/// This class receives slot-based FAPI messages and translates them into PHY specific data types, which can then be
/// processed by the upper PHY.
///
/// Translating and processing slot-based FAPI messages involves converting them to the suitable data types for the PHY
/// layer, validation of the contents for each incoming message and selecting and reserving resources in the upper PHY
/// to process the messages.
///
/// \note This class is thread safe and allows calling the \ref handle_new_slot method and the message handlers from
/// different threads.
/// \note The translator assumes that only one message of each type can be received within a slot. Receiving multiple
/// messages of the same type in one slot results in undefined behavior.
/// \note The translator is designed to work for a sector and subcarrier spacing. Supporting more than one sector and/or
/// subcarrier spacing will require more instances of the translator.
class fapi_to_phy_translator : public fapi::slot_message_gateway
{
  /// \brief Slot-based upper PHY controller.
  ///
  /// Takes care of the opening and closing procedures required by the upper PHY within each slot.
  ///
  /// \note This class follows the RAII (resource acquisition is initialization) programming principle.
  /// \note The lifetime of any instantiation of this class is meant to be a single slot.
  class slot_based_upper_phy_controller
  {
    slot_point                                 slot;
    std::reference_wrapper<downlink_processor> dl_processor;

  public:
    slot_based_upper_phy_controller();

    slot_based_upper_phy_controller(downlink_processor_pool& dl_processor_pool,
                                    resource_grid_pool&      rg_pool,
                                    slot_point               slot_,
                                    unsigned                 sector_id);

    slot_based_upper_phy_controller(slot_based_upper_phy_controller&& other) = delete;

    slot_based_upper_phy_controller& operator=(slot_based_upper_phy_controller&& other);

    ~slot_based_upper_phy_controller();

    /// Overloaded member of pointer operator.
    downlink_processor* operator->() { return &dl_processor.get(); }
    /// Overloaded member of pointer operator.
    const downlink_processor* operator->() const { return &dl_processor.get(); }
  };

  /// Manages slot based controllers.
  class slot_based_upper_phy_controller_manager
  {
    slot_based_upper_phy_controller              dummy;
    downlink_processor_pool&                     dl_processor_pool;
    resource_grid_pool&                          rg_pool;
    const unsigned                               sector_id;
    std::vector<slot_based_upper_phy_controller> controllers;

    /// Returns the controller for the given slot.
    slot_based_upper_phy_controller& controller(slot_point slot)
    {
      return controllers[slot.system_slot() % controllers.size()];
    }

    /// Returns the controller for the given slot.
    const slot_based_upper_phy_controller& controller(slot_point slot) const
    {
      return controllers[slot.system_slot() % controllers.size()];
    }

  public:
    slot_based_upper_phy_controller_manager(downlink_processor_pool& dl_processor_pool_,
                                            resource_grid_pool&      rg_pool_,
                                            unsigned                 sector_id_,
                                            unsigned                 nof_slots_request_headroom);

    /// Acquires and returns the controller for the given slot.
    slot_based_upper_phy_controller& acquire_controller(slot_point slot);

    /// \brief Releases the controller for the given slot.
    ///
    /// If the controller has already been released, this function does nothing.
    void release_controller(slot_point slot);

    /// Returns the controller for the given slot.
    slot_based_upper_phy_controller& get_controller(slot_point slot);
  };

  /// \brief PDSCH PDU repository.
  ///
  /// Stores the PDSCH PDUs for later processing.
  struct pdsch_pdu_repository {
    slot_point                                                     slot;
    static_vector<pdsch_processor::pdu_t, MAX_PDSCH_PDUS_PER_SLOT> pdus;

    /// Returns true if the repository is empty, otherwise false.
    bool empty() const { return pdus.empty(); }

    /// Clears the repository.
    void clear()
    {
      slot = slot_point();
      pdus.clear();
    }

    /// Resets the repository by clearing the PDUs and setting the slot to the given one.
    void reset(slot_point new_slot)
    {
      pdus.clear();
      slot = new_slot;
    }
  };

public:
  fapi_to_phy_translator(const fapi_to_phy_translator_config&  config,
                         fapi_to_phy_translator_dependencies&& dependencies);

  // See interface for documentation.
  void dl_tti_request(const fapi::dl_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_tti_request(const fapi::ul_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_dci_request(const fapi::ul_dci_request_message& msg) override;

  // See interface for documentation.
  void tx_data_request(const fapi::tx_data_request_message& msg) override;

  /// \brief Handles a new slot.
  ///
  /// Handling a new slot consists of the following steps.
  /// - Finish processing the PDUs from a past slot.
  /// - Update the current slot value to the new one.
  /// - Reset the contents of the uplink PDU repository.
  ///
  /// \param[in] slot Identifies the new slot.
  /// \note This method is thread safe and may be called from different threads.
  void handle_new_slot(slot_point slot);

  /// Configures the FAPI slot-based, error-specific notifier to the given one.
  void set_slot_error_message_notifier(fapi::slot_error_message_notifier& fapi_error_notifier)
  {
    error_notifier = std::ref(fapi_error_notifier);
  }

private:
  /// Returns true if the given message arrived in time, otherwise returns false.
  template <typename T>
  bool is_message_in_time(const T& msg) const;

  /// Returns this adaptor current slot.
  slot_point get_current_slot() const
  {
    return slot_point(scs, current_slot_count_val.load(std::memory_order_acquire));
  }

  /// Updates this adaptor current slot to the given value.
  void update_current_slot(slot_point slot)
  {
    // Update the atomic variable that holds the slot point.
    current_slot_count_val.store(slot.system_slot(), std::memory_order_release);
  }

private:
  /// Sector identifier.
  const unsigned sector_id;
  /// Request headroom size in slots.
  const unsigned nof_slots_request_headroom;
  /// Logger.
  srslog::basic_logger& logger;
  /// Downlink PDU validator.
  const downlink_pdu_validator& dl_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor& ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool& ul_rg_pool;
  /// Uplink PDU validator.
  const uplink_pdu_validator& ul_pdu_validator;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository& ul_pdu_repository;
  /// Current slot count value.
  std::atomic<uint32_t> current_slot_count_val;
  /// Slot controller manager.
  slot_based_upper_phy_controller_manager slot_controller_mngr;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
  /// Error indication notifier.
  std::reference_wrapper<fapi::slot_error_message_notifier> error_notifier;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  const subcarrier_spacing scs;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  const subcarrier_spacing scs_common;
  /// PRACH configuration as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config prach_cfg;
  /// Carrier configuration as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config carrier_cfg;
  /// PRACH receive ports.
  const static_vector<uint8_t, MAX_PORTS> prach_ports;
  /// PDSCH PDU repository.
  pdsch_pdu_repository pdsch_repository;
};

} // namespace fapi_adaptor
} // namespace srsran
