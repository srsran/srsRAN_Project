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

#include "srsran/adt/byte_buffer_slice_chain.h"

namespace srsran {

/// This interface represents the data exit point of the receiving side of a F1-U entity.
/// The F1-U entity will use this class to pass PDCP PDUs into the PDCP.
class f1u_cu_up_rx_upper_data_notifier
{
public:
  f1u_cu_up_rx_upper_data_notifier()                                                    = default;
  virtual ~f1u_cu_up_rx_upper_data_notifier()                                           = default;
  f1u_cu_up_rx_upper_data_notifier(const f1u_cu_up_rx_upper_data_notifier&)             = delete;
  f1u_cu_up_rx_upper_data_notifier& operator=(const f1u_cu_up_rx_upper_data_notifier&)  = delete;
  f1u_cu_up_rx_upper_data_notifier(const f1u_cu_up_rx_upper_data_notifier&&)            = delete;
  f1u_cu_up_rx_upper_data_notifier& operator=(const f1u_cu_up_rx_upper_data_notifier&&) = delete;

  virtual void on_new_pdcp_pdu(byte_buffer_slice_chain pdu) = 0;
};

} // namespace srsran
