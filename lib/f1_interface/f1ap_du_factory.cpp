/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1_interface/f1ap_du_factory.h"
#include "f1ap_du_impl.h"
#include "f1ap_packet_procedures_impl.h"
#include "srsgnb/du_manager/du_manager.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<f1_du_interface> srsgnb::create_f1ap_du(f1c_pdu_handler& f1c_pdu_handler_)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(f1c_pdu_handler_);
  return f1ap_du;
}
