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

#include "srsran/phy/upper/channel_processors/pdcch_processor.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"

namespace srsran {

struct resource_grid_context;
class resource_grid;

/// \brief Downlink processor class that groups and process all the downlink channels within a slot.
///
/// The downlink processor process all the given downlink PDUs and sends the configured resource grid through a gateway
/// when every PDU has finished processing and the finish_processing_pdus() has been called. Prior to start processing
/// PDUs, configure_resource_grid() must be called in order to configure the resource grid and the context for the
/// downlink processor.
///
/// \note
/// Undefined behavior can be caused by:
/// - Calling any process method prior to configure_resource_grid().
/// - Calling finish_processing_pdus() without calling configure_resource_grid() in a slot context.
class downlink_processor
{
public:
  virtual ~downlink_processor() = default;

  /// \brief Process the given PDCCH PDU.
  ///
  /// \param[in] pdu PDCCH PDU to process.
  /// \return \c true if the PDCCH PDU processing task is created and enqueued successfully, \c false otherwise.
  virtual bool process_pdcch(const pdcch_processor::pdu_t& pdu) = 0;

  /// \brief Process the given PDSCH PDU and its related data, which it is given in data parameter.
  ///
  /// \param[in] data Contains the data of the PDSCH.
  /// \param[in] pdu PDSCH PDU to process.
  /// \return \c true if the PDSCH PDU processing task is created and enqueued successfully, \c false otherwise.
  virtual bool process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                             const pdsch_processor::pdu_t& pdu) = 0;

  /// \brief Process the given SSB PDU.
  ///
  /// \param[in] pdu SSB PDU to pr
  /// \return \c true if the SSB PDU processing task is created and enqueued successfully, \c false otherwise.
  virtual bool process_ssb(const ssb_processor::pdu_t& pdu) = 0;

  /// \brief Process the given NZP-CSI-RS configuration.
  ///
  /// \param[in] config NZP-CSI-RS configuration to process.
  /// \return \c true if the NZP CSI-RS processing task is created and enqueued successfully, \c false otherwise.
  virtual bool process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) = 0;

  /// \brief Configures the resource grid of the downlink_processor.
  ///
  /// \param[in] context Resource grid context that contains the information of the processing slot.
  /// \param[in] grid Resource grid that will contain the data of the processed downlink channels.
  /// \return \c true if the resource grid is successfully configured, \c false otherwise.
  ///
  /// \note
  /// - Calling this method is mandatory before processing any PDU.
  /// - The resource grid number of ports and bandwidth must be sufficient to accommodate all the PDUs.
  // :TODO: move this method to other interface to avoid controlling the order of the methods execution.
  virtual bool configure_resource_grid(const resource_grid_context& context, resource_grid& grid) = 0;

  /// \brief Stops accepting PDUs.
  ///
  /// When this method is called, the interface will not expect to process more PDUs, so once it finishes to process all
  /// the enqueued PDUs, the resource grid will be sent to the lower bound gateway using the \c context from
  /// configure_resource_grid() to provide the processing context of the resource grid in the lower physical layer.
  // :TODO: move this method to other interface to avoid controlling the order of the methods execution.
  virtual void finish_processing_pdus() = 0;
};

/// Downlink processor validation interface.
class downlink_pdu_validator
{
public:
  /// Default destructor.
  virtual ~downlink_pdu_validator() = default;

  /// \brief Validates SSB processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const ssb_processor::pdu_t& pdu) const = 0;

  /// \brief Validates PDCCH processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const pdcch_processor::pdu_t& pdu) const = 0;

  /// \brief Validates PDSCH processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const pdsch_processor::pdu_t& pdu) const = 0;

  /// \brief Validates NZP-CSI-RS generator configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const nzp_csi_rs_generator::config_t& config) const = 0;
};

/// Pool to access a downlink processor.
class downlink_processor_pool
{
public:
  virtual ~downlink_processor_pool() = default;

  /// \brief Returns a downlink processor with the given slot and sector.
  ///
  /// \param slot Slot point.
  /// \param sector_id Sector ID.
  /// \return A downlink processor.
  virtual downlink_processor& get_processor(slot_point slot, unsigned sector_id) = 0;
};

} // namespace srsran
