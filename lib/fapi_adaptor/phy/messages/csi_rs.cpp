/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/csi_rs.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;
using namespace fapi_adaptor;

/// Returns the scaling value applied to the bit position in the frequency allocation bitmap as per TS38.211
/// Section 7.4.1.5.3.
static unsigned get_bitpos_scale(unsigned row)
{
  if (row == 1) {
    return 1;
  }
  if (row == 2) {
    return 1;
  }
  if (row == 4) {
    return 4;
  }
  return 2;
}

/// Returns the number of bits in the frequency domain bitmap as per TS38.331 IE CSI-RS-ResourceMapping.
static unsigned get_bitmap_size(unsigned row)
{
  if (row == 1) {
    return 4;
  }
  if (row == 2) {
    return 12;
  }
  if (row == 4) {
    return 3;
  }
  return 6;
}

/// Converts a frequency domain bitmap to the corresponding k_n values.
static void
convert_freq_domain(const bounded_bitset<12>& src, static_vector<unsigned, CSI_RS_MAX_NOF_K_INDEXES>& dst, unsigned row)
{
  unsigned scale = get_bitpos_scale(row);
  unsigned size  = get_bitmap_size(row);
  for (unsigned i = size; i != 0; --i) {
    if (src.test(i - 1)) {
      dst.push_back(scale * (size - i));
    }
  }
}

/// Translates the \c nzp_csi_rs_epre_to_ssb enum to a linear amplitude value.
static float translate_amplitude(fapi::nzp_csi_rs_epre_to_ssb power)
{
  switch (power) {
    case fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3:
      return 0.5F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB0:
      return 1.F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB3:
      return 2.F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB6:
      return 4.F;
    case fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss:
    default:
      return 1.F;
  }
}

void srsgnb::fapi_adaptor::convert_csi_rs_fapi_to_phy(nzp_csi_rs_generator::config_t& proc_pdu,
                                                      const fapi::dl_csi_rs_pdu&      fapi_pdu,
                                                      uint16_t                        sfn,
                                                      uint16_t                        slot)
{
  proc_pdu.slot = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.cp   = fapi_pdu.cp;

  proc_pdu.start_rb                 = fapi_pdu.start_rb;
  proc_pdu.nof_rb                   = fapi_pdu.num_rbs;
  proc_pdu.csi_rs_mapping_table_row = fapi_pdu.row;
  convert_freq_domain(fapi_pdu.freq_domain, proc_pdu.freq_allocation_ref_idx, fapi_pdu.row);

  proc_pdu.symbol_l0     = fapi_pdu.symb_L0;
  proc_pdu.symbol_l1     = fapi_pdu.symb_L1;
  proc_pdu.cdm           = fapi_pdu.cdm_type;
  proc_pdu.freq_density  = fapi_pdu.freq_density;
  proc_pdu.scrambling_id = fapi_pdu.scramb_id;

  proc_pdu.amplitude = translate_amplitude(fapi_pdu.power_control_offset_ss_profile_nr);
  // Disable precoding.
  proc_pdu.pmi   = 0;
  proc_pdu.ports = {0};
}
