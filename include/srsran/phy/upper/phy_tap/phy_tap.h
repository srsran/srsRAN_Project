/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/uplink_pdu_slot_repository.h"

namespace srsran {

/// \brief Interface for tapping into the uplink symbols received by the PHY.
///
/// This interface exposes the received uplink symbols to any external processing unit. A third party plugin can
/// implement this class and obtain the uplink symbols along with their associated physical layer PDUs. The interface
/// also allows rewriting the symbols in the resource grid, which enables the insertion of additional algorithms to
/// the physical layer processing chain.
class phy_tap
{
public:
  /// Default destructor.
  virtual ~phy_tap() = default;

  /// \brief Handler that exposes the received uplink symbols to any external processing unit.
  ///
  /// This method is called from the upper physical layer for every received uplink symbol. It provides access to the
  /// received symbols of the current \ref slot up to the last received \ref symbol. The symbols can be modified and
  /// written back to the resource grid. The method also provides the list of physical layer PDUs scheduled in the
  /// current slot and with allocations ending at the current symbol.
  ///
  /// \param[out]    grid_writer   Resource grid writer. It can be used send the modified symbols to the physical layer.
  /// \param[in]     grid_reader   Resource grid reader, used to access the received symbols.
  /// \param[in]     slot          Current slot.
  /// \param[in]     symbol        Current symbol index within the slot.
  /// \param[in]     pusch_pdus    PUSCH PDUs scheduled in the slot up to the current symbol.
  /// \param[in]     pucch_pdus    PUCCH PDUs scheduled in the slot up to the current symbol.
  /// \param[in]     pucch_f1_pdus Common parameters of PUCCH Format 1 PDUs scheduled up to the current symbol.
  /// \param[in]     srs_pdus      SRS PDUs scheduled in the slot up to the current symbol.
  ///
  /// \remark The method implementation may assume that the physical layer does not consume the Resource
  /// Elements (RE) belonging to a PUSCH, PUCCH or SRS allocation until the last symbol of the allocation is received
  /// and after this method is called. This means that the REs of a PUSCH, PUCCH or SRS allocation can be processed and
  /// returned to the physical layer before any further processing takes place.
  ///
  /// \warning The contents of the resource grid belonging to a PDU shall not be modified after the call that passed the
  /// PDU returns. Modifying the contents of the resource grid belonging to a PDU that was passed in the past may lead
  /// to undefined behavior.
  ///
  /// \warning This method is called from the RU UL execution context. Performing heavy processing tasks synchronously
  /// in this method may lead to realtime errors.
  virtual void handle_ul_symbol(resource_grid_writer&                                     grid_writer,
                                const resource_grid_reader&                               grid_reader,
                                slot_point                                                slot,
                                unsigned                                                  symbol,
                                span<const uplink_pdu_slot_repository::pusch_pdu>         pusch_pdus,
                                span<const uplink_pdu_slot_repository::pucch_pdu>         pucch_pdus,
                                span<const pucch_processor::format1_common_configuration> pucch_f1_pdus,
                                span<const uplink_pdu_slot_repository::srs_pdu>           srs_pdus) = 0;
};

} // namespace srsran
