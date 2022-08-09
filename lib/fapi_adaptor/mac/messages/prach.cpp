/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/prach.h"
#include "srsgnb/mac/mac_cell_result.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu& fapi_pdu, const prach_occasion_info& mac_pdu)
{
  ul_prach_pdu_builder builder(fapi_pdu);

  convert_prach_mac_to_fapi(builder, mac_pdu);
}

static prach_format_type convert_to_fapi_prach_format(preamble_format format)
{
  switch (format) {
    case preamble_format::values::FORMAT0:
      return prach_format_type::zero;
    case preamble_format::values::FORMAT1:
      return prach_format_type::one;
    case preamble_format::values::FORMAT2:
      return prach_format_type::two;
    case preamble_format::values::FORMAT3:
      return prach_format_type::three;
    default:
      srsgnb_assert(0, "Invalid preamble format {{}}", static_cast<unsigned>(format));
      break;
  }
  return prach_format_type::zero;
}

void srsgnb::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder,
                                                     const prach_occasion_info&  mac_pdu)
{
  builder.set_basic_parameters(mac_pdu.pci,
                               mac_pdu.nof_prach_occasions,
                               convert_to_fapi_prach_format(mac_pdu.format),
                               mac_pdu.index_fd_ra,
                               mac_pdu.start_symbol,
                               mac_pdu.nof_cs);

  // NOTE: Parameter not used for now, setting value to 0.
  static constexpr unsigned handle = 0;
  // NOTE: As only one configuration is supported, the prach index is set to 0.
  static constexpr unsigned prach_config_index = 0;
  // NOTE: Only supporting PHY context.
  static constexpr prach_config_scope_type context = prach_config_scope_type::phy_context;
  builder.set_maintenance_v3_basic_parameters(handle,
                                              context,
                                              prach_config_index,
                                              mac_pdu.nof_fd_ra,
                                              mac_pdu.start_preamble_index,
                                              mac_pdu.nof_preamble_indexes);
}
