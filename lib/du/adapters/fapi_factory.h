/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"

namespace srsran {

std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
build_phy_fapi_adaptor(unsigned                                                   sector_id,
                       subcarrier_spacing                                         scs,
                       subcarrier_spacing                                         scs_common,
                       downlink_processor_pool&                                   dl_processor_pool,
                       resource_grid_pool&                                        dl_rg_pool,
                       uplink_request_processor&                                  ul_request_processor,
                       resource_grid_pool&                                        ul_rg_pool,
                       uplink_slot_pdu_repository&                                ul_pdu_repository,
                       const downlink_pdu_validator&                              dl_pdu_validator,
                       const uplink_pdu_validator&                                ul_pdu_validator,
                       const fapi::prach_config&                                  prach_cfg,
                       const fapi::carrier_config&                                carrier_cfg,
                       std::unique_ptr<fapi_adaptor::precoding_matrix_repository> pm_repo,
                       task_executor&  async_executor,
                       tx_buffer_pool& buffer_pool);

std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>
build_mac_fapi_adaptor(unsigned                                               sector_id,
                       subcarrier_spacing                                     scs,
                       fapi::slot_message_gateway&                            gateway,
                       fapi::slot_last_message_notifier&                      last_msg_notifier,
                       std::unique_ptr<fapi_adaptor::precoding_matrix_mapper> pm_mapper,
                       unsigned                                               cell_nof_prbs);

} // namespace srsran
