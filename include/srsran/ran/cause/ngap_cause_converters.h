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

#include "srsran/ran/cause/e1ap_cause.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/cause/ngap_cause.h"

namespace srsran {

/// \brief Converts an NGAP cause to an F1AP cause.
f1ap_cause_t ngap_to_f1ap_cause(ngap_cause_t ngap_cause);

/// \brief Converts an NGAP cause to an E1AP cause.
e1ap_cause_t ngap_to_e1ap_cause(ngap_cause_t ngap_cause);

} // namespace srsran
