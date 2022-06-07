/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_DOWNLINK_PROCESSOR_H
#define SRSGNB_PHY_UPPER_DOWNLINK_PROCESSOR_H

#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"

namespace srsgnb {

class resource_grid_context;

/// Downlink processor class that groups and process all the downlink channels within a slot.
class downlink_processor
{
public:
  virtual ~downlink_processor() = default;

  /// \brief Process the given PDCCH pdu.
  ///
  /// \param[in] pdu PDCCH PDU to process.
  virtual void process_pdcch(pdcch_processor::pdu_t& pdu) = 0;

  /// \brief Process the given PDSCH pdu and its related data, which it is given in data parameter.
  ///
  /// \param[in] data Contains the data of the PDSCH.
  /// \param[in] pdu PDSCH PDU to process.
  virtual void process_pdsch(static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
                             const pdsch_processor::pdu_t&                                                 pdu) = 0;

  /// \brief Process the given SSB pdu.
  ///
  /// \param pdu SSB PDU to process.
  virtual void process_ssb(const ssb_processor::pdu_t& pdu) = 0;

  /// \brief Configures the resource grid of the downlink_processor.
  ///
  /// \param context Resource grid context that contains the information of the processing slot.
  /// \param grid Resource grid that will contain the data of the processed downlink channels.
  virtual void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) = 0;

  /// \brief Sends the resource grid.
  virtual void send_resource_grid() = 0;
};

/// Pool to access a downlink processor.
class downlink_processor_pool
{
public:
  virtual ~downlink_processor_pool() = default;

  /// \brief Returns a downlink processor with the given slot and sector.
  virtual downlink_processor&       get_processor(const slot_point& slot, unsigned sector)       = 0;
  virtual const downlink_processor& get_processor(const slot_point& slot, unsigned sector) const = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_DOWNLINK_PROCESSOR_H
