/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "nrppa_dummy_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_cu_cp;

nrppa_dummy_impl::nrppa_dummy_impl() : logger(srslog::fetch_basic_logger("NRPPA")) {}

// Note: For fwd declaration of member types, dtor cannot be trivial.
nrppa_dummy_impl::~nrppa_dummy_impl() {}

void nrppa_dummy_impl::remove_ue_context(ue_index_t ue_index) {}

void nrppa_dummy_impl::handle_new_nrppa_pdu(const byte_buffer&                    nrppa_pdu,
                                            std::variant<ue_index_t, amf_index_t> ue_or_amf_index)
{
  logger.info("NRPPa messages are not supported");
}
