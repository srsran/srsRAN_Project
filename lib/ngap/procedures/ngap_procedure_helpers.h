/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/ue_manager.h" // for ngap_ue
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

inline void handle_nas_pdu(srslog::basic_logger& logger, const asn1::unbounded_octstring<true>& nas_pdu, ngap_ue& ue)
{
  logger.debug("Forwarding NAS PDU to RRC");
  byte_buffer rrc_nas_pdu;
  rrc_nas_pdu.resize(nas_pdu.size());
  std::copy(nas_pdu.begin(), nas_pdu.end(), rrc_nas_pdu.begin());
  ue.get_rrc_ue_pdu_notifier().on_new_pdu(std::move(rrc_nas_pdu));
}

} // namespace srs_cu_cp
} // namespace srsran
