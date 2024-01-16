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

/// \file
/// \brief Uplink processor interface.

#pragma once

#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/phy/upper/uplink_processor_context.h"

namespace srsran {

class prach_buffer;
struct prach_buffer_context;
class slot_point;
class upper_phy_rx_results_notifier;

/// \brief Uplink processor interface.
///
/// The uplink processor is in charge of handling incoming requests to process the physical uplink channels within
/// a certain slot.
///
/// Requests are dispatched asynchronously as they get enqueued for execution, and generate an event through the \ref
/// upper_phy_rx_results_notifier interface upon completion.
class uplink_processor
{
public:
  /// PUSCH PDU configuration.
  struct pusch_pdu {
    /// HARQ process number.
    unsigned harq_id;
    /// Transport block size.
    units::bytes tb_size;
    /// PUSCH processor PDU.
    pusch_processor::pdu_t pdu;
  };

  /// PUCCH PDU configuration.
  struct pucch_pdu {
    /// PUCCH context;
    ul_pucch_context context;
    // :TODO: the formats will use a variant when available.
    /// PUCCH format 0.
    pucch_processor::format0_configuration format0;
    /// PUCCH format 1.
    pucch_processor::format1_configuration format1;
    /// PUCCH format 2.
    pucch_processor::format2_configuration format2;
    /// PUCCH format 3.
    pucch_processor::format3_configuration format3;
    /// PUCCH format 4.
    pucch_processor::format4_configuration format4;
  };

  virtual ~uplink_processor() = default;

  /// \brief Processes the PRACH using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the upper_phy_rx_results_notifier with event \ref
  /// upper_phy_rx_results_notifier::on_new_prach_results "on_new_prach_results".
  ///
  /// \param[in] notifier Event notification interface.
  /// \param[in] buffer   Channel symbols the PRACH detection is performed on.
  /// \param[in] context  Context used by the underlying PRACH detector.
  virtual void process_prach(upper_phy_rx_results_notifier& notifier,
                             const prach_buffer&            buffer,
                             const prach_buffer_context&    context) = 0;

  /// \brief Processes a PUSCH transmission.
  ///
  /// The size of each transport block is determined by <tt>data[TB index].size()</tt>.
  ///
  /// The PUSCH results will be notified by the upper_phy_rx_results_notifier with event \ref
  /// upper_phy_rx_results_notifier::on_new_pusch_results "on_new_pusch_results".
  ///
  /// \param[out]    data       Received transport block.
  /// \param[in,out] rm_buffer  Rate matcher buffer.
  /// \param[in]     notifier   Event notification interface.
  /// \param[in]     grid       Grid the capture data is stored in.
  /// \param[in]     pdu        PUSCH transmission parameters.
  virtual void process_pusch(span<uint8_t>                      data,
                             unique_rx_buffer                   rm_buffer,
                             upper_phy_rx_results_notifier&     notifier,
                             const resource_grid_reader&        grid,
                             const uplink_processor::pusch_pdu& pdu) = 0;

  /// \brief Processes a PUCCH transmission.
  ///
  /// The PUCCH results will be notified by the upper_phy_rx_results_notifier with event \ref
  /// upper_phy_rx_results_notifier::on_new_pucch_results "on_new_pucch_results".
  ///
  /// \param[in] notifier Event notification interface.
  /// \param[in] grid     Resource grid.
  /// \param[in] pdu      PUCCH transmission parameters.
  virtual void
  process_pucch(upper_phy_rx_results_notifier& notifier, const resource_grid_reader& grid, const pucch_pdu& pdu) = 0;
};

/// Uplink processor validation interface.
class uplink_pdu_validator
{
public:
  /// Default destructor.
  virtual ~uplink_pdu_validator() = default;

  /// \brief Validates PRACH detector configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const prach_detector::configuration& config) const = 0;

  /// \brief Validates PUCCH Format 0 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format0_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 1 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format1_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 2 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format2_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 3 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format3_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 4 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format4_configuration& config) const = 0;

  /// \brief Validates PUSCH configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pusch_processor::pdu_t& pdu) const = 0;
};

/// \brief Pool of uplink processors.
///
/// This interface manages the access to the available uplink processors.
class uplink_processor_pool
{
public:
  virtual ~uplink_processor_pool() = default;

  /// \brief Returns an uplink processor for the given slot and sector.
  ///
  /// \param slot[in]      Slot point.
  /// \param sector_id[in] Sector identifier.
  /// \return An uplink processor.
  virtual uplink_processor& get_processor(slot_point slot, unsigned sector_id) = 0;
};

} // namespace srsran
