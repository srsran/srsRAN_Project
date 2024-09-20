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

#include "srsran/asn1/f1ap/f1ap.h"

namespace srsran {

/// F1AP message definition which is transferred between a CU-CP and a DU.
struct f1ap_message {
  asn1::f1ap::f1ap_pdu_c pdu;
};

} // namespace srsran
