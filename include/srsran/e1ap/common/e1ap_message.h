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

#include "srsran/asn1/e1ap/e1ap.h"

namespace srsran {

/// \brief E1AP message transferred between a CU-CP and a CU-UP.
struct e1ap_message {
  asn1::e1ap::e1ap_pdu_c pdu;
};

} // namespace srsran