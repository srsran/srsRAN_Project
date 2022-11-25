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

#include "srsgnb/sdap/sdap.h"
#include "srsgnb/sdap/sdap_packet_procedures.h"
#include <cstdio>

namespace srsgnb {

namespace srs_cu_up {

class sdap_procedure_dispatcher : public sdap_pdu_handler
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.
  std::unique_ptr<sdap_packet_procedures> procedure;

public:
  explicit sdap_procedure_dispatcher(std::unique_ptr<sdap_packet_procedures> procedure_) :
    procedure(std::move(procedure_))
  {
  }

  void handle_pdu(byte_buffer data) override
  {
    std::printf("[SDAP-PROCEDURE-DISPATCHER] Received a new data packet of size = %u bytes. Dispatching an UL packet "
                "data procedure to handle it\n",
                (unsigned)data.length());

    procedure->handle_ul_data_packet(data);
  }
};

} // namespace srs_cu_up

} // namespace srsgnb