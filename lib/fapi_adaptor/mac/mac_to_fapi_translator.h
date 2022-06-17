/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_TO_FAPI_TRANSLATOR_H
#define SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_TO_FAPI_TRANSLATOR_H

#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief This class listens to MAC cell result messages converting them to FAPI messages and sending them through the
/// FAPI \c slot_message_gateway interface.
class mac_to_fapi_translator : public mac_cell_result_notifier
{
public:
  explicit mac_to_fapi_translator(fapi::slot_message_gateway& gateway) : gateway(gateway) {}

  // See interface for documentation.
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;

  // See interface for documentation.
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;

  // See interface for documentation.
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;

private:
  fapi::slot_message_gateway& gateway;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_TO_FAPI_TRANSLATOR_H
