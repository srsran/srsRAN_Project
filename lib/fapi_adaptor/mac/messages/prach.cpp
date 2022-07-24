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
      srsran_assert(0, "Invalid preamble format {{}}", static_cast<unsigned>(format));
      break;
  }
  return prach_format_type::zero;
}

void srsgnb::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder,
                                                     const prach_occasion_info&  mac_pdu)
{
  // PCI not provided, passing 0.
  static constexpr pci_t pci = 0;
  builder.set_basic_parameters(pci,
                               mac_pdu.nof_prach_occasions,
                               convert_to_fapi_prach_format(mac_pdu.format),
                               mac_pdu.fd_ra_resources.start(),
                               mac_pdu.start_symbol,
                               mac_pdu.nof_cs);

  // :TODO: what to to with the handle.
  static constexpr unsigned handle = 0;
  builder.set_maintenance_v3_basic_parameters(handle,
                                              prach_config_scope_type::common_context,
                                              mac_pdu.prach_config_index,
                                              mac_pdu.fd_ra_resources.length(),
                                              mac_pdu.start_preamble_index,
                                              mac_pdu.nof_preamble_indexes);
}
