/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_PHY_FAPI_TO_PHY_TRANSLATOR_H
#define SRSGNB_LIB_FAPI_ADAPTOR_PHY_FAPI_TO_PHY_TRANSLATOR_H

#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/ran/slot_point.h"
#include <mutex>

namespace srsgnb {

class downlink_processor;
class downlink_processor_pool;
class resource_grid_pool;

namespace fapi_adaptor {

/// \brief This class listen to FAPI slot messages and translate them into SRS PHY messages, so they can be processed by
/// the PHY. IT also commands the upper PHY to process them.
///
/// Translating and processing the FAPI slot messages involves converting the data structures, checking of message
/// validity at slot level, selecting and reserving resources in the PHY to process the messages.
///
/// \note This translator assumes that the contents of the incoming FAPI slot message is valid, ie, that it has
/// previously been validated using the provided FAPI validators.
class fapi_to_phy_translator : public fapi::slot_message_gateway
{
  /// Helper class that wraps the information needed by the translator for a given slot.
  ///
  /// \note The scope of these objects is a slot point. When a new slot starts an object it's created while the and the
  /// previous one gets destroyed.
  class slot_task_processor
  {
    slot_point                                 slot;
    std::reference_wrapper<downlink_processor> dl_processor;

  public:
    slot_task_processor();

    slot_task_processor(downlink_processor_pool& dl_processor_pool,
                        resource_grid_pool&      rg_pool,
                        slot_point               slot,
                        unsigned                 sector_id);

    slot_task_processor& operator=(slot_task_processor&& other);

    ~slot_task_processor();

    /// Overloads the operator to give access to the downlink processor.
    downlink_processor* operator->() { return &dl_processor.get(); }
  };

public:
  fapi_to_phy_translator(unsigned sector_id, downlink_processor_pool& dl_processor_pool, resource_grid_pool& rg_pool) :
    sector_id(sector_id), dl_processor_pool(dl_processor_pool), rg_pool(rg_pool)
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
  /// Handling a new slot consists of finishing processing pdus of the previous slot, updating the current slot,
  /// grabbing a resource grid and a downlink processor and configuring the downlink processor with the resource grid.
  ///
  /// \param slot Identifies the new slot.
  void handle_new_slot(const slot_point& slot);

private:
  const unsigned           sector_id;
  downlink_processor_pool& dl_processor_pool;
  resource_grid_pool&      rg_pool;

  slot_task_processor      current_task_processor;
  // Protects current_task_processors.
  std::mutex mutex;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_PHY_FAPI_TO_PHY_TRANSLATOR_H
