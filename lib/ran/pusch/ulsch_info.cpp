/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/sch/sch_segmentation.h"

using namespace srsran;

static constexpr units::bits get_crc_size_uci(units::bits nof_bits)
{
  using namespace units::literals;

  if (nof_bits < 12_bits) {
    return 0_bits;
  }

  if (nof_bits < 20_bits) {
    return 6_bits;
  }

  return 11_bits;
}

static constexpr unsigned calculate_nof_re_harq_ack(units::bits nof_harq_ack_bits,
                                                    float       beta_offset_pusch,
                                                    unsigned    nof_re_uci,
                                                    unsigned    sum_nof_cb_size,
                                                    float       alpha_scaling,
                                                    unsigned    nof_re_uci_l0)
{
  using namespace units::literals;

  if (nof_harq_ack_bits == 0_bits) {
    return 0;
  }

  units::bits nof_bits_crc = get_crc_size_uci(nof_harq_ack_bits);

  unsigned left  = std::ceil(static_cast<float>((nof_harq_ack_bits + nof_bits_crc).value()) * beta_offset_pusch *
                            static_cast<float>(nof_re_uci) / static_cast<float>(sum_nof_cb_size));
  unsigned right = std::ceil(alpha_scaling * static_cast<float>(nof_re_uci_l0));

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_harq_ack_without_sch(units::bits nof_harq_ack_bits,
                                                                float       beta_offset_pusch,
                                                                float       target_code_rate,
                                                                unsigned    modulation_order,
                                                                float       alpha_scaling,
                                                                unsigned    nof_re_uci_l0)
{
  using namespace units::literals;

  if (nof_harq_ack_bits == 0_bits) {
    return 0;
  }

  units::bits nof_bits_crc = get_crc_size_uci(nof_harq_ack_bits);

  unsigned left  = std::ceil(static_cast<float>((nof_harq_ack_bits + nof_bits_crc).value()) * beta_offset_pusch /
                            (target_code_rate * static_cast<float>(modulation_order)));
  unsigned right = std::ceil(alpha_scaling * static_cast<float>(nof_re_uci_l0));

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_csi_part1(units::bits nof_csi_part1_bits,
                                                     float       beta_offset_pusch,
                                                     unsigned    nof_re_uci,
                                                     unsigned    nof_re_harq_ack,
                                                     unsigned    sum_nof_cb_size,
                                                     float       alpha_scaling)
{
  using namespace units::literals;

  if (nof_csi_part1_bits == 0_bits) {
    return 0;
  }

  units::bits nof_bits_crc = get_crc_size_uci(nof_csi_part1_bits);

  unsigned left  = std::ceil(static_cast<float>((nof_csi_part1_bits + nof_bits_crc).value()) * beta_offset_pusch *
                            static_cast<float>(nof_re_uci) / static_cast<float>(sum_nof_cb_size));
  unsigned right = static_cast<unsigned>(std::ceil(alpha_scaling * static_cast<float>(nof_re_uci))) - nof_re_harq_ack;

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_csi_part1_without_sch(units::bits nof_csi_part1_bits,
                                                                 units::bits nof_csi_part2_bits,
                                                                 unsigned    nof_re_uci,
                                                                 unsigned    nof_re_harq_ack,
                                                                 float       beta_offset_pusch,
                                                                 float       pusch_code_rate,
                                                                 unsigned    pusch_mod_order)
{
  using namespace units::literals;

  if (nof_csi_part1_bits == 0_bits) {
    return 0;
  }

  // No CSI Part 2 is multiplexed over PUSCH.
  if (nof_csi_part2_bits == 0_bits) {
    return nof_re_uci - nof_re_harq_ack;
  }

  // CSI Part 2 is multiplexed over PUSCH.
  units::bits nof_bits_crc = get_crc_size_uci(nof_csi_part1_bits);

  unsigned left  = std::ceil(static_cast<float>((nof_csi_part1_bits + nof_bits_crc).value()) * beta_offset_pusch /
                            (pusch_code_rate * static_cast<float>(pusch_mod_order)));
  unsigned right = nof_re_uci - nof_re_harq_ack;

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_csi_part2(units::bits nof_csi_part2_bits,
                                                     float       beta_offset_pusch,
                                                     unsigned    nof_re_uci,
                                                     unsigned    nof_re_harq_ack,
                                                     unsigned    nof_re_csi_part1,
                                                     unsigned    sum_nof_cb_size,
                                                     float       alpha_scaling)
{
  using namespace units::literals;

  if (nof_csi_part2_bits == 0_bits) {
    return 0;
  }

  units::bits nof_bits_crc = get_crc_size_uci(nof_csi_part2_bits);

  unsigned left  = std::ceil(static_cast<float>((nof_csi_part2_bits + nof_bits_crc).value()) * beta_offset_pusch *
                            static_cast<float>(nof_re_uci) / static_cast<float>(sum_nof_cb_size));
  unsigned right = static_cast<unsigned>(std::ceil(alpha_scaling * static_cast<float>(nof_re_uci))) - nof_re_harq_ack -
                   nof_re_csi_part1;

  return std::min(left, right);
}

static constexpr unsigned calculate_nof_re_csi_part2_without_sch(units::bits nof_csi_part2_bits,
                                                                 unsigned    nof_re_uci,
                                                                 unsigned    nof_re_harq_ack,
                                                                 unsigned    nof_re_csi_part1)
{
  using namespace units::literals;

  if (nof_csi_part2_bits == 0_bits) {
    return 0;
  }

  return nof_re_uci - nof_re_harq_ack - nof_re_csi_part1;
}

ulsch_information srsran::get_ulsch_information(const ulsch_configuration& config)
{
  using namespace units::literals;

  ulsch_information result = {};

  // Get shared channel parameters.
  if (config.tbs > 0_bits) {
    result.sch.emplace(get_sch_segmentation_info(config.tbs, config.mcs_descr.get_normalised_target_code_rate()));
  }

  // Check whether the number of CDM groups without data is valid.
  srsran_assert(config.nof_cdm_groups_without_data >= 1 &&
                    config.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(config.dmrs_type),
                "The number of CDM groups without data (i.e., {}) is invalid (min. 1, max. {}).",
                config.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(config.dmrs_type));

  // Make sure the OFDM symbol that carry DM-RS are within the time allocation.
  srsran_assert(config.dmrs_symbol_mask.count() > 0,
                "The number of OFDM symbols carrying DM-RS RE must be greater than zero.");
  srsran_assert(
      static_cast<unsigned>(config.dmrs_symbol_mask.find_lowest(true)) >= config.start_symbol_index,
      "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the first symbol "
      "allocated to transmission (i.e., {}).",
      config.dmrs_symbol_mask.find_lowest(true),
      config.start_symbol_index);
  srsran_assert(static_cast<unsigned>(config.dmrs_symbol_mask.find_highest(true)) <
                    (config.start_symbol_index + config.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated "
                "to transmission (i.e., {}).",
                config.dmrs_symbol_mask.find_highest(true),
                config.start_symbol_index + config.nof_symbols - 1);
  srsran_assert(config.dmrs_symbol_mask.size() >= (config.start_symbol_index + config.nof_symbols),
                "The DM-RS symbol mask size (i.e., {}) must be the same as the number of symbols allocated to the "
                "transmission within the slot (i.e., {}).",
                config.dmrs_symbol_mask.size(),
                config.start_symbol_index + config.nof_symbols);

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
  for (unsigned i_symbol     = config.dmrs_symbol_mask.find_lowest(),
                i_symbol_end = config.start_symbol_index + config.nof_symbols;
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
  if (config.tbs > 0_bits) {
    // In case of PUSCH multiplexing SCH.
    result.nof_harq_ack_re =
        calculate_nof_re_harq_ack(config.nof_harq_ack_bits,
                                  config.beta_offset_harq_ack,
                                  nof_re_uci,
                                  result.sch.value().nof_cb * result.sch.value().nof_bits_per_cb.value(),
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
  if (config.nof_harq_ack_bits < 2_bits) {
    if (config.tbs > 0_bits) {
      // In case of PUSCH multiplexing SCH.
      nof_harq_ack_rvd_re =
          calculate_nof_re_harq_ack(2_bits,
                                    config.beta_offset_harq_ack,
                                    nof_re_uci,
                                    result.sch.value().nof_cb * result.sch.value().nof_bits_per_cb.value(),
                                    config.alpha_scaling,
                                    nof_re_uci_l0);
    } else {
      // In case of PUSCH NOT multiplexing SCH.
      nof_harq_ack_rvd_re = calculate_nof_re_harq_ack_without_sch(2_bits,
                                                                  config.beta_offset_harq_ack,
                                                                  config.mcs_descr.get_normalised_target_code_rate(),
                                                                  modulation_order,
                                                                  config.alpha_scaling,
                                                                  nof_re_uci_l0);
    }
  } else if (config.nof_harq_ack_bits == 2_bits) {
    nof_harq_ack_rvd_re = result.nof_harq_ack_re;
  }

  // If two or less HARQ-ACK bits are multiplexed, compute the CSI Part 1 RE using the reserved HARQ-ACK resources.
  unsigned nof_harq_ack_re_csi_part1 =
      (config.nof_harq_ack_bits <= 2_bits) ? nof_harq_ack_rvd_re : result.nof_harq_ack_re;

  // Calculate the number of RE occupied by CSI Part 1.
  if (config.tbs > 0_bits) {
    // In case of PUSCH multiplexing SCH.
    result.nof_csi_part1_re =
        calculate_nof_re_csi_part1(config.nof_csi_part1_bits,
                                   config.beta_offset_csi_part1,
                                   nof_re_uci,
                                   nof_harq_ack_re_csi_part1,
                                   result.sch.value().nof_cb * result.sch.value().nof_bits_per_cb.value(),
                                   config.alpha_scaling);
  } else {
    // In case of PUSCH NOT multiplexing SCH.
    result.nof_csi_part1_re = calculate_nof_re_csi_part1_without_sch(config.nof_csi_part1_bits,
                                                                     config.nof_csi_part2_bits,
                                                                     nof_re_uci,
                                                                     nof_harq_ack_re_csi_part1,
                                                                     config.beta_offset_csi_part1,
                                                                     config.mcs_descr.get_normalised_target_code_rate(),
                                                                     modulation_order);
  }

  // If two or less HARQ-ACK bits are multiplexed, compute the CSI Part 2 RE as if there were no HARQ-ACK resources.
  unsigned nof_harq_ack_re_csi_part2 = (config.nof_harq_ack_bits <= 2_bits) ? 0 : result.nof_harq_ack_re;

  // Calculate the number of RE occupied by CSI Part 2.
  if (config.tbs > 0_bits) {
    // In case of PUSCH multiplexing SCH.
    result.nof_csi_part2_re =
        calculate_nof_re_csi_part2(config.nof_csi_part2_bits,
                                   config.beta_offset_csi_part2,
                                   nof_re_uci,
                                   nof_harq_ack_re_csi_part2,
                                   result.nof_csi_part1_re,
                                   result.sch.value().nof_cb * result.sch.value().nof_bits_per_cb.value(),
                                   config.alpha_scaling);
  } else {
    // In case of PUSCH NOT multiplexing SCH.
    result.nof_csi_part2_re = calculate_nof_re_csi_part2_without_sch(
        config.nof_csi_part2_bits, nof_re_uci, nof_harq_ack_re_csi_part2, result.nof_csi_part1_re);
  }

  // Calculate actual number of RE used by HARQ-ACK. The RE used for HARQ-ACK bits are only considered when there are
  // more than 2 HARQ-ACK bits.
  unsigned nof_harq_ack_re = 0;
  if (config.nof_harq_ack_bits > 2_bits) {
    nof_harq_ack_re = result.nof_harq_ack_re;
  }

  // Calculate the number of RE occupied by shared channel payload.
  unsigned nof_re_ul_sch =
      (config.tbs > 0_bits) ? nof_re_total - nof_harq_ack_re - result.nof_csi_part1_re - result.nof_csi_part2_re : 0U;

  // Number of bits used for shared channel.
  result.nof_ul_sch_bits = units::bits(nof_re_ul_sch * config.nof_layers * modulation_order);

  // Number of bits used for HARQ-ACK.
  result.nof_harq_ack_bits = units::bits(result.nof_harq_ack_re * config.nof_layers * modulation_order);

  // Number of bits reserved for HARQ-ACK.
  result.nof_harq_ack_rvd = units::bits(nof_harq_ack_rvd_re * config.nof_layers * modulation_order);

  // Number of bits used for CSI Part 1.
  result.nof_csi_part1_bits = units::bits(result.nof_csi_part1_re * config.nof_layers * modulation_order);

  // Number of bits used for CSI Part 2.
  result.nof_csi_part2_bits = units::bits(result.nof_csi_part2_re * config.nof_layers * modulation_order);

  return result;
}
