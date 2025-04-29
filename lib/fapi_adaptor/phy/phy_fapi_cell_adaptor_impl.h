/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/messages/config_request_tlvs.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_cell_adaptor.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

/// PHY-FAPI cell adaptor implementation.
class phy_fapi_cell_adaptor_impl : public phy_fapi_cell_adaptor
{
  struct fapi_cell_config {
    fapi::phy_config     phy_cfg;
    fapi::carrier_config carrier_cfg;
    fapi::cell_config    cell_cfg;
    fapi::prach_config   prach_cfg;
    fapi::ssb_config     ssb_cfg;
    fapi::tdd_phy_config tdd_cfg;
  };

  class phy_fapi_config_message_adaptor_impl;

public:
  phy_fapi_cell_adaptor_impl();

  // See interface for documentation.
  phy_fapi_config_message_adaptor& get_config_message_adaptor() override;

  // See interface for documentation.
  phy_fapi_slot_message_adaptor* get_message_adaptor() override { return slot_adaptor.get(); }

  void start()
  {
    // This start should start the cell. This means:
    //  - Get the cell configuration
    //  - Create O-DU low:
    //       - Create DU low
    //       - Create slot message adaptor
    //       - Connect adaptor with DU low
    //  - Create RU
    //  - Connect the O-DU low with RU.
    //  - Start O-DU low + RU.
  }

  void stop() {}

private:
  srslog::basic_logger&                                 logger;
  fapi_cell_config                                      cell_config;
  std::unique_ptr<phy_fapi_config_message_adaptor_impl> config_adaptor;
  std::unique_ptr<phy_fapi_slot_message_adaptor>        slot_adaptor;
};

} // namespace fapi_adaptor
} // namespace srsran
