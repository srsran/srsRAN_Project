/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ssb_configuration_asn1_helpers.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

using namespace srsgnb;

void fill_ssb_configuration_from_asn1(ssb_configuration&                                 out,
                                      const asn1::rrc_nr::mib_s&                         mib_in,
                                      const asn1::rrc_nr::serving_cell_cfg_common_sib_s& sib_in,
                                      subcarrier_spacing                                 ssb_scs)
{
  // Parameters taken from ServingCellConfigCommonSIB
  out.ssb_power             = sib_in.ss_pbch_block_pwr;
  out.ssb_offset_to_point_A = sib_in.dl_cfg_common.freq_info_dl.offset_to_point_a;
  out.ssb_period            = (uint8_t)sib_in.ssb_periodicity_serving_cell;
  // SSB subcarrier is taken from MIB.ssb-SubcarrierOffset.
  out.ssb_subcarrier_offset = mib_in.ssb_subcarrier_offset;

  // The SSB subcarrierSpacing should be passed as input parameter.
  out.scs = ssb_scs;

  // Hard-coded for the time being. Should be taken either from
  // - ServingCellConfigCommonSIB.ssb-PositionsInBurst, or
  // - ServingCellConfigCommon.ssb-PositionsInBurst.
  // TODO: derive bitmap from correct fields.
  out.ssb_bitmap = static_cast<uint64_t>(0b1) << static_cast<uint64_t>(63U);

  // TODO: define which Beam ID should be used
  out.beam_ids[0] = 0;

  // This is not imported from RRC
  // TODO: define where to get this value
  out.beta_pss = 0;
}
