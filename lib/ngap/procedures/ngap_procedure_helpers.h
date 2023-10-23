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

#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

inline void
handle_nas_pdu(srslog::basic_logger& logger, byte_buffer nas_pdu, ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier)
{
  logger.debug("Forwarding NAS PDU to RRC");
  rrc_ue_pdu_notifier.on_new_pdu(std::move(nas_pdu));
}

} // namespace srs_cu_cp
} // namespace srsran
