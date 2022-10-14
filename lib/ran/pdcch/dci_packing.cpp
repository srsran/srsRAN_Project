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

// Computes the number of information bits before padding for a DCI format 0_0 message. This is used to perform DCI size
// alignment and to determine if the UL/SUL field is included in the size-aligned DCI format 0_0 payload.
static unsigned dci_f0_0_bits_before_padding(unsigned N_rb_ul_bwp)

{
  constexpr unsigned dci_format_id_nof_bits            = 1;
  constexpr unsigned time_resource_nof_bits            = 4;
  constexpr unsigned frequency_hopping_flag_nof_bits   = 1;
  constexpr unsigned modulation_coding_scheme_nof_bits = 5;
  constexpr unsigned new_data_indicator_nof_bits       = 1;
  constexpr unsigned redundancy_version_nof_bits       = 2;
  constexpr unsigned harq_process_number_nof_bits      = 4;
  constexpr unsigned tpc_command_nof_bits              = 2;

  // Contribution to the DCI payload size that is fixed.
  constexpr unsigned dci_fixed_nof_bits = dci_format_id_nof_bits + time_resource_nof_bits +
                                          frequency_hopping_flag_nof_bits + modulation_coding_scheme_nof_bits +
                                          new_data_indicator_nof_bits + redundancy_version_nof_bits +
                                          harq_process_number_nof_bits + tpc_command_nof_bits;

  // Frequency resource allocation field size. the frequency hopping offset is packed using the MSB bits of this field,
  // therefore, the payload size is not altered by the use of frequency hopping. See TS38.212 Section 7.3.1.1.1.
  unsigned dci_variable_nof_bits = log2_ceil(N_rb_ul_bwp * (N_rb_ul_bwp + 1) / 2);

  return dci_fixed_nof_bits + dci_variable_nof_bits;
}

// Computes the number of information bits before padding for a DCI format 1_0 message. This is used to perform DCI size
// alignment and to determine if the UL/SUL field is included in the size-aligned DCI format 0_0 payload.
static unsigned dci_f1_0_bits_before_padding(unsigned N_rb_dl_bwp)
{
  // Contribution to the DCI payload size that is fixed. It is the same number of bits for all format 1_1 variants.
  constexpr unsigned dci_fixed_nof_bits = 28U;

  // Frequency resource allocation field size.
  unsigned dci_variable_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);

  return dci_fixed_nof_bits + dci_variable_nof_bits;
}

