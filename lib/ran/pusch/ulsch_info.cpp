/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/pusch/ulsch_info.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/sch/sch_segmentation.h"

using namespace srsgnb;

static constexpr unsigned get_crc_size_uci(unsigned nof_bits)
{
  if (nof_bits < 12) {
    return 0;
  }

  if (nof_bits < 20) {
    return 6;
  }

  return 11;
}

static constexpr unsigned calculate_nof_re_harq_ack(unsigned nof_harq_ack_bits,
                                                    float    beta_offset_pusch,
                                                    unsigned nof_re_uci,
                                                    unsigned sum_nof_cb_size,
                                                    float    alpha_scaling,
                                                    unsigned nof_re_uci_l0)
{
  if (nof_harq_ack_bits == 0) {
    return 0;
  }

  unsigned nof_bits_crc = get_crc_size_uci(nof_harq_ack_bits);

  unsigned left  = std::ceil(static_cast<float>(nof_harq_ack_bits + nof_bits_crc) * beta_offset_pusch *
                            static_cast<float>(nof_re_uci) / static_cast<float>(sum_nof_cb_size));
  unsigned right = std::ceil(alpha_scaling * static_cast<float>(nof_re_uci_l0));

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_harq_ack_without_sch(unsigned nof_harq_ack_bits,
                                                                float    beta_offset_pusch,
                                                                float    target_code_rate,
                                                                unsigned modulation_order,
                                                                float    alpha_scaling,
                                                                unsigned nof_re_uci_l0)
{
  if (nof_harq_ack_bits == 0) {
    return 0;
  }

  unsigned nof_bits_crc = get_crc_size_uci(nof_harq_ack_bits);

  unsigned left  = std::ceil(static_cast<float>(nof_harq_ack_bits + nof_bits_crc) * beta_offset_pusch /
                            (target_code_rate * static_cast<float>(modulation_order)));
  unsigned right = std::ceil(alpha_scaling * static_cast<float>(nof_re_uci_l0));

  return std::min(left, right);
}

ulsch_information srsgnb::get_ulsch_information(const ulsch_configuration& config)
{
  ulsch_information result = {};

  // Get shared channel parameters.
  result.sch = get_sch_segmentation_info(config.tbs, config.mcs_descr.get_normalised_target_code_rate());

  // Check DM-RS number of CDM groups without data is valid.
  srsgnb_assert(config.nof_cdm_groups_without_data >= 1 &&
                    config.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(config.dmrs_type),
                "The number of CDM groups without data (i.e., {}) exceeds the maximum (i.e., {}).",
                config.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(config.dmrs_type));

  // Count number of OFDM symbols that contain DM-RS.
  unsigned nof_symbols_dmrs = config.dmrs_symbol_mask.count();

  // Count number of RE used for DM-RS.
  unsigned nof_re_dmrs_per_rb =
      nof_symbols_dmrs * config.nof_cdm_groups_without_data * get_nof_re_per_prb(config.dmrs_type);

  // Count total number of resource elements available for UL data.
  unsigned nof_re_total = config.nof_rb * (config.nof_symbols * NRE - nof_re_dmrs_per_rb);

  // Count number of resource elements that can potentially carry UCI. It excludes symbols with DM-RS.
  unsigned nof_re_uci = (config.nof_symbols - nof_symbols_dmrs) * config.nof_rb * NRE;

  // Retrieve the modulation order.
  unsigned modulation_order = get_bits_per_symbol(config.mcs_descr.modulation);

  // Count the number of resource elements that can potentially carry UCI after the first burst of OFDM symbols that
  // contain DM-RS.
  unsigned nof_re_uci_l0 = 0;
  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Skip if the OFDM symbol contains DM-RS.
    if (config.dmrs_symbol_mask.test(i_symbol)) {
      continue;
    }

    // Accumulate RE contained in the symbol.
    nof_re_uci_l0 += config.nof_rb * NRE;
  }

  // Calculate the number of RE occupied by HARQ-ACK.
  if (config.tbs > 0) {
    // In case of PUSCH multiplexing SCH.
    result.nof_harq_ack_re = calculate_nof_re_harq_ack(config.nof_harq_ack_bits,
                                                       config.beta_offset_harq_ack,
                                                       nof_re_uci,
                                                       result.sch.nof_cb * result.sch.nof_bits_per_cb,
                                                       config.alpha_scaling,
                                                       nof_re_uci_l0);
  } else {
    // In case of PUSCH NOT multiplexing SCH.
    result.nof_harq_ack_re = calculate_nof_re_harq_ack_without_sch(config.nof_harq_ack_bits,
                                                                   config.beta_offset_harq_ack,
                                                                   config.mcs_descr.get_normalised_target_code_rate(),
                                                                   modulation_order,
                                                                   config.alpha_scaling,
                                                                   nof_re_uci_l0);
  }

  // If two or less HARQ-ACK bits are multiplexed, then calculate the number of reserved bits as it was two HARQ-ACK
  // payload bits.
  unsigned nof_harq_ack_rvd_re = 0;
  if (config.nof_harq_ack_bits < 2) {
    if (config.tbs > 0) {
      // In case of PUSCH multiplexing SCH.
      nof_harq_ack_rvd_re = calculate_nof_re_harq_ack(2,
                                                      config.beta_offset_harq_ack,
                                                      nof_re_uci,
                                                      result.sch.nof_cb * result.sch.nof_bits_per_cb,
                                                      config.alpha_scaling,
                                                      nof_re_uci_l0);
    } else {
      // In case of PUSCH NOT multiplexing SCH.
      nof_harq_ack_rvd_re = calculate_nof_re_harq_ack_without_sch(2,
                                                                  config.beta_offset_harq_ack,
                                                                  config.mcs_descr.get_normalised_target_code_rate(),
                                                                  modulation_order,
                                                                  config.alpha_scaling,
                                                                  nof_re_uci_l0);
    }
  } else if (config.nof_harq_ack_bits == 2) {
    nof_harq_ack_rvd_re = result.nof_harq_ack_re;
  }

  // Calculate the number of RE occupied by CSI-part1.
  srsgnb_assert(config.nof_csi_part1_bits == 0, "CSI Part 1 is not currently implemented.");
  result.nof_csi_part2_re = 0;

  // Calculate the number of RE occupied by CSI-part2.
  srsgnb_assert(config.nof_csi_part2_bits == 0, "CSI Part 2 is not currently implemented.");
  result.nof_csi_part2_re = 0;

  // Calculate actual number of RE used by HARQ-ACK. The RE used for HARQ-ACK bits are only considered when there are
  // more than 2 HARQ-ACK bits.
  unsigned nof_harq_ack_re = 0;
  if (config.nof_harq_ack_bits > 2) {
    nof_harq_ack_re = result.nof_harq_ack_re;
  }

  // Calculate the number of RE occupied by shared channel payload.
  unsigned nof_re_ul_sch = nof_re_total - nof_harq_ack_re - result.nof_csi_part1_re - result.nof_csi_part2_re;

  // Number of bits used for shared channel.
  result.nof_ul_sch_bits = nof_re_ul_sch * config.nof_layers * modulation_order;

  // Number of bits used for HARQ-ACK.
  result.nof_harq_ack_bits = result.nof_harq_ack_re * config.nof_layers * modulation_order;

  // Number of bits reserved for HARQ-ACK.
  result.nof_harq_ack_rvd = nof_harq_ack_rvd_re * config.nof_layers * modulation_order;

  // Number of bits used for CSI-part1.
  result.nof_csi_part1_bits = result.nof_csi_part1_re * config.nof_layers * modulation_order;

  // Number of bits used for CSI-part2.
  result.nof_csi_part2_bits = result.nof_csi_part2_re * config.nof_layers * modulation_order;

  return result;
}
