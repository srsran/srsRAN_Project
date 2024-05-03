/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// \brief Served cell configuration that will be passed to CU-CP.
struct f1_cell_setup_params {
  nr_cell_global_id_t             nr_cgi;
  pci_t                           pci;
  uint32_t                        tac;
  duplex_mode                     duplx_mode;
  subcarrier_spacing              scs_common;
  carrier_configuration           dl_carrier;
  optional<carrier_configuration> ul_carrier;
  byte_buffer                     packed_meas_time_cfg;
  byte_buffer                     packed_mib;
  byte_buffer                     packed_sib1;
};

/// \brief Message that initiates a F1 Setup procedure.
struct f1_setup_request_message {
  uint64_t                          gnb_du_id;
  std::string                       gnb_du_name;
  uint8_t                           rrc_version;
  std::vector<f1_cell_setup_params> served_cells;
  unsigned                          max_setup_retries = 5;
};

struct f1_setup_response_message {
  bool success = false;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Connect the DU to CU-CP via F1-C interface.
  virtual SRSRAN_NODISCARD bool connect_to_cu_cp() = 0;

  /// \brief Initiates the F1 Setup procedure as per TS 38.473, Section 8.2.3.
  /// \param[in] request The F1SetupRequest message to transmit.
  /// \return Returns a f1_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The DU transmits the F1SetupRequest as per TS 38.473 section 8.2.3
  /// and awaits the response. If a F1SetupFailure is received the F1AP will handle the failure.
  virtual async_task<f1_setup_response_message> handle_f1_setup_request(const f1_setup_request_message& request) = 0;

  /// \brief Launches the F1 Removal procedure as per TS 38.473, Section 8.2.8.
  virtual async_task<void> handle_f1_removal_request() = 0;
};

} // namespace srs_du
} // namespace srsran
