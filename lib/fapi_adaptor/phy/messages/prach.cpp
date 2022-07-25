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

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace fapi;

preamble_format convert_fapi_format_to_phy(prach_format_type format)
{
  switch (format) {
    case prach_format_type::zero:
      return preamble_format::values::FORMAT0;
    case prach_format_type::one:
      return preamble_format::values::FORMAT1;
    case prach_format_type::two:
      return preamble_format::values::FORMAT2;
    case prach_format_type::three:
      return preamble_format::values::FORMAT3;
    default:
      srsgnb_assert(0, "Invalid PRACH format type ({})", static_cast<unsigned>(format));
      break;
  }

  return preamble_format::values::OTHER;
}

void srsgnb::fapi_adaptor::convert_prach_fapi_to_phy(prach_detector::slot_configuration& detector_config,
                                                     const fapi::ul_prach_pdu&           fapi_pdu,
                                                     const fapi::multi_prach_config&     prach_config_tlv)
{
  detector_config.format = convert_fapi_format_to_phy(fapi_pdu.prach_format);

  const ul_prach_maintenance_v3& v3    = fapi_pdu.maintenance_v3;
  detector_config.start_preamble_index = v3.start_preamble_index;
  detector_config.nof_preamble_indices = v3.num_preamble_indices;

  srsgnb_assert(v3.prach_res_config_index < prach_config_tlv.prachs.size(),
                "Invalid index ({}) in multi-PRACH configuration TLV ({}).",
                v3.prach_res_config_index,
                prach_config_tlv.prachs.size());

  const prach_config& tlv = prach_config_tlv.prachs[v3.prach_res_config_index];

  detector_config.restricted_set = tlv.restricted_set;
  detector_config.pusch_scs      = tlv.prach_ul_bwp_pusch_scs;

  srsgnb_assert(fapi_pdu.index_fd_ra < tlv.fd_occasions.size(),
                "Invalid Fd occasion index ({}) (Number of occassions {})",
                fapi_pdu.index_fd_ra,
                tlv.fd_occasions.size());
  const prach_fd_occasion_config& fd_occassion = tlv.fd_occasions[fapi_pdu.index_fd_ra];
  detector_config.root_sequence_index          = fd_occassion.prach_root_sequence_index;
  detector_config.frequency_offset             = fd_occassion.prach_freq_offset;
  detector_config.zero_correlation_zone        = fd_occassion.prach_zero_corr_conf;
}
