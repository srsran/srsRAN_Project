/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ulsch_demultiplex_impl.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

namespace {

unsigned get_ulsch_demultiplex_l1(const symbol_slot_mask& dmrs_symbol_mask)
{
  // Find first OFDM symbol that contains DM-RS.
  int first_symbol_dmrs = dmrs_symbol_mask.find_lowest(true);
  srsran_assert(first_symbol_dmrs >= 0, "No DM-RS symbol found.");

  // Find first OFDM symbol that does not contain DM-RS after the first OFDM symbol that contains DM-RS.
  int first_symbol_without_dmrs =
      dmrs_symbol_mask.find_lowest(static_cast<size_t>(first_symbol_dmrs), dmrs_symbol_mask.size(), false);
  srsran_assert(first_symbol_without_dmrs >= 0, "No DM-RS symbol found.");

  return static_cast<unsigned>(first_symbol_without_dmrs);
}

unsigned get_ulsch_demultiplex_l1_csi(const symbol_slot_mask& dmrs_symbol_mask)
{
  // Find first OFDM symbol that does not contain DM-RS.
  int first_symbol_without_dmrs = dmrs_symbol_mask.find_lowest(false);
  srsran_assert(first_symbol_without_dmrs >= 0, "No DM-RS symbol found.");

  return static_cast<unsigned>(first_symbol_without_dmrs);
}

unsigned get_ulsch_demultiplex_nof_re_prb_dmrs(dmrs_type dmrs_, unsigned nof_cdm_groups_without_data, unsigned nof_prb)
{
  dmrs_config_type dmrs = (dmrs_ == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  // Check whether the number of CDM groups without data is valid.
  srsran_assert(nof_cdm_groups_without_data >= 1 &&
                    nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs),
                "The number of CDM groups without data (i.e., {}) exceeds the maximum (i.e., {}).",
                nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs));

  // Count number of RE used for DM-RS.
  unsigned nof_re_dmrs_per_rb = nof_cdm_groups_without_data * get_nof_re_per_prb(dmrs);

  return (NRE - nof_re_dmrs_per_rb) * nof_prb;
}

