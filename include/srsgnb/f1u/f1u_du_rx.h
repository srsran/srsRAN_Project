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

#include "srsgnb/adt/byte_buffer_slice_chain.h"

namespace srsgnb {

/// This interface represents the data entry point of the receiving side of a F1-U entity.
/// The lower-layers will use this class to pass PDCP PDUs into the PDCP.
class f1u_du_rx_lower_data_interface
{
public:
  f1u_du_rx_lower_data_interface()                                                  = default;
  virtual ~f1u_du_rx_lower_data_interface()                                         = default;
  f1u_du_rx_lower_data_interface(const f1u_du_rx_lower_data_interface&)             = delete;
  f1u_du_rx_lower_data_interface& operator=(const f1u_du_rx_lower_data_interface&)  = delete;
  f1u_du_rx_lower_data_interface(const f1u_du_rx_lower_data_interface&&)            = delete;
  f1u_du_rx_lower_data_interface& operator=(const f1u_du_rx_lower_data_interface&&) = delete;

  virtual void handle_pdcp_pdu(byte_buffer_slice_chain pdu) = 0;
};

} // namespace srsgnb