dci_sizes srsgnb::get_dci_sizes(const dci_config& config)
{
  dci_sizes sizes = {};

  // TODO (joaquim): improve asserts.
  srsgnb_assert(config.N_rb_dl_bwp_initial && config.N_rb_dl_bwp_active && config.N_rb_ul_bwp_initial &&
                    config.N_rb_ul_bwp_active,
                "Invalid DCI common config parameters.");

  // Step 0
  // - Determine DCI format 0_0 monitored in a common search space according to clause 7.3.1.1.1 where N_UL_BWP_RB is
  // given by the size of the initial UL bandwidth part.
  unsigned format0_0_info_bits_common = dci_f0_0_bits_before_padding(config.N_rb_ul_bwp_initial);

  // - Determine DCI format 1_0 monitored in a common search space according to clause 7.3.1.2.1 where N_DL_BWP_RB given
  // by:
  //   - the size of CORESET 0 if CORESET 0 is configured for the cell; and
  //   - the size of initial DL bandwidth part if CORESET 0 is not configured for the cell.
  unsigned format1_0_info_bits_common = dci_f1_0_bits_before_padding(
      static_cast<bool>(config.coreset0_bw) ? config.coreset0_bw : config.N_rb_dl_bwp_initial);

  sizes.format0_0_common_size = format0_0_info_bits_common;
  sizes.format1_0_common_size = format1_0_info_bits_common;

  // - If DCI format 0_0 is monitored in common search space and if the number of information bits in the DCI format 0_0
  // prior to padding is less than the payload size of the DCI format 1_0 monitored in common search space for
  // scheduling the same serving cell, a number of zero padding bits are generated for the DCI format 0_0 until the
  // payload size equals that of the DCI format 1_0.
  if (format0_0_info_bits_common < format1_0_info_bits_common) {
    // The number of padding bits is computed here, without taking the optional single bit UL/SUL field into account.
    // This field is located after the padding, and it must only be included if the format 1_0 payload has a larger
    // amount of bits before the padding bits. Therefore, the UL/SUL can be though of as a field that takes the space of
    // the last padding bit within the format 0_0 payload, if present. See TS38.212 Sections 7.3.1.0 and 7.3.1.1.1.
    unsigned padding_bits_incl_ul_sul = format1_0_info_bits_common - format0_0_info_bits_common;
    sizes.format0_0_common_size += padding_bits_incl_ul_sul;
  }

  // - If DCI format 0_0 is monitored in common search space and if the number of information bits in the DCI format 0_0
  // prior to truncation is larger than the payload size of the DCI format 1_0 monitored in common search space for
  // scheduling the same serving cell, the bitwidth of the frequency domain resource assignment field in the DCI format
  // 0_0 is reduced by truncating the first few most significant bits such that the size of DCI format 0_0 equals the
  // size of the DCI format 1_0.
  if (format0_0_info_bits_common > format1_0_info_bits_common) {
    unsigned nof_truncated_bits = format0_0_info_bits_common - format1_0_info_bits_common;
    sizes.format0_0_common_size -= nof_truncated_bits;
  }

  srsgnb_assert(sizes.format1_0_common_size == sizes.format0_0_common_size, "DCI format 0_0 and 1_0 sizes must match");

  // Step 1
  // - Determine DCI format 0_0 monitored in a UE-specific search space according to clause 7.3.1.1.1 where N_UL_BWP_RB
  // is the size of the active UL bandwidth part.
  unsigned format0_0_info_bits_ue = dci_f0_0_bits_before_padding(config.N_rb_ul_bwp_active);

  // - Determine DCI format 1_0 monitored in a UE-specific search space according to clause 7.3.1.2.1 where N_DL_BWP_RB
  // is the size of the active DL bandwidth part.
  unsigned format1_0_info_bits_ue = dci_f1_0_bits_before_padding(config.N_rb_dl_bwp_active);

  sizes.format0_0_ue_specific_size = format0_0_info_bits_ue;
  sizes.format1_0_ue_specific_size = format1_0_info_bits_ue;

  // - For a UE configured with supplementaryUplink in ServingCellConfig in a cell, if PUSCH is configured to be
  // transmitted on both the SUL and the non-SUL of the cell and if the number of information bits in DCI format 0_0 in
  // UE-specific search space for the SUL is not equal to the number of information bits in DCI format 0_0 in
  // UE-specific search space for the non-SUL, a number of zero padding bits are generated for the smaller DCI format
  // 0_0 until the payload size equals that of the larger DCI format 0_0.

  // Not implemented.

  // - If DCI format 0_0 is monitored in UE-specific search space and if the number of information bits in the DCI
  // format 0_0 prior to padding is less than the payload size of the DCI format 1_0 monitored in UE-specific search
  // space for scheduling the same serving cell, a number of zero padding bits are generated for the DCI format 0_0
  // until the payload size equals that of the DCI format 1_0.
  if (format0_0_info_bits_ue < format1_0_info_bits_ue) {
    unsigned nof_padding_bits_incl_ul_sul = format1_0_info_bits_ue - format0_0_info_bits_ue;
    sizes.format0_0_ue_specific_size += nof_padding_bits_incl_ul_sul;
  }

  // - If DCI format 1_0 is monitored in UE-specific search space and if the number of information bits in the DCI
  // format 1_0 prior to padding is less than the payload size of the DCI format 0_0 monitored in UE-specific search
  // space for scheduling the same serving cell, zeros shall be appended to the DCI format 1_0 until the payload size
  // equals that of the DCI format 0_0
  if (format1_0_info_bits_ue < format0_0_info_bits_ue) {
    unsigned nof_padding_bits = format0_0_info_bits_ue - format1_0_info_bits_ue;
    sizes.format1_0_ue_specific_size += nof_padding_bits;
  }

  srsgnb_assert(sizes.format1_0_ue_specific_size == sizes.format0_0_ue_specific_size,
                "DCI format 0_0 and 1_0 sizes must match");

  return sizes;
}

