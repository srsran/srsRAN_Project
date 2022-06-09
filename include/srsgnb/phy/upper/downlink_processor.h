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
#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"

namespace srsgnb {

struct resource_grid_context;

/// Downlink processor class that groups and process all the downlink channels within a slot.
///
/// \note: The downlink processor must be configured before starting to process downlink PDUs by calling
/// configure_resource_grid() method. After all the PDUs are processed by the process_* methods, tx_resource_grid should
/// be called to transmit the resource grid. If this order is not respected, using the interface will result in
/// undefined behaviour.
class downlink_processor
{
public:
  virtual ~downlink_processor() = default;

  /// \brief Process the given PDCCH PDU.
  ///
  /// \param[in] pdu PDCCH PDU to process.
  virtual void process_pdcch(pdcch_processor::pdu_t& pdu) = 0;

  /// \brief Process the given PDSCH PDU and its related data, which it is given in data parameter.
  ///
  /// \param[in] data Contains the data of the PDSCH.
  /// \param[in] pdu PDSCH PDU to process.
  virtual void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                             const pdsch_processor::pdu_t& pdu) = 0;

  /// \brief Process the given SSB PDU.
  ///
  /// \param[in] pdu SSB PDU to process.
  virtual void process_ssb(const ssb_processor::pdu_t& pdu) = 0;

  /// \brief Process the given CSI-RS configuration.
  ///
  /// \param[in] config CSI-RS configuration to process.
  virtual void process_csi_rs(const csi_rs_processor::config_t& config) = 0;

  /// \brief Configures the resource grid of the downlink_processor.
  ///
  /// \param[in] context Resource grid context that contains the information of the processing slot.
  /// \param[in] grid Resource grid that will contain the data of the processed downlink channels.
  /// \note Calling this method is mandatory before processing any PDU.
  // :TODO: move this method to other interface to avoid controlling the order of the methods execution.
  virtual void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) = 0;

  /// \brief Stops accepting PDUs. When this method is called, the interface will not expect to process more PDUs, so
  /// once it finishes to process all the previous accepted PDUs, the resource grid will be sent.
  // :TODO: move this method to other interface to avoid controlling the order of the methods execution.
  virtual void finish_processing_pdus() = 0;
};

/// Pool to access a downlink processor.
class downlink_processor_pool
{
public:
  virtual ~downlink_processor_pool() = default;

  /// \brief Returns a downlink processor with the given slot and sector.
  virtual downlink_processor& get_processor(const slot_point& slot, unsigned sector) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_DOWNLINK_PROCESSOR_H
