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

#include "srsran/fapi_adaptor/mac/messages/prach.h"
#include "srsran/mac/mac_cell_result.h"

using namespace srsran;
using namespace fapi_adaptor;

void srsran::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu& fapi_pdu, const prach_occasion_info& mac_pdu)
{
  fapi::ul_prach_pdu_builder builder(fapi_pdu);

  convert_prach_mac_to_fapi(builder, mac_pdu);
}

void srsran::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder,
                                                     const prach_occasion_info&  mac_pdu)
{
  // NOTE: For long preambles the number of time-domain PRACH occasions parameter does not apply, so set it to 1 to be
  // compliant with the FAPI specification.
  unsigned nof_prach_occasions = is_long_preamble(mac_pdu.format) ? 1U : mac_pdu.nof_prach_occasions;

  builder.set_basic_parameters(
      mac_pdu.pci, nof_prach_occasions, mac_pdu.format, mac_pdu.index_fd_ra, mac_pdu.start_symbol, mac_pdu.nof_cs);

  // NOTE: Parameter not used for now, setting value to 0.
  static constexpr unsigned handle = 0;
  // NOTE: As only one configuration is supported, the prach res config index is set to 0.
  static constexpr unsigned prach_res_config_index = 0;
  // NOTE: Only supporting PHY context.
  static constexpr fapi::prach_config_scope_type context = fapi::prach_config_scope_type::phy_context;
  builder.set_maintenance_v3_basic_parameters(handle,
                                              context,
                                              prach_res_config_index,
                                              mac_pdu.nof_fd_ra,
                                              mac_pdu.start_preamble_index,
                                              mac_pdu.nof_preamble_indexes);
}
