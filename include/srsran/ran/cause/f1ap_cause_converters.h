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

#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/cause/ngap_cause.h"

namespace srsran {

/// \brief Converts an F1AP cause to an NGAP cause.
ngap_cause_t f1ap_to_ngap_cause(f1ap_cause_t f1ap_cause);

} // namespace srsran
