/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ulsch_demultiplex_impl.h"
#include "srsgnb/ran/dmrs.h"
#include "srsgnb/ran/pusch/pusch_constants.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

namespace {

unsigned get_ulsch_demultiplex_l1(const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_symbol_mask)
{
  // Find first OFDM symbol that contains DM-RS.
  int first_symbol_dmrs = dmrs_symbol_mask.find_lowest(true);
  srsgnb_assert(first_symbol_dmrs >= 0, "No DM-RS symbol found.");

  // Find first OFDM symbol that does not contain DM-RS after the first OFDM symbol that contains DM-RS.
  int first_symbol_without_dmrs =
      dmrs_symbol_mask.find_lowest(static_cast<size_t>(first_symbol_dmrs), dmrs_symbol_mask.size(), false);
  srsgnb_assert(first_symbol_without_dmrs >= 0, "No DM-RS symbol found.");

  return static_cast<unsigned>(first_symbol_without_dmrs);
}

unsigned get_ulsch_demultiplex_l1_csi(const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_symbol_mask)
{
  // Find first OFDM symbol that does not contain DM-RS.
  int first_symbol_without_dmrs = dmrs_symbol_mask.find_lowest(false);
  srsgnb_assert(first_symbol_without_dmrs >= 0, "No DM-RS symbol found.");

  return static_cast<unsigned>(first_symbol_without_dmrs);
}

unsigned get_ulsch_demultiplex_nof_re_prb_dmrs(dmrs_type dmrs_, unsigned nof_cdm_groups_without_data, unsigned nof_prb)
{
  dmrs_config_type dmrs = (dmrs_ == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  // Check DM-RS number of CDM groups without data is valid.
  srsgnb_assert(nof_cdm_groups_without_data >= 1 &&
                    nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs),
                "The number of CDM groups without data (i.e., {}) exceeds the maximum (i.e., {}).",
                nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs));

  // Count number of RE used for DM-RS.
  unsigned nof_re_dmrs_per_rb = nof_cdm_groups_without_data * get_nof_re_per_prb(dmrs);

  return (NRE - nof_re_dmrs_per_rb) * nof_prb;
}

