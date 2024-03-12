/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "e1ap_cause.h"
#include "f1ap_cause.h"
#include "ngap_cause.h"

namespace srsran {

/// \brief Converts an NGAP cause to an F1AP cause.
f1ap_cause_t ngap_to_f1ap_cause(ngap_cause_t ngap_cause);

/// \brief Converts an NGAP cause to an E1AP cause.
e1ap_cause_t ngap_to_e1ap_cause(ngap_cause_t ngap_cause);

} // namespace srsran
