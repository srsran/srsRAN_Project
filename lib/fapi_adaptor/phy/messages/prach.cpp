/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/prach.h"
#include "srsgnb/phy/support/prach_buffer_context.h"

using namespace srsgnb;
using namespace fapi_adaptor;

static preamble_format convert_fapi_format_to_phy(fapi::prach_format_type format)
{
  switch (format) {
    case fapi::prach_format_type::zero:
      return preamble_format::values::FORMAT0;
    case fapi::prach_format_type::one:
      return preamble_format::values::FORMAT1;
    case fapi::prach_format_type::two:
      return preamble_format::values::FORMAT2;
    case fapi::prach_format_type::three:
      return preamble_format::values::FORMAT3;
    default:
      srsgnb_assert(0, "Invalid PRACH format type ({})", static_cast<unsigned>(format));
      break;
  }

  return preamble_format::values::FORMAT1;
}

void srsgnb::fapi_adaptor::convert_prach_fapi_to_phy(prach_buffer_context&           context,
                                                     const fapi::ul_prach_pdu&       fapi_pdu,
                                                     const fapi::multi_prach_config& prach_config_tlv)
{
  context.format                = convert_fapi_format_to_phy(fapi_pdu.prach_format);
  context.start_symbol          = fapi_pdu.prach_start_symbol;
  context.zero_correlation_zone = fapi_pdu.num_cs;
  context.start_preamble_index  = fapi_pdu.maintenance_v3.start_preamble_index;
  context.nof_preamble_indices  = fapi_pdu.maintenance_v3.num_preamble_indices;

  const fapi::prach_config& config = prach_config_tlv.prachs[fapi_pdu.maintenance_v3.prach_res_config_index];
  context.pusch_scs                = config.prach_ul_bwp_pusch_scs;

  const fapi::prach_fd_occasion_config& fd_occas = config.fd_occasions.back();
  context.rb_offset                              = fd_occas.prach_freq_offset;
  context.restricted_set                         = config.restricted_set;
  context.root_sequence_index                    = fd_occas.prach_root_sequence_index;

  // NOTE: set the port to 0 for now.
  context.port = 0;
}
