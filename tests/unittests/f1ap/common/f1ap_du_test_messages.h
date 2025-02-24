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

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ran/rb_id.h"

namespace srsran {

/// \brief Generate dummy F1AP DL RRC Message Transfer message (CU -> DU).
f1ap_message generate_f1ap_dl_rrc_message_transfer(srb_id_t srb_id, const byte_buffer& rrc_container);

} // namespace srsran