// Calculates the UCI fields multiplexing on PUSCH. The functions passed as parameters provide the multiplexing actions
// for each specific field. This allows reusing the same function for demultiplexing and calculating the placeholder
// positions.
template <typename FuncSchData, typename FuncHarqAck, typename FuncCsiPart1, typename FuncCsiPart2>
void ulsch_demultiplex_generic(FuncSchData&                            func_sch_data,
                               FuncHarqAck&                            func_harq_ack,
                               FuncCsiPart1&                           func_csi_part1,
                               FuncCsiPart2&                           func_csi_part2,
                               unsigned                                G_harq_ack,
                               unsigned                                G_csi_part1,
                               unsigned                                G_csi_part2,
                               const ulsch_demultiplex::configuration& config)
{
  // Make sure the functions provided as parameters have a valid signature.
  static_assert(std::is_convertible<FuncSchData, std::function<void(bool reserved)>>::value,
                "The function for multiplexing SCH signature must be \"void () ()\"");
  static_assert(std::is_convertible<FuncHarqAck, std::function<void()>>::value,
                "The function for multiplexing HARQ-ACK signature must be \"void () ()\"");
  static_assert(std::is_convertible<FuncCsiPart1, std::function<void()>>::value,
                "The function for multiplexing CSI Part 1 signature must be \"void () ()\"");
  static_assert(std::is_convertible<FuncCsiPart2, std::function<void(bool reserved)>>::value,
                "The function for multiplexing CSI Part 2 signature must be \"void () ()\"");

  // Calculates the number of bits per resource element.
  unsigned nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;

  // OFDM symbol index of the first OFDM symbol after the first set of consecutive OFDM symbol(s) carrying DM-RS in the
  // first hop.
  unsigned l1 = get_ulsch_demultiplex_l1(config.dmrs_symbol_mask);

  // OFDM symbol index of the first OFDM symbol that does not carry DM-RS in the first hop.
  unsigned l1_csi = get_ulsch_demultiplex_l1_csi(config.dmrs_symbol_mask);

  // Number of bits multiplexed in OFDM symbols containing DM-RS.
  unsigned nof_re_dmrs =
      get_ulsch_demultiplex_nof_re_prb_dmrs(config.dmrs, config.nof_cdm_groups_without_data, config.nof_prb);

  // Number of RE used for Phase Tracking Reference Signals.
  unsigned nof_re_ptrs = 0;

  // Number of reserved HARQ-ACK information bits.
  unsigned G_ack_rvd = config.nof_harq_ack_rvd;

  // Count the number of multiplexed information bits for each of the fields.
  unsigned m_rvd_count       = 0;
  unsigned m_harq_ack_count  = 0;
  unsigned m_csi_part1_count = 0;
  unsigned m_csi_part2_count = 0;

  // For each symbol of the transmission...
  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // If the OFDM symbol carries DM-RS, it multiplexes only SCH data.
    if (config.dmrs_symbol_mask.test(i_symbol)) {
      for (unsigned i = 0; i != nof_re_dmrs; ++i) {
        func_sch_data(false);
      }
      continue;
    }

    unsigned M_ulsch_sc = config.nof_prb * NRE;
    unsigned M_uci_sc   = M_ulsch_sc - nof_re_ptrs;
    unsigned M_uci_rvd  = 0;

    // Stride and counter for the number of HARQ-ACK bits multiplexed in the OFDM symbol.
    unsigned ack_d          = 0;
    unsigned ack_m_re_count = 0;

    // Stride and counter for the number of reserved bits multiplexed in the OFDM symbol.
    unsigned rvd_d          = 0;
    unsigned rvd_m_re_count = 0;

    // Stride and counter for the number of CSI Part 1 bits multiplexed in the OFDM symbol.
    unsigned csi1_d          = 0;
    unsigned csi1_m_re_count = 0;

    // Stride and counter for the number of CSI Part 2 bits multiplexed in the OFDM symbol.
    unsigned csi2_d          = 0;
    unsigned csi2_m_re_count = 0;

    // Prepare HARQ-ACK and reserved bits.
    if (i_symbol >= l1) {
      // Calculate the remainder of reserved bits.
      unsigned G_ack_rvd_remainder = G_ack_rvd - m_rvd_count;

      // Calculate the remainder of HARQ-ACK bits.
      unsigned G_ack_remainder = G_harq_ack - m_harq_ack_count;

      if ((G_ack_rvd != 0) && (G_ack_rvd_remainder != 0)) {
        rvd_d          = 1;
        rvd_m_re_count = M_uci_sc;
        if (G_ack_rvd_remainder < M_uci_sc * nof_bits_per_re) {
          rvd_d          = (M_uci_sc * nof_bits_per_re) / G_ack_rvd_remainder;
          rvd_m_re_count = divide_ceil(G_ack_rvd_remainder, nof_bits_per_re);
        }
        M_uci_rvd = rvd_m_re_count;

        if (G_ack_remainder != 0) {
          ack_d          = 1;
          ack_m_re_count = M_uci_rvd;
          if (G_ack_remainder < M_uci_rvd * nof_bits_per_re) {
            ack_d          = (M_uci_rvd * nof_bits_per_re) / G_ack_remainder;
            ack_m_re_count = divide_ceil(G_ack_remainder, nof_bits_per_re);
          }
        }
      } else if (G_ack_remainder != 0) {
        ack_d          = 1;
        ack_m_re_count = M_uci_sc;
        if (G_ack_remainder < M_uci_sc * nof_bits_per_re) {
          ack_d          = (M_uci_sc * nof_bits_per_re) / G_ack_remainder;
          ack_m_re_count = divide_ceil(G_ack_remainder, nof_bits_per_re);
        }
        M_uci_sc -= ack_m_re_count;
      }
    }

    // Prepare CSI Part 1 and Part 2 reserved bits.
    if (i_symbol >= l1_csi) {
      // Calculate the remainder of CSI Part 1 bits.
      unsigned G_csi1_remainder = G_csi_part1 - m_csi_part1_count;

      // Calculate the remainder of CSI Part 2 bits.
      unsigned G_csi2_remainder = G_csi_part2 - m_csi_part2_count;

      // Prepare CSI Part 1 bits.
      if ((M_uci_sc > M_uci_rvd) && (G_csi1_remainder != 0)) {
        csi1_d          = 1;
        csi1_m_re_count = M_uci_sc - M_uci_rvd;
        if (G_csi1_remainder < (M_uci_sc - M_uci_rvd) * nof_bits_per_re) {
          csi1_d          = ((M_uci_sc - M_uci_rvd) * nof_bits_per_re) / G_csi1_remainder;
          csi1_m_re_count = divide_ceil(G_csi1_remainder, nof_bits_per_re);
        }
        M_uci_sc -= csi1_m_re_count;
      }

      // Prepare CSI Part 2 bits.
      if ((M_uci_sc > 0) && (G_csi2_remainder != 0)) {
        csi2_d          = 1;
        csi2_m_re_count = M_uci_sc;
        if (G_csi2_remainder < M_uci_sc * nof_bits_per_re) {
          csi2_d          = (M_uci_sc * nof_bits_per_re) / G_csi2_remainder;
          csi2_m_re_count = divide_ceil(G_csi2_remainder, nof_bits_per_re);
        }
        M_uci_sc -= csi2_m_re_count;
      }
    }

    // Leave the rest of resource elements for UL-SCH.
    unsigned ulsch_m_re_count = M_uci_sc;

    // Count the number of bits to process of each type.
    m_rvd_count += rvd_m_re_count * nof_bits_per_re;
    m_harq_ack_count += ack_m_re_count * nof_bits_per_re;
    m_csi_part1_count += csi1_m_re_count * nof_bits_per_re;
    m_csi_part2_count += csi2_m_re_count * nof_bits_per_re;

    // Iterate over each subcarrier within the OFDM symbol.
    for (unsigned i_subcarrier = 0, i_csi1 = 0, i_csi2 = 0, i_ack = 0; i_subcarrier != M_ulsch_sc; ++i_subcarrier) {
      // Detect if the resource element is reserved.
      bool is_reserved = ((rvd_m_re_count != 0) && (i_subcarrier % rvd_d == 0));

      // Set to true if a reserved RE is used for HARQ-ACK and either CSI Part 1 or UL-SCH.
      bool is_zero = false;

      // Decrement number of pending Reserved RE for the symbol.
      if (is_reserved) {
        --rvd_m_re_count;
      }

      // Process HARQ-ACK with reserved elements.
      if (G_ack_rvd != 0) {
        if ((is_reserved) && (ack_m_re_count != 0) && ((i_ack++) % ack_d == 0)) {
          // Multiplex HARQ-ACK.
          func_harq_ack();

          // Decrement number of pending HARQ-ACK bits RE for the symbol.
          --ack_m_re_count;

          is_zero = true;
        }
      } else {
        // Process HARQ-ACK without reserved elements.
        if ((ack_m_re_count != 0) && ((i_ack++) % ack_d == 0)) {
          // Multiplex HARQ-ACK.
          func_harq_ack();

          // Decrement number of pending HARQ-ACK bits RE for the symbol.
          --ack_m_re_count;

          continue;
        }
      }

      // Process CSI Part 1. It is not allowed to use reserved RE.
      if (!is_reserved && (csi1_m_re_count != 0) && ((i_csi1++) % csi1_d == 0)) {
        // Multiplex CSI Part 1.
        func_csi_part1();

        // Decrement number of pending CSI Part 1 RE for the symbol.
        --csi1_m_re_count;

        continue;
      }

      // Process CSI Part 2.
      if ((csi2_m_re_count != 0) && ((i_csi2++) % csi2_d == 0)) {
        // Multiplex CSI Part 2.
        func_csi_part2(is_zero);

        // Decrement number of pending CSI Part 2 RE for the symbol.
        --csi2_m_re_count;

        continue;
      }

      // Multiplex SCH data.
      func_sch_data(is_zero);

      // Decrement number of pending SCH data RE for the symbol.
      srsran_assert(ulsch_m_re_count > 0, "UL-SCH RE are exhausted.");
      --ulsch_m_re_count;
    }

    // Assert that all RE have been allocated.
    srsran_assert(ack_m_re_count == 0, "{} RE for HARQ-ACK are not processed.", ack_m_re_count);
    srsran_assert(csi1_m_re_count == 0, "{} RE for CSI Part 1 are not processed.", csi1_m_re_count);
    srsran_assert(csi2_m_re_count == 0, "{} RE for CSI Part 2 are not processed.", csi2_m_re_count);
    srsran_assert(ulsch_m_re_count == 0, "{} RE for SCH data are not processed.", ulsch_m_re_count);
  }

  // Assert all bits have been processed.
  srsran_assert(m_rvd_count == G_ack_rvd, "Only {} of {} reserved elements processed.", m_rvd_count, G_ack_rvd);
  srsran_assert(
      m_harq_ack_count == G_harq_ack, "Only {} of {} HARQ-ACK elements processed.", m_harq_ack_count, G_harq_ack);
  srsran_assert(
      m_csi_part1_count == G_csi_part1, "Only {} of {} CSI Part 1 elements processed.", m_csi_part1_count, G_csi_part1);
  srsran_assert(
      m_csi_part2_count == G_csi_part2, "Only {} of {} CSI Part 2 elements processed.", m_csi_part2_count, G_csi_part2);
}

} // namespace

