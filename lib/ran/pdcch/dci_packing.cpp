/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/pdcch/dci_packing.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

dci_payload dci_0_0_c_rnti_pack(const dci_0_0_c_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2);
  dci_payload payload;

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  payload.push_back(config.time_resource, 4);

  // Frequency hopping flag - 1 bit.
  payload.push_back(config.frequency_hopping_flag, 1);

  // Modulation coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  payload.push_back(config.new_data_indicator, 1);

  // Redundancy version - 2 bit.
  payload.push_back(config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  payload.push_back(config.harq_process_number, 4);

  // TPC command for scheduled PUSCH - 2 bit.
  payload.push_back(config.tpc_command, 2);

  // Padding bits, if necessary, as per TS38.212 Section 7.3.1.0.

  // UL/SUL indicator - 1 bit if present.
  payload.push_back(config.ul_sul_indicator, 1);

  return payload;
}

dci_payload dci_0_0_tc_rnti_pack(const dci_0_0_tc_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2);
  dci_payload payload;

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  payload.push_back(config.time_resource, 4);

  // Frequency hopping flag - 1 bit.
  payload.push_back(config.frequency_hopping_flag, 1);

  // Modulation coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit, reserved.
  payload.push_back(0x00U, 1);

  // Redundancy version - 2 bit.
  payload.push_back(config.redundancy_version, 2);

  // HARQ process number - 4 bit, reserved.
  payload.push_back(0x00U, 4);

  // TPC command for scheduled PUSCH - 2 bit.
  payload.push_back(config.tpc_command, 2);

  // Padding bits, if necessary, as per TS38.212 Section 7.3.1.0.

  // UL/SUL indicator - 1 bit if present, reserved.
  payload.push_back(0x00U, 1);

  return payload;
}

dci_payload srsgnb::dci_1_0_c_rnti_pack(const dci_1_0_c_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  dci_payload payload;

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  payload.push_back(config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  payload.push_back(config.vrb_to_prb_mapping, 1);

  // Modulation coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  payload.push_back(config.new_data_indicator, 1);

  // Redundancy version - 2 bit.
  payload.push_back(config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  payload.push_back(config.harq_process_number, 4);

  // Downlink assignment index - 2 bit.
  payload.push_back(config.dl_assignment_index, 2);

  // TPC command for scheduled PUCCH - 2 bit.
  payload.push_back(config.tpc_command, 2);

  // PUCCH resource indicator - 3 bit.
  payload.push_back(config.pucch_resource_indicator, 3);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  payload.push_back(config.pdsch_harq_fb_timing_indicator, 3);

  return payload;
}

dci_payload srsgnb::dci_1_0_p_rnti_pack(const dci_1_0_p_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  dci_payload payload;

  // Short Message Indicator - 2 bits.
  switch (config.short_messages_indicator) {
    case dci_1_0_p_rnti_configuration::payload_info::scheduling_information:
      payload.push_back(0b01U, 2);
      break;
    case dci_1_0_p_rnti_configuration::payload_info::short_messages:
      payload.push_back(0b10U, 2);
      break;
    case dci_1_0_p_rnti_configuration::payload_info::both:
      payload.push_back(0b11U, 2);
      break;
  }

  // Short Messages - 8 bits.
  if (config.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::scheduling_information) {
    // If only the scheduling information for paging is carried, this bit field is reserved.
    payload.push_back(0x00U, 8);
  } else {
    payload.push_back(config.short_messages, 8);
  }

  if (config.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::short_messages) {
    // If only the short message is carried, the scheduling information for paging bit fields are reserved.
    payload.push_back(0x00U, frequency_resource_nof_bits + 12);
  } else {
    // Frequency domain resource assignment - frequency_resource_nof_bits bits.
    payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

    // Time domain resource assignment - 4 bits.
    payload.push_back(config.time_resource, 4);

    // VRB-to-PRB mapping - 1 bit.
    payload.push_back(config.vrb_to_prb_mapping, 1);

    // Modulation and coding scheme - 5 bits.
    payload.push_back(config.modulation_coding_scheme, 5);

    // Transport Block scaling - 2 bits.
    payload.push_back(config.tb_scaling, 2);
  }

  // Reserved bits: 6 bits.
  payload.push_back(0x00U, 6);

  return payload;
}

dci_payload srsgnb::dci_1_0_si_rnti_pack(const dci_1_0_si_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  dci_payload payload;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  payload.push_back(config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  payload.push_back(config.vrb_to_prb_mapping, 1);

  // Modulation coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // Redundancy version - 2 bits.
  payload.push_back(config.redundancy_version, 2);

  // System information indicator - 1 bit.
  payload.push_back(config.system_information_indicator, 1);

  // Reserved bits - 15 bit.
  payload.push_back(0x00U, 15);

  return payload;
}

dci_payload srsgnb::dci_1_0_ra_rnti_pack(const dci_1_0_ra_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  dci_payload payload;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bits.
  payload.push_back(config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  payload.push_back(config.vrb_to_prb_mapping, 1);

  // Modulation and coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // Transport Block scaling - 2 bits.
  payload.push_back(config.tb_scaling, 2);

  // Reserved bits - 16 bits.
  payload.push_back(0x00U, 16);

  return payload;
}

dci_payload srsgnb::dci_1_0_tc_rnti_pack(const dci_1_0_tc_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  dci_payload payload;

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  payload.push_back(config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  payload.push_back(config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  payload.push_back(config.vrb_to_prb_mapping, 1);

  // Modulation coding scheme - 5 bits.
  payload.push_back(config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  payload.push_back(config.new_data_indicator, 1);

  // Redundancy version - 2 bit.
  payload.push_back(config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  payload.push_back(config.harq_process_number, 4);

  // Downlink assignment index - 2 bit, reserved.
  payload.push_back(0x00U, 2);

  // TPC command for scheduled PUCCH - 2 bit.
  payload.push_back(config.tpc_command, 2);

  // PUCCH resource indicator - 3 bit.
  payload.push_back(config.pucch_resource_indicator, 3);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  payload.push_back(config.pdsch_harq_fb_timing_indicator, 3);

  return payload;
}

dci_payload srsgnb::dci_rar_pack(const dci_rar_configuration& config)
{
  dci_payload payload;

  // Frequency hopping flag - 1 bit.
  payload.push_back(config.frequency_hopping_flag, 1);

  // PUSCH frequency resource allocation - 14 bits.
  payload.push_back(config.frequency_resource, 14);

  // PUSCH time resource allocation - 4 bits.
  payload.push_back(config.time_resource, 4);

  // Modulation and coding scheme - 4 bits.
  payload.push_back(config.modulation_coding_scheme, 4);

  // TPC command for PUSCH - 3 bits.
  payload.push_back(config.tpc, 3);

  // CSI request - 1 bit.
  payload.push_back(config.csi_request, 1);

  return payload;
}