dci_payload srsgnb::dci_0_0_c_rnti_pack(const dci_0_0_c_rnti_configuration& config)
{
  dci_payload payload;
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2);

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  if (config.frequency_hopping_flag) {
    // Assert that the number of bits used to pack the frequency hopping offset is valid.
    srsgnb_assert((config.N_ul_hop == 1) || (config.N_ul_hop == 2),
                  "DCI frequency offset number of bits must be either 1 or 2");

    // Assert that the frequency hopping offset index can be packed with the allocated bits.
    srsgnb_assert(config.hopping_offset < (1U << config.N_ul_hop),
                  "DCI frequency offset value ({}) cannot be packed with the allocated number of bits ({})",
                  config.hopping_offset,
                  config.N_ul_hop);

    // Truncate the frequency resource allocation field.
    frequency_resource_nof_bits -= config.N_ul_hop;

    // Frequency hopping offset - N_ul_hop bits.
    payload.push_back(config.hopping_offset, config.N_ul_hop);
  }

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

  unsigned nof_bits_before_padding = dci_f0_0_bits_before_padding(config.N_rb_ul_bwp);
  srsgnb_assert(nof_bits_before_padding <= config.payload_size,
                "DCI payload size must be able to fit the information bits");

  // Number of padding bits, including the UL/SUL optional field, if present.
  unsigned padding_incl_ul_sul = config.payload_size - nof_bits_before_padding;

  if (config.ul_sul_indicator.has_value() && (padding_incl_ul_sul > 0)) {
    // UL/SUL field is included if it is present in the DCI message and the number of DCI format 1_0 bits before padding
    // is larger than the number of DCI format 0_0 bits before padding.
    constexpr unsigned nof_ul_sul_bit = 1U;
    // Padding bits, if necessary, as per TS38.212 Section 7.3.1.0.
    payload.push_back(0X00U, padding_incl_ul_sul - nof_ul_sul_bit);

    // UL/SUL indicator - 1 bit.
    payload.push_back(config.ul_sul_indicator.value(), nof_ul_sul_bit);
  } else {
    // UL/SUL field is not included otherwise.
    payload.push_back(0X00U, padding_incl_ul_sul);
  }

  return payload;
}

dci_payload srsgnb::dci_0_0_tc_rnti_pack(const dci_0_0_tc_rnti_configuration& config)
{
  unsigned    frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2);
  dci_payload payload;

  // Identifier for DCI formats - 1 bit.
  payload.push_back(config.dci_format_id, 1);

  if (config.frequency_hopping_flag) {
    // Assert that the number of bits used to pack the frequency hopping offset is valid.
    srsgnb_assert((config.N_ul_hop == 1) || (config.N_ul_hop == 2),
                  "DCI frequency offset number of bits must be either 1 or 2");

    // Assert that the frequency hopping offset index can be packed with the allocated bits.
    srsgnb_assert(config.hopping_offset < (1U << config.N_ul_hop),
                  "DCI frequency offset value ({}) cannot be packed with the allocated number of bits ({})",
                  config.hopping_offset,
                  config.N_ul_hop);

    // Truncate the frequency resource allocation field.
    frequency_resource_nof_bits -= config.N_ul_hop;

    // Frequency hopping offset - N_ul_hop bits.
    payload.push_back(config.hopping_offset, config.N_ul_hop);
  }

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

  // UL/SUL indicator - 1 bit if present, reserved. The PUSCH allocation is always on the same UL carrier as the
  // previous transmission of the same TB, as per TS38.212 Section 7.3.1.1.1.
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

  int nof_padding_bits = config.payload_size - dci_f1_0_bits_before_padding(config.N_rb_dl_bwp);

  // Truncation is not allowed for DCI format 1_0.
  srsgnb_assert(nof_padding_bits >= 0, "Truncation is not allowed for DCI format 1_0");

  // Padding - nof_padding_bits bits.
  payload.push_back(0x00U, nof_padding_bits);

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

  // Since DCI format 1_0 scrambled by P-RNTI is used in common search space, no padding or truncation are applied.
  srsgnb_assert(config.payload_size == dci_f1_0_bits_before_padding(config.N_rb_dl_bwp),
                "padding or truncation are not allowed for DCI format 1_0 scrambled by P-RNTI");

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

  // Since DCI format 1_0 scrambled by SI-RNTI is used in common search space, no padding or truncation are applied.
  srsgnb_assert(config.payload_size == dci_f1_0_bits_before_padding(config.N_rb_dl_bwp),
                "padding or truncation are not allowed for DCI format 1_0 scrambled by SI-RNTI");

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

  // Since DCI format 1_0 scrambled by RA-RNTI is used in common search space, no padding or truncation are applied.
  srsgnb_assert(config.payload_size == dci_f1_0_bits_before_padding(config.N_rb_dl_bwp),
                "padding or truncation are not allowed for DCI format 1_0 scrambled by RA-RNTI");

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

  // Since DCI format 1_0 scrambled by TC-RNTI is used in common search space, no padding or truncation are applied.
  srsgnb_assert(config.payload_size == dci_f1_0_bits_before_padding(config.N_rb_dl_bwp),
                "padding or truncation are not allowed for DCI format 1_0 scrambled by TC-RNTI");

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
