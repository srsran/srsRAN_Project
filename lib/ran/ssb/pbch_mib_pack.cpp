/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/ssb/pbch_mib_pack.h"

using namespace srsran;

uint32_t srsran::pbch_mib_pack(const pbch_mib_message& msg, unsigned L_max)
{
  uint32_t payload = 0;

  // CHOICE in BCCH-BCH-MessageType - 1 bit.
  // Set to zero.

  // Pack msg.

  // systemFrameNumber - 6 bits MSB.
  payload |= (((msg.sfn >> 4U) & 0x3fU) << 25U);

  // subCarrierSpacingCommon - 1 bit.
  bool scs_flag = (msg.scs_common == subcarrier_spacing::kHz30 || msg.scs_common == subcarrier_spacing::kHz120);
  payload |= (scs_flag << 24U);

  // ssb-SubcarrierOffset - 4 bits.
  payload |= ((msg.subcarrier_offset.value() & 0xfU) << 20U);

  // dmrs-TypeA-Position - 1 bit.
  payload |= ((static_cast<std::underlying_type_t<dmrs_typeA_position>>(msg.dmrs_typeA_pos) & 0x1U) << 19U);

  // pdcch-ConfigSIB1 - 8 bits.
  payload |= ((msg.pdcch_config_sib1 & 0xffU) << 11U);

  // Barred - 1 bit.
  payload |= ((msg.cell_barred ? 0U : 1U) << 10U);

  // intraFreqReselection - 1 bit.
  payload |= ((msg.intrafreq_reselection ? 0U : 1U) << 9U);

  // Spare - 1 bit.
  // Leave to zero.

  // Add the SFN. - 4 bit LSB.
  payload |= ((msg.sfn & 0xfU) << 4U);

  // Half radio frame bit.
  payload |= (msg.hrf << 3U);

  if (L_max == 64) {
    // Pack the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((msg.ssb_block_index >> 3U) & 7U);
  } else {
    // 3rd LSB set to MSB of the SSB subcarrier offset. 2nd and 1st bits are reserved.
    payload |= (((msg.subcarrier_offset.value() >> 4U) & 1U) << 2U);
  }

  return payload;
}