void ulsch_demultiplex_generic(span<log_likelihood_ratio>              sch_data,
                               span<log_likelihood_ratio>              harq_ack,
                               span<log_likelihood_ratio>              csi_part1,
                               span<log_likelihood_ratio>              csi_part2,
                               span<const log_likelihood_ratio>        input,
                               const ulsch_demultiplex::configuration& config)
{
  // Skip demultiplexing if no UCI is multiplexed.
  if (harq_ack.empty() && csi_part1.empty() && csi_part2.empty()) {
    srsgnb_assert(sch_data.size() == input.size(),
                  "SCH data number of soft bits (i.e. {}) must be equal to the number of input soft bits (i.e., {}).",
                  sch_data.size(),
                  input.size());
    srsvec::copy(sch_data, input);
    return;
  }

  // Calculates the number of bits per resource element.
  unsigned nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;

  // OFDM symbol index of the first OFDM symbol after the first set of consecutive OFDM symbol(s) carrying DM-RS in the
  // first hop.
  unsigned l1 = get_ulsch_demultiplex_l1(config.dmrs_symbol_mask);

  // OFDM symbol index of the first OFDM symbol that does not carry DM-RS in the first hop.
  unsigned l1_csi = get_ulsch_demultiplex_l1_csi(config.dmrs_symbol_mask);

  // Number of bits multiplexed in OFDM symbols containing DM-RS.
  unsigned nof_bits_dmrs = nof_bits_per_re * get_ulsch_demultiplex_nof_re_prb_dmrs(
                                                 config.dmrs, config.nof_cdm_groups_without_data, config.nof_prb);

  // Number of RE used for Phase Tracking Reference Signals.
  unsigned nof_re_ptrs = 0;

  // Number of reserved HARQ-ACK information bits.
  unsigned G_ack_rvd = config.nof_harq_ack_rvd;

  // Count the number of multiplexed information bits for each of the fields.
  unsigned m_rvd_count = 0;

  // For each symbol of the transmission...
  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // If the OFDM symbol carries DM-RS, it multiplexes only SCH data.
    if (config.dmrs_symbol_mask.test(i_symbol)) {
      srsvec::copy(sch_data.first(nof_bits_dmrs), input.first(nof_bits_dmrs));
      sch_data = sch_data.last(sch_data.size() - nof_bits_dmrs);
      input    = input.last(input.size() - nof_bits_dmrs);
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

    // Calculate the remainder of reserved bits.
    unsigned G_ack_rvd_remainder = G_ack_rvd - m_rvd_count;

    // Calculate the remainder of HARQ-ACK bits.
    unsigned G_ack_remainder = harq_ack.size();

    // Prepare HARQ-ACK and reserved bits.
    if (i_symbol >= l1) {
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

    // Stride and counter for the number of CSI part 1 bits multiplexed in the OFDM symbol.
    unsigned csi1_d          = 0;
    unsigned csi1_m_re_count = 0;

    // Stride and counter for the number of CSI part 2 bits multiplexed in the OFDM symbol.
    unsigned csi2_d          = 0;
    unsigned csi2_m_re_count = 0;

    // Prepare CSI Part1 and Part2 reserved bits.
    if (i_symbol >= l1_csi) {
      // Calculate the remainder of CSI Part1 bits.
      unsigned G_csi1_remainder = csi_part1.size();

      // Calculate the remainder of CSI Part2 bits.
      unsigned G_csi2_remainder = csi_part2.size();

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
      if ((M_uci_sc > M_uci_rvd) && (G_csi2_remainder != 0)) {
        csi2_d          = 1;
        csi2_m_re_count = M_uci_sc - M_uci_rvd;
        if (G_csi2_remainder < (M_uci_sc - M_uci_rvd) * nof_bits_per_re) {
          csi2_d          = ((M_uci_sc - M_uci_rvd) * nof_bits_per_re) / G_csi2_remainder;
          csi2_m_re_count = divide_ceil(G_csi2_remainder, nof_bits_per_re);
        }
        M_uci_sc -= csi2_m_re_count;
      }
    }

    // Leave the rest of resource elements for UL-SCH.
    unsigned ulsch_m_re_count = M_uci_sc;

    // Iterate over each subcarrier within the OFDM symbol.
    for (unsigned i_subcarrier = 0, i_csi1 = 0, i_csi2 = 0, i_rvd = 0; i_subcarrier != M_ulsch_sc; ++i_subcarrier) {
      // Check if RE is reserved.
      if ((rvd_m_re_count != 0) && (i_subcarrier % rvd_d == 0) && (m_rvd_count < G_ack_rvd)) {
        // Process HARQ-ACK.
        if ((ack_m_re_count != 0) && ((i_rvd++) % ack_d == 0) && !harq_ack.empty()) {
          // Multiplex HARQ-ACK.
          srsvec::copy(harq_ack.first(nof_bits_per_re), input.first(nof_bits_per_re));
          harq_ack = harq_ack.last(harq_ack.size() - nof_bits_per_re);

          // Set SCH bits to zero.
          srsvec::zero(sch_data.first(nof_bits_per_re));

          // Decrement number of pending HARQ-ACK bits RE for the symbol.
          --ack_m_re_count;
        } else {
          // Multiplex SCH data.
          srsvec::copy(sch_data.first(nof_bits_per_re), input.first(nof_bits_per_re));
        }

        // Advance input and SCH data buffers.
        input    = input.last(input.size() - nof_bits_per_re);
        sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);

        // Increment reserved bits count.
        m_rvd_count += nof_bits_per_re;

        // Decrement number of pending RE for reserved HARQ-ACK.
        --rvd_m_re_count;

        // Decrement number of pending SCH data RE for the symbol.
        --ulsch_m_re_count;

        continue;
      }

      // Check if the RE is for HARQ-ACK if no reserved bits.
      if ((G_ack_rvd == 0) && (ack_m_re_count != 0) && (i_subcarrier % ack_d == 0) && !harq_ack.empty()) {
        // Multiplex HARQ-ACK.
        srsvec::copy(harq_ack.first(nof_bits_per_re), input.first(nof_bits_per_re));
        harq_ack = harq_ack.last(harq_ack.size() - nof_bits_per_re);
        input    = input.last(input.size() - nof_bits_per_re);

        // Decrement number of pending HARQ-ACK RE for the symbol.
        --ack_m_re_count;

        continue;
      }

      // Process CSI-Part1.
      if ((csi1_m_re_count != 0) && ((i_csi1++) % csi1_d == 0) && !csi_part1.empty()) {
        // Multiplex CSI-Part1.
        srsvec::copy(csi_part1.first(nof_bits_per_re), input.first(nof_bits_per_re));
        csi_part1 = csi_part1.last(csi_part1.size() - nof_bits_per_re);
        input     = input.last(input.size() - nof_bits_per_re);

        // Decrement number of pending CSI-Part1 RE for the symbol.
        --csi1_m_re_count;

        continue;
      }

      // Process CSI-Part2.
      if ((csi2_m_re_count != 0) && ((i_csi2++) % csi2_d == 0) && !csi_part2.empty()) {
        // Multiplex CSI-Part2.
        srsvec::copy(csi_part2.first(nof_bits_per_re), input.first(nof_bits_per_re));
        csi_part2 = csi_part2.last(csi_part2.size() - nof_bits_per_re);
        input     = input.last(input.size() - nof_bits_per_re);

        // Decrement number of pending CSI-Part2 RE for the symbol.
        --csi2_m_re_count;

        continue;
      }

      // Multiplex SCH data.
      srsvec::copy(sch_data.first(nof_bits_per_re), input.first(nof_bits_per_re));
      sch_data = sch_data.last(sch_data.size() - nof_bits_per_re);
      input    = input.last(input.size() - nof_bits_per_re);

      // Decrement number of pending SCH data RE for the symbol.
      --ulsch_m_re_count;
    }

    // Assert that all RE have been allocated
    srsgnb_assert(ack_m_re_count == 0, "{} RE for HARQ-ACK are not processed.", ack_m_re_count);
    srsgnb_assert(csi1_m_re_count == 0, "{} RE for CSI-Part1 are not processed.", csi1_m_re_count);
    srsgnb_assert(csi2_m_re_count == 0, "{} RE for CSI-Part2 are not processed.", csi2_m_re_count);
    srsgnb_assert(ulsch_m_re_count == 0, "{} RE for SCH data are not processed.", ulsch_m_re_count);
  }

  // Assert Number of bits
  srsgnb_assert(harq_ack.empty(), "{} soft bits have not been multiplexed for HARQ-ACK.", harq_ack.size());
  srsgnb_assert(csi_part1.empty(), "{} soft bits have not been multiplexed for CSI-Part1.", csi_part1.size());
  srsgnb_assert(csi_part2.empty(), "{} soft bits have not been multiplexed for CSI-Part2.", csi_part2.size());
  srsgnb_assert(sch_data.empty(), "{} soft bits have not been multiplexed for SCH data.", sch_data.size());
  srsgnb_assert(input.empty(), "{} input soft bits have not been multiplexed.", input.size());
}

} // namespace

void ulsch_demultiplex_impl::demultiplex(span<log_likelihood_ratio>              sch_data,
                                         span<log_likelihood_ratio>              harq_ack,
                                         span<log_likelihood_ratio>              csi_part1,
                                         span<log_likelihood_ratio>              csi_part2,
                                         span<const log_likelihood_ratio>        input,
                                         const ulsch_demultiplex::configuration& config)
{
  ulsch_demultiplex_generic(sch_data, harq_ack, csi_part1, csi_part2, input, config);
}
