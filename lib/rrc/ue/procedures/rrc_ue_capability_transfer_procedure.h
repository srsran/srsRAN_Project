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

#include "../rrc_ue_context.h"
#include "rrc_ue_event_manager.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of UE capabilities in the RRC UE.
class rrc_ue_capability_transfer_procedure
{
public:
  rrc_ue_capability_transfer_procedure(rrc_ue_context_t&                           context_,
                                       rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                                       rrc_ue_event_manager&                       ev_mng_,
                                       srslog::basic_logger&                       logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC UE Capability Transfer Procedure"; }

private:
  /// \remark Send RRC UE Capability Enquiry, see section 5.6.1.1 in TS 38.331
  void send_rrc_ue_capability_enquiry();

  rrc_ue_context_t& context;

  rrc_ue_security_mode_command_proc_notifier& rrc_ue;    // handler to the parent RRC UE object
  rrc_ue_event_manager&                       event_mng; // event manager for the RRC UE entity
  srslog::basic_logger&                       logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  const std::chrono::milliseconds rrc_ue_cap_timeout_ms{
      80}; // See 38.331 Sec. 12  arbitrary timeout for RRC UE Capability procedure
  bool procedure_result = false;
};

/// \brief Fills ASN.1 RRC UE Capability Enquiry struct.
/// \param[out] rrc_ue_cap_enquiry The RRC UE Capability Enquiry ASN.1 struct to fill.
/// \param[in] rrc_transaction_id The RRC transaction id.
inline void fill_asn1_rrc_ue_capability_enquiry(asn1::rrc_nr::ue_cap_enquiry_s& rrc_ue_cap_enquiry,
                                                uint8_t                         rrc_transaction_id,
                                                std::vector<nr_band>            bands)
{
  using namespace asn1::rrc_nr;
  ue_cap_enquiry_ies_s& ue_cap_ies      = rrc_ue_cap_enquiry.crit_exts.set_ue_cap_enquiry();
  rrc_ue_cap_enquiry.rrc_transaction_id = rrc_transaction_id;

  asn1::rrc_nr::ue_cap_rat_request_s ue_cap_rat_request;
  ue_cap_rat_request.rat_type = asn1::rrc_nr::rat_type_e::nr;

  ue_cap_request_filt_nr_s filter;
  for (const auto& band : bands) {
    freq_band_info_c band_info;
    band_info.set_band_info_nr().band_nr = nr_band_to_uint(band);
    filter.freq_band_list_filt.push_back(band_info);
  }

  byte_buffer   filter_packed;
  asn1::bit_ref bref(filter_packed);
  filter.pack(bref); // possible packing error fill be shown in ASN1 logs
  ue_cap_rat_request.cap_request_filt = std::move(filter_packed);

  ue_cap_ies.ue_cap_rat_request_list.push_back(ue_cap_rat_request);
}

} // namespace srs_cu_cp
} // namespace srsran
