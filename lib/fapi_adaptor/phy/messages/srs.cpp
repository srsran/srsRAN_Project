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

#include "srsran/fapi_adaptor/phy/messages/srs.h"

using namespace srsran;
using namespace fapi_adaptor;

void srsran::fapi_adaptor::convert_srs_fapi_to_phy(uplink_processor::srs_pdu& pdu,
                                                   const fapi::ul_srs_pdu&    fapi_pdu,
                                                   uint16_t                   sfn,
                                                   uint16_t                   slot)
{
  // Fill main context fields.
  ul_srs_context& context = pdu.context;
  context.slot            = slot_point(fapi_pdu.scs, sfn, slot);
  context.rnti            = fapi_pdu.rnti;

  // Fill SRS resource configuration.
  pdu.config.slot = slot_point(fapi_pdu.scs, sfn, slot);
  pdu.config.resource.nof_antenna_ports =
      static_cast<srs_resource_configuration::one_two_four_enum>(fapi_pdu.num_ant_ports);
  pdu.config.resource.nof_symbols  = static_cast<srs_resource_configuration::one_two_four_enum>(fapi_pdu.num_symbols);
  pdu.config.resource.start_symbol = fapi_pdu.time_start_position;
  pdu.config.resource.configuration_index = fapi_pdu.config_index;
  pdu.config.resource.sequence_id         = fapi_pdu.sequence_id;
  pdu.config.resource.bandwidth_index     = fapi_pdu.bandwidth_index;
  pdu.config.resource.comb_size           = static_cast<srs_resource_configuration::comb_size_enum>(fapi_pdu.comb_size);
  pdu.config.resource.comb_offset         = fapi_pdu.comb_offset;
  pdu.config.resource.cyclic_shift        = fapi_pdu.cyclic_shift;
  pdu.config.resource.freq_position       = fapi_pdu.frequency_position;
  pdu.config.resource.freq_shift          = fapi_pdu.frequency_shift;
  pdu.config.resource.freq_hopping        = fapi_pdu.frequency_hopping;
  pdu.config.resource.hopping =
      static_cast<srs_resource_configuration::group_or_sequence_hopping_enum>(fapi_pdu.group_or_sequence_hopping);
  // SRS frequency hopping not currently supported.

  // GCC gives a false positive of type Werror=stringop-overflow.
  // Fill the antenna port indices starting from 0.
  static_vector<unsigned, srs_constants::max_nof_rx_ports> ports(static_cast<unsigned>(fapi_pdu.num_ant_ports));
  std::iota(ports.begin(), ports.end(), 0);
  pdu.config.ports.assign(ports.begin(), ports.end());
}
