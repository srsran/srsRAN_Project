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

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// This interface represents the NR-U entry point of a F1-U entity.
/// The upper-layer transport (GTP-U) will use this class to pass NR-U PDUs into F1-U.
class f1u_du_upper_transport_interface
{
public:
  f1u_du_upper_transport_interface()                                                    = default;
  virtual ~f1u_du_upper_transport_interface()                                           = default;
  f1u_du_upper_transport_interface(const f1u_du_upper_transport_interface&)             = delete;
  f1u_du_upper_transport_interface& operator=(const f1u_du_upper_transport_interface&)  = delete;
  f1u_du_upper_transport_interface(const f1u_du_upper_transport_interface&&)            = delete;
  f1u_du_upper_transport_interface& operator=(const f1u_du_upper_transport_interface&&) = delete;

  virtual void handle_nr_u_pdu(byte_buffer pdu) = 0;
};

/// This interface represents the NR-U exit point of a F1-U entity.
/// The F1-U entity will use this class to pass NR-U PDUs to upper-layer transport (GTP-U).
class f1u_du_upper_transport_notifier
{
public:
  f1u_du_upper_transport_notifier()                                                   = default;
  virtual ~f1u_du_upper_transport_notifier()                                          = default;
  f1u_du_upper_transport_notifier(const f1u_du_upper_transport_notifier&)             = delete;
  f1u_du_upper_transport_notifier& operator=(const f1u_du_upper_transport_notifier&)  = delete;
  f1u_du_upper_transport_notifier(const f1u_du_upper_transport_notifier&&)            = delete;
  f1u_du_upper_transport_notifier& operator=(const f1u_du_upper_transport_notifier&&) = delete;

  virtual void on_new_nr_u_pdu(byte_buffer pdu) = 0;
};

} // namespace srsgnb