void ulsch_demultiplex_impl::demultiplex_csi_part1(span<log_likelihood_ratio>       csi_part1,
                                                   span<const log_likelihood_ratio> input,
                                                   unsigned                         nof_enc_harq_ack_bits,
                                                   const configuration&             config)
{
  // Skip demultiplexing if no UCI is multiplexed.
  if ((nof_enc_harq_ack_bits == 0) && csi_part1.empty()) {
    return;
  }

  unsigned nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;

  // Function to ignore input data.
  auto func_ignore = [&input, &nof_bits_per_re]() { input = input.last(input.size() - nof_bits_per_re); };

  // Function to ignore input data.
  auto func_ignore2 = [&input, &nof_bits_per_re](bool is_reserved) {
    if (is_reserved) {
      return;
    }

    input = input.last(input.size() - nof_bits_per_re);
  };

  // Function to demultiplex CSI Part 1 bits.
  auto func_csi_part1 = [&csi_part1, &input, &nof_bits_per_re]() {
    srsvec::copy(csi_part1.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input     = input.last(input.size() - nof_bits_per_re);
    csi_part1 = csi_part1.last(csi_part1.size() - nof_bits_per_re);
  };

  ulsch_demultiplex_generic(
      func_ignore2, func_ignore, func_csi_part1, func_ignore2, nof_enc_harq_ack_bits, csi_part1.size(), 0, config);

  // Assert that input buffers have been consumed.
  srsran_assert(csi_part1.empty(), "{} soft bits have not been multiplexed for CSI Part 1.", csi_part1.size());
  srsran_assert(input.empty(), "{} input soft bits have not been multiplexed.", input.size());
}

void ulsch_demultiplex_impl::demultiplex_sch_harq_ack_and_csi_part2(span<log_likelihood_ratio>       sch_data,
                                                                    span<log_likelihood_ratio>       harq_ack,
                                                                    span<log_likelihood_ratio>       csi_part2,
                                                                    span<const log_likelihood_ratio> input,
                                                                    unsigned                         nof_csi_part1_bits,
                                                                    const configuration&             config)
{
  // Skip demultiplexing if no UCI is multiplexed.
  if (harq_ack.empty() && (nof_csi_part1_bits == 0) && csi_part2.empty()) {
    srsran_assert(sch_data.size() == input.size(),
                  "SCH data number of soft bits (i.e. {}) must be equal to the number of input soft bits (i.e., {}).",
                  sch_data.size(),
                  input.size());
    srsvec::copy(sch_data, input);
    return;
  }

  unsigned nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;

  // Function to ignore input data.
  auto func_ignore = [&input, &nof_bits_per_re]() { input = input.last(input.size() - nof_bits_per_re); };

  // Function to demultiplex SCH data.
  auto func_sch_data = [&sch_data, &input, &nof_bits_per_re](bool is_reserved) {
    if (is_reserved) {
      srsvec::zero(sch_data.first(nof_bits_per_re));
      sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);
      return;
    }

    srsvec::copy(sch_data.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input    = input.last(input.size() - nof_bits_per_re);
    sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);
  };

  // Function to demultiplex HARQ ACK.
  auto func_harq_ack = [&harq_ack, &input, &nof_bits_per_re]() {
    srsvec::copy(harq_ack.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input    = input.last(input.size() - nof_bits_per_re);
    harq_ack = harq_ack.last(harq_ack.size() - nof_bits_per_re);
  };

  // Function to demultiplex CSI Part 2 bits.
  auto func_csi_part2 = [&csi_part2, &input, &nof_bits_per_re](bool is_reserved) {
    if (is_reserved) {
      srsvec::zero(csi_part2.first(nof_bits_per_re));
      csi_part2 = csi_part2.last(csi_part2.size() - nof_bits_per_re);
      return;
    }

    srsvec::copy(csi_part2.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input     = input.last(input.size() - nof_bits_per_re);
    csi_part2 = csi_part2.last(csi_part2.size() - nof_bits_per_re);
  };

  ulsch_demultiplex_generic(func_sch_data,
                            func_harq_ack,
                            func_ignore,
                            func_csi_part2,
                            harq_ack.size(),
                            nof_csi_part1_bits,
                            csi_part2.size(),
                            config);

  // Assert that input buffers have been consumed.
  srsran_assert(csi_part2.empty(), "{} soft bits have not been multiplexed for CSI Part 2.", csi_part2.size());
  srsran_assert(sch_data.empty(), "{} soft bits have not been multiplexed for SCH data.", sch_data.size());
  srsran_assert(input.empty(), "{} input soft bits have not been multiplexed.", input.size());
}

void ulsch_demultiplex_impl::demultiplex(span<log_likelihood_ratio>              sch_data,
                                         span<log_likelihood_ratio>              harq_ack,
                                         span<log_likelihood_ratio>              csi_part1,
                                         span<log_likelihood_ratio>              csi_part2,
                                         span<const log_likelihood_ratio>        input,
                                         const ulsch_demultiplex::configuration& config)
{
  // Skip demultiplexing if no UCI is multiplexed.
  if (harq_ack.empty() && csi_part1.empty() && csi_part2.empty()) {
    srsran_assert(sch_data.size() == input.size(),
                  "SCH data number of soft bits (i.e. {}) must be equal to the number of input soft bits (i.e., {}).",
                  sch_data.size(),
                  input.size());
    srsvec::copy(sch_data, input);
    return;
  }

  unsigned nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;

  // Function to demultiplex SCH data.
  auto func_sch_data = [&sch_data, &input, &nof_bits_per_re](bool is_reserved) {
    if (is_reserved) {
      srsvec::zero(sch_data.first(nof_bits_per_re));
      sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);
      return;
    }

    srsvec::copy(sch_data.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input    = input.last(input.size() - nof_bits_per_re);
    sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);
  };

  // Function to demultiplex HARQ-ACK bits.
  auto func_harq_ack = [&harq_ack, &input, &nof_bits_per_re]() {
    srsvec::copy(harq_ack.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input    = input.last(input.size() - nof_bits_per_re);
    harq_ack = harq_ack.last(harq_ack.size() - nof_bits_per_re);
  };

  // Function to demultiplex CSI Part 1 bits.
  auto func_csi_part1 = [&csi_part1, &input, &nof_bits_per_re]() {
    srsvec::copy(csi_part1.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input     = input.last(input.size() - nof_bits_per_re);
    csi_part1 = csi_part1.last(csi_part1.size() - nof_bits_per_re);
  };

  // Function to demultiplex CSI Part 2 bits.
  auto func_csi_part2 = [&csi_part2, &input, &nof_bits_per_re](bool is_reserved) {
    if (is_reserved) {
      srsvec::zero(csi_part2.first(nof_bits_per_re));
      csi_part2 = csi_part2.last(csi_part2.size() - nof_bits_per_re);
      return;
    }

    srsvec::copy(csi_part2.first(nof_bits_per_re), input.first(nof_bits_per_re));
    input     = input.last(input.size() - nof_bits_per_re);
    csi_part2 = csi_part2.last(csi_part2.size() - nof_bits_per_re);
  };

  ulsch_demultiplex_generic(func_sch_data,
                            func_harq_ack,
                            func_csi_part1,
                            func_csi_part2,
                            harq_ack.size(),
                            csi_part1.size(),
                            csi_part2.size(),
                            config);

  // Assert that input buffers have been consumed.
  srsran_assert(harq_ack.empty(), "{} soft bits have not been multiplexed for HARQ-ACK.", harq_ack.size());
  srsran_assert(csi_part1.empty(), "{} soft bits have not been multiplexed for CSI Part 1.", csi_part1.size());
  srsran_assert(csi_part2.empty(), "{} soft bits have not been multiplexed for CSI Part 2.", csi_part2.size());
  srsran_assert(sch_data.empty(), "{} soft bits have not been multiplexed for SCH data.", sch_data.size());
  srsran_assert(input.empty(), "{} input soft bits have not been multiplexed.", input.size());
}

ulsch_placeholder_list ulsch_demultiplex_impl::get_placeholders(const message_information& message_info,
                                                                const configuration&       config)
{
  unsigned               re_counter = 0;
  ulsch_placeholder_list result;

  // Skip placeholder if the modulation order is less than two.
  if (get_bits_per_symbol(config.modulation) < 2) {
    return result;
  }

  // Skip placeholder if none of the fields carries one information bit.
  if ((message_info.nof_harq_ack_bits != 1) && (message_info.nof_csi_part1_bits != 1) &&
      (message_info.nof_csi_part2_bits != 1)) {
    return result;
  }

  // Function to demultiplex SCH data.
  auto func_sch_data = [&re_counter](bool is_reserved) {
    if (is_reserved) {
      return;
    }
    ++re_counter;
  };

  // Function to demultiplex HARQ-ACK bits.
  auto func_harq_ack = [&re_counter, &message_info, &result]() {
    if (message_info.nof_harq_ack_bits == 1) {
      result.push_back(re_counter);
    }
    ++re_counter;
  };

  // Function to demultiplex CSI Part 1 bits.
  auto func_csi_part1 = [&re_counter, &message_info, &result]() {
    if (message_info.nof_csi_part1_bits == 1) {
      result.push_back(re_counter);
    }
    ++re_counter;
  };

  // Function to demultiplex CSI Part 2 bits.
  auto func_csi_part2 = [&re_counter, &message_info, &result](bool is_reserved) {
    if (is_reserved) {
      return;
    }
    if (message_info.nof_csi_part2_bits == 1) {
      result.push_back(re_counter);
    }
    ++re_counter;
  };

  ulsch_demultiplex_generic(func_sch_data,
                            func_harq_ack,
                            func_csi_part1,
                            func_csi_part2,
                            message_info.nof_enc_harq_ack_bits,
                            message_info.nof_enc_csi_part1_bits,
                            message_info.nof_enc_csi_part2_bits,
                            config);
  return result;
}
