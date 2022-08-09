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

void srsgnb::fapi_adaptor::convert_prach_fapi_to_phy(prach_buffer_context&     context,
                                                     const fapi::ul_prach_pdu& fapi_pdu,
                                                     const fapi::prach_config& prach_config_tlv,
                                                     unsigned                  sfn,
                                                     unsigned                  slot,
                                                     unsigned                  sector_id)
{
  srsgnb_assert(fapi_pdu.maintenance_v3.prach_config_scope == fapi::prach_config_scope_type::phy_context,
                "Common context not supported.");
  srsgnb_assert(fapi_pdu.maintenance_v3.prach_res_config_index == 0,
                "Only PRACH resource configuration index 0 supported.");
  srsgnb_assert(fapi_pdu.index_fd_ra == 0, "Only one FD occasion supported.");
  srsgnb_assert(fapi_pdu.num_prach_ocas == 1, "Only one PRACH occasion supported.");

  context.slot                 = slot_point(to_numerology_value(prach_config_tlv.prach_ul_bwp_pusch_scs), sfn, slot);
  context.sector               = sector_id;
  context.format               = convert_fapi_format_to_phy(fapi_pdu.prach_format);
  context.start_symbol         = fapi_pdu.prach_start_symbol;
  context.start_preamble_index = fapi_pdu.maintenance_v3.start_preamble_index;
  context.nof_preamble_indices = fapi_pdu.maintenance_v3.num_preamble_indices;

  context.pusch_scs      = prach_config_tlv.prach_ul_bwp_pusch_scs;
  context.restricted_set = prach_config_tlv.restricted_set;

  const fapi::prach_fd_occasion_config& fd_occas = prach_config_tlv.fd_occasions[fapi_pdu.index_fd_ra];
  context.rb_offset                              = fd_occas.prach_freq_offset;
  context.root_sequence_index                    = fd_occas.prach_root_sequence_index;
  context.zero_correlation_zone                  = fd_occas.prach_zero_corr_conf;

  // NOTE: set the port to 0 for now.
  context.port = 0;
}
