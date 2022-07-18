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

#include "srsgnb/fapi/messages.h"
#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/ran/slot_point.h"
#include <mutex>

namespace srsgnb {

class downlink_processor;
class downlink_processor_pool;
class resource_grid_pool;

namespace fapi_adaptor {

/// \brief This class receives FAPI slot based messages and translates them into PHY specific data types, commanding the
/// upper PHY to process them.
///
/// Translating and processing the FAPI slot based messages involves converting the data structures, checking for
/// message validity at slot level and selecting and reserving resources in the PHY to process the messages.
///
/// \note The translator assumes that the contents of the incoming FAPI slot message are valid, ie: it has been
/// previously validated using the provided FAPI validators.
/// \note This class has been designed to be thread safe to allow calling the \c set_handle() method and message
/// handlers from different threads.
/// \note The translator assumes that only one message of each type can be received for a slot. If multiples messages of
/// the same type arrives, this will result in undefined behaviour.
class fapi_to_phy_translator : public fapi::slot_message_gateway
{
  /// RAII style class which is meant to have a lifetime of a single slot. It executes the preparation and closing
  /// procedures required by the upper PHY within a slot.
  ///
  /// \note The lifetime of this object is meant to be a single slot point.
  class slot_based_upper_phy_controller
  {
    slot_point                                 slot;
    std::reference_wrapper<downlink_processor> dl_processor;

  public:
    slot_based_upper_phy_controller();

    slot_based_upper_phy_controller(downlink_processor_pool& dl_processor_pool,
                                    resource_grid_pool&      rg_pool,
                                    slot_point               slot,
                                    unsigned                 sector_id);

    slot_based_upper_phy_controller(slot_based_upper_phy_controller&& other) = delete;

    slot_based_upper_phy_controller& operator=(slot_based_upper_phy_controller&& other);

    ~slot_based_upper_phy_controller();

    downlink_processor*       operator->() { return &dl_processor.get(); }
    const downlink_processor* operator->() const { return &dl_processor.get(); }
  };

public:
  /// \brief Constructor for the FAPI to PHY translator.
  ///
  /// \param sector_id Sector identifier.
  /// \param dl_processor_pool Downlink processor pool that will be used to process PDUs.
  /// \param rg_pool Resource grid pool that will be used to process PDUs.
  /// \param scs_common subcarrier spacing common, as per TS 331, Section 6.2.2,
  fapi_to_phy_translator(unsigned                 sector_id,
                         downlink_processor_pool& dl_processor_pool,
                         resource_grid_pool&      rg_pool,
                         subcarrier_spacing       scs_common) :
    sector_id(sector_id), dl_processor_pool(dl_processor_pool), rg_pool(rg_pool), scs_common(scs_common)
  {
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
  /// Handling a new slot consists of the following steps:
  /// - Finishing processing the PDUs from the previous slot.
  /// - Updating the current slot.
  /// - Grabbing a resource grid and a downlink processor.
  /// - Configuring the downlink processor with the new resource grid.
  /// - Resetting the pdsch_pdu vector.
  ///
  /// \param slot Identifies the new slot.
  /// \note This method may be called from a different thread compared to the rest of methods.
  void handle_new_slot(slot_point slot);

private:
  const unsigned           sector_id;
  downlink_processor_pool& dl_processor_pool;
  resource_grid_pool&      rg_pool;

  slot_based_upper_phy_controller                                                   current_slot_controller;
  static_vector<pdsch_processor::pdu_t, fapi::dl_tti_request_message::MAX_NUM_PDUS> pdsch_pdus;
  // Protects current_slot_controller and pdsch_pdus.
  //: TODO: make this lock free.
  std::mutex mutex;

  // :TODO: this variable should be asked to the cell configuration. Remove it when it's available.
  const subcarrier_spacing scs_common;
};

} // namespace fapi_adaptor
} // namespace srsgnb
