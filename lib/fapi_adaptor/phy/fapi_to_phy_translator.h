/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include <mutex>

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
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Downlink processor pool.
  downlink_processor_pool* dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool* dl_rg_pool;
  /// Uplink request processor.
  uplink_request_processor* ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool* ul_rg_pool;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository* ul_pdu_repository;
  /// Downlink PDU validator.
  const downlink_pdu_validator* dl_pdu_validator;
  /// Uplink PDU validator.
  const uplink_pdu_validator* ul_pdu_validator;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// FAPI PRACH configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config* prach_cfg;
  /// FAPI carrier configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config* carrier_cfg;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
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

    explicit slot_based_upper_phy_controller(slot_point slot_);

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

    /// Returns the associated slot for this controller.
    slot_point get_slot() const { return slot; }
  };

public:
  explicit fapi_to_phy_translator(fapi_to_phy_translator_config&& config, srslog::basic_logger& logger_) :
    sector_id(config.sector_id),
    pm_repo(std::move(config.pm_repo)),
    dl_processor_pool(*config.dl_processor_pool),
    dl_rg_pool(*config.dl_rg_pool),
    ul_request_processor(*config.ul_request_processor),
    ul_rg_pool(*config.ul_rg_pool),
    dl_pdu_validator(*config.dl_pdu_validator),
    ul_pdu_validator(*config.ul_pdu_validator),
    ul_pdu_repository(*config.ul_pdu_repository),
    scs(config.scs),
    scs_common(config.scs_common),
    prach_cfg(*config.prach_cfg),
    carrier_cfg(*config.carrier_cfg),
    logger(logger_)
  {
    srsran_assert(pm_repo, "Invalid precoding matrix repository");
  }

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
  /// - Finish processing the PDUs from the previous slot.
  /// - Update the current slot value to the new one.
  /// - Obtain a new resource grid and a new downlink processor from the corresponding pools.
  /// - Configure the downlink processor with the new resource grid.
  /// - Reset the contents of the PDSCH PDU repository.
  ///
  /// \param[in] slot Identifies the new slot.
  /// \note This method is thread safe and may be called from different threads.
  void handle_new_slot(slot_point slot);

private:
  /// Returns true if the given message arrived in time, otherwise returns false.
  template <typename T>
  bool is_message_in_time(const T& msg) const
  {
    return (current_slot_controller.get_slot().slot_index() == msg.slot &&
            current_slot_controller.get_slot().sfn() == msg.sfn);
  }

private:
  /// Sector identifier.
  const unsigned sector_id;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// Downlink processor pool.
  downlink_processor_pool& dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool& dl_rg_pool;
  /// Uplink request processor.
  uplink_request_processor& ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool& ul_rg_pool;
  /// Downlink PDU validator.
  const downlink_pdu_validator& dl_pdu_validator;
  /// Uplink PDU validator.
  const uplink_pdu_validator& ul_pdu_validator;
  /// Current slot task controller.
  slot_based_upper_phy_controller current_slot_controller;
  /// PDSCH PDU repository.
  static_vector<pdsch_processor::pdu_t, MAX_DL_PDSCH_PDUS_PER_SLOT> pdsch_pdu_repository;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository& ul_pdu_repository;
  /// Protects concurrent access to shared variables.
  //: TODO: make this lock free.
  std::mutex mutex;
  // :TODO: these variables should be asked to the cell configuration. Remove them when they're available.
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  const subcarrier_spacing scs;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  const subcarrier_spacing scs_common;
  /// PRACH configuration as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config prach_cfg;
  /// Carrier configuration as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config carrier_cfg;
  /// Logger.
  srslog::basic_logger& logger;
};

} // namespace fapi_adaptor
} // namespace srsran
