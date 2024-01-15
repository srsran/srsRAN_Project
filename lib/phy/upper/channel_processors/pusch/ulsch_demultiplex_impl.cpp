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

#include "ulsch_demultiplex_impl.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

static unsigned get_ulsch_demultiplex_l1(const symbol_slot_mask& dmrs_symbol_mask)
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

static unsigned get_ulsch_demultiplex_l1_csi(const symbol_slot_mask& dmrs_symbol_mask)
{
  // Find first OFDM symbol that does not contain DM-RS.
  int first_symbol_without_dmrs = dmrs_symbol_mask.find_lowest(false);
  srsran_assert(first_symbol_without_dmrs >= 0, "No DM-RS symbol found.");

  return static_cast<unsigned>(first_symbol_without_dmrs);
}

static unsigned
get_ulsch_demultiplex_nof_re_prb_dmrs(dmrs_type dmrs_, unsigned nof_cdm_groups_without_data, unsigned nof_prb)
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

// Selects from re_set the first m_re_count elements skipping d.
static ulsch_demultiplex_impl::re_set_type
re_set_select(const ulsch_demultiplex_impl::re_set_type& re_set, unsigned d, unsigned m_re_count)
{
  bounded_bitset<MAX_RB * NRE> result(re_set.size());

  for (unsigned count = 0, startpos = 0, d_count = 0; count != m_re_count;) {
    // Find next available position.
    int found = re_set.find_lowest(startpos, re_set.size());
    srsran_assert(found >= 0, "It must always find a true.");

    if (d_count % d == 0) {
      result.set(found);
      ++count;
    }
    ++d_count;

    // Update starting position.
    startpos = found + 1;
  }

  return result;
}

// Creates another resource element set without any selection of the same size.
static bounded_bitset<MAX_RB * NRE> re_set_copy_size(const bounded_bitset<MAX_RB * NRE>& re_set)
{
  bounded_bitset<MAX_RB * NRE> result(re_set.size());
  return result;
}

static void on_uci_placeholder_1bit(pusch_decoder_buffer&            buffer,
                                    span<const log_likelihood_ratio> data,
                                    const bit_buffer&                scrambling_seq,
                                    unsigned                         scrambling_seq_offset,
                                    modulation_scheme                modulation)
{
  unsigned modulation_order = get_bits_per_symbol(modulation);
  unsigned block_size       = data.size();
  srsran_assert(block_size % modulation_order == 0, "Input size and modulation order are not concistent.");
  srsran_assert(scrambling_seq.size() >= block_size + scrambling_seq_offset, "Scrambling sequence size is invalid.");

  unsigned nof_symbols = block_size / modulation_order;

  // Skip if modulation order is one.
  if (modulation_order == 1) {
    buffer.on_new_softbits(data);
    return;
  }

  static_vector<log_likelihood_ratio, pusch_constants::MAX_MODULATION_ORDER> temp(modulation_order);

  // Iterate each symbol within the block.
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    // First soft bit is copied.
    temp[0] = data[0];

    // Extract the first two soft bits mask.
    uint8_t mask0 = scrambling_seq.extract(scrambling_seq_offset++, 1);
    uint8_t mask1 = scrambling_seq.extract(scrambling_seq_offset++, 1);

    // Placeholder y: revert the scrambling of the second bit and apply the same mask of the first bit.
    temp[1] = ((mask0 ^ mask1) == 1) ? -data[1] : data[1];

    // Placeholder x: revert scrambling.
    for (unsigned i_bit = 2; i_bit != modulation_order; ++i_bit) {
      uint8_t mask = scrambling_seq.extract(scrambling_seq_offset++, 1);
      temp[i_bit]  = (mask == 1) ? -data[i_bit] : data[i_bit];
    }

    // Advance buffers.
    data = data.last(data.size() - modulation_order);

    // Notify result.
    buffer.on_new_softbits(temp);
  }
}

static void on_uci_placeholder_2bit(pusch_decoder_buffer&            buffer,
                                    span<const log_likelihood_ratio> data,
                                    const bit_buffer&                scrambling_seq,
                                    unsigned                         scrambling_seq_offset,
                                    modulation_scheme                modulation)
{
  unsigned modulation_order = get_bits_per_symbol(modulation);
  unsigned block_size       = data.size();
  srsran_assert(block_size % modulation_order == 0, "Input size and modulation order are not consistent.");
  srsran_assert(scrambling_seq.size() >= block_size + scrambling_seq_offset, "Scrambling sequence size is invalid.");

  unsigned nof_symbols = block_size / modulation_order;

  // Skip if modulation order is one.
  if (modulation_order == 1) {
    buffer.on_new_softbits(data);
    return;
  }

  static_vector<log_likelihood_ratio, pusch_constants::MAX_MODULATION_ORDER> temp(modulation_order);

  // Iterate each symbol within the block.
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    // First two soft bits are copied.
    temp[0] = data[0];
    temp[1] = data[1];

    // Increment offset.
    scrambling_seq_offset += 2;

    // Placeholder x: revert scrambling.
    for (unsigned i_bit = 2; i_bit != modulation_order; ++i_bit) {
      uint8_t mask = scrambling_seq.extract(scrambling_seq_offset++, 1);
      temp[i_bit]  = (mask == 1) ? -data[i_bit] : data[i_bit];
    }

    // Advance buffers.
    data = data.last(data.size() - modulation_order);

    // Notify result.
    buffer.on_new_softbits(temp);
  }
}

pusch_codeword_buffer& ulsch_demultiplex_impl::demultiplex(pusch_decoder_buffer& sch_data_,
                                                           pusch_decoder_buffer& harq_ack_,
                                                           pusch_decoder_buffer& csi_part1_,
                                                           const configuration&  config_)
{
  // Copy configuration.
  config = config_;

  // Prepare the decoder buffers.
  sch_data = &sch_data_;
  if (config.nof_harq_ack_bits != 0) {
    harq_ack = &harq_ack_;
  } else {
    harq_ack = nullptr;
  }
  if (config.nof_csi_part1_bits != 0) {
    csi_part1 = &csi_part1_;
  } else {
    csi_part1 = nullptr;
  }
  csi_part2 = nullptr;

  // Calculate derivative parameters.
  nof_bits_per_re = get_bits_per_symbol(config.modulation) * config.nof_layers;
  l1              = get_ulsch_demultiplex_l1(config.dmrs_symbol_mask);
  l1_csi          = get_ulsch_demultiplex_l1_csi(config.dmrs_symbol_mask);
  nof_re_dmrs = get_ulsch_demultiplex_nof_re_prb_dmrs(config.dmrs, config.nof_cdm_groups_without_data, config.nof_prb);

  // CSI Part 2 is not configured initially.
  nof_csi_part2_bits     = 0;
  nof_csi_part2_enc_bits = 0;

  // Initialise counters.
  ofdm_symbol_index = config.start_symbol_index;
  m_rvd_count       = 0;
  m_harq_ack_count  = 0;
  m_csi_part1_count = 0;
  m_csi_part2_count = 0;

  // Configure the first OFDM symbol.
  configure_current_ofdm_symbol();

  return *this;
}

void ulsch_demultiplex_impl::set_csi_part2(pusch_decoder_buffer& csi_part2_,
                                           unsigned              nof_csi_part2_bits_,
                                           unsigned              nof_csi_part2_enc_bits_)
{
  csi_part2              = &csi_part2_;
  nof_csi_part2_bits     = nof_csi_part2_bits_;
  nof_csi_part2_enc_bits = nof_csi_part2_enc_bits_;

  // Re-configure current OFDM symbol.
  configure_csi_part2_current_ofdm_symbol();
}

span<log_likelihood_ratio> ulsch_demultiplex_impl::get_next_block_view(unsigned block_size)
{
  // Bypass buffer if no UCI and no samples are stored in the buffer.
  if ((softbit_count == 0) && (harq_ack == nullptr) && (csi_part1 == nullptr) && (csi_part2 == nullptr) &&
      (sch_data != nullptr)) {
    return sch_data->get_next_block_view(block_size);
  }

  // Limit block size to accommodate the rest of the current OFDM symbol.
  block_size = std::min(block_size, nof_softbits - softbit_count);
  return span<log_likelihood_ratio>(temp_data_ofdm_symbol).subspan(softbit_count, block_size);
}

void ulsch_demultiplex_impl::on_new_block(span<const log_likelihood_ratio> new_data,
                                          const bit_buffer&                new_scrambling_seq)
{
  srsran_assert(new_data.size() == new_scrambling_seq.size(), "Demodulated and descrambled data sizes must be equal.");

  // New scrambling sequence read offset.
  unsigned new_scrambling_seq_offset = 0;

  // Repeat processing while there are soft bits to process.
  while (!new_data.empty()) {
    // Bypass buffer if no UCI and no samples are stored in the buffer.
    if ((softbit_count == 0) && (harq_ack == nullptr) && (csi_part1 == nullptr) && (csi_part2 == nullptr) &&
        (sch_data != nullptr)) {
      return sch_data->on_new_softbits(new_data);
    }

    unsigned block_size = std::min(static_cast<unsigned>(new_data.size()), nof_softbits - softbit_count);
    span<log_likelihood_ratio> data_block =
        span<log_likelihood_ratio>(temp_data_ofdm_symbol).subspan(softbit_count, block_size);

    // Copy new data (if necessary) at the end of the buffer.
    if (data_block.data() != new_data.data()) {
      srsvec::copy(data_block, new_data.first(block_size));
    }

    // Append scrambling sequence.
    srsvec::copy_offset(
        temp_scrambling_seq_ofdm_symbol, softbit_count, new_scrambling_seq, new_scrambling_seq_offset, block_size);

    // Increment count of soft bits for the current OFDM symbol.
    softbit_count += block_size;

    // Advance new data and scrambling sequence offset.
    new_data = new_data.last(new_data.size() - block_size);
    new_scrambling_seq_offset += block_size;

    // Demultiplex if the OFDM symbol is full.
    if (softbit_count == nof_softbits) {
      demux_current_ofdm_symbol();

      // Advance OFDM symbol to the next one containing data.
      nof_softbits = 0;
      while ((nof_softbits == 0) && (ofdm_symbol_index != (config.start_symbol_index + config.nof_symbols))) {
        // Increment OFDM symbol.
        ++ofdm_symbol_index;

        // Configure symbol.
        if (ofdm_symbol_index != (config.start_symbol_index + config.nof_symbols)) {
          configure_current_ofdm_symbol();
        }
      }
    }
  }
}

void ulsch_demultiplex_impl::on_end_codeword()
{
  srsran_assert(harq_ack == nullptr, "Not all HARQ-ACK bits have been processed.");
  srsran_assert(csi_part1 == nullptr, "Not all CSI Part 1 bits have been processed.");
  srsran_assert(csi_part2 == nullptr, "Not all CSI Part 2 bits have been processed.");

  sch_data->on_end_softbits();
  sch_data = nullptr;
}

void ulsch_demultiplex_impl::configure_current_ofdm_symbol()
{
  // Calculate the number of RE available for UL-SCH.
  unsigned M_ulsch = config.nof_prb * NRE;

  // If the OFDM symbol contain DM-RS, subtract the number of RE for DM-RS.
  bool contain_dm_rs = config.dmrs_symbol_mask.test(ofdm_symbol_index);
  if (contain_dm_rs) {
    M_ulsch = nof_re_dmrs;
  }
  nof_softbits = M_ulsch * nof_bits_per_re;
  ulsch_re_set.resize(M_ulsch);

  // Initially all RE are available for UL-SCH.
  ulsch_re_set.fill(0, M_ulsch, true);

  // UCI can only be allocated if DM-RS are not transmitted in the OFDM symbol.
  uci_re_set.resize(M_ulsch);
  uci_re_set.fill(0, M_ulsch, !contain_dm_rs);

  rvd_re_set       = re_set_copy_size(ulsch_re_set);
  harq_ack_re_set  = re_set_copy_size(ulsch_re_set);
  csi_part1_re_set = re_set_copy_size(ulsch_re_set);
  csi_part2_re_set = re_set_copy_size(ulsch_re_set);

  // Step 1. Reserve potential REs for HARQ-ACK.
  unsigned M_uci                  = uci_re_set.count();
  unsigned remainder_harq_ack_rvd = (config.nof_harq_ack_rvd - m_rvd_count) / nof_bits_per_re;
  if ((ofdm_symbol_index >= l1) && (M_uci > 0) && (remainder_harq_ack_rvd > 0)) {
    unsigned d          = 1;
    unsigned m_re_count = M_uci;
    if (remainder_harq_ack_rvd < M_uci) {
      d          = M_uci / remainder_harq_ack_rvd;
      m_re_count = remainder_harq_ack_rvd;
    }

    // Select reserved REs.
    rvd_re_set = re_set_select(ulsch_re_set, d, m_re_count);

    // Increment count of reserved elements.
    m_rvd_count += m_re_count * nof_bits_per_re;
  }

  // Step 2. Allocate HARQ-ACK if the number of HARQ-ACK message bits is greater than two.
  unsigned remainder_harq_ack = (config.nof_enc_harq_ack_bits - m_harq_ack_count) / nof_bits_per_re;
  if ((ofdm_symbol_index >= l1) && (M_uci > 0) && (config.nof_harq_ack_bits > 2) && (remainder_harq_ack > 0)) {
    unsigned d          = 1;
    unsigned m_re_count = M_uci;
    if (remainder_harq_ack < M_uci) {
      d          = M_uci / remainder_harq_ack;
      m_re_count = remainder_harq_ack;
    }

    // Select HARQ-ACK REs.
    harq_ack_re_set = re_set_select(uci_re_set, d, m_re_count);

    // Remove HARQ-ACK REs from the UL-SCH and UCI sets.
    ulsch_re_set &= ~harq_ack_re_set;
    uci_re_set &= ~harq_ack_re_set;

    // Recalculate the number of available REs for UCI.
    M_uci = uci_re_set.count();

    // Increment count of HARQ-ACK elements.
    m_harq_ack_count += m_re_count * nof_bits_per_re;
  }

  // Step 3. Allocate CSI Part 1.
  unsigned remainder_csi_part1 = (config.nof_enc_csi_part1_bits - m_csi_part1_count) / nof_bits_per_re;
  unsigned M_rvd               = rvd_re_set.count();
  if ((ofdm_symbol_index >= l1_csi) && ((M_uci - M_rvd) > 0) && (remainder_csi_part1 > 0)) {
    unsigned d          = 1;
    unsigned m_re_count = M_uci - M_rvd;
    if (remainder_csi_part1 < (M_uci - M_rvd)) {
      d          = (M_uci - M_rvd) / remainder_csi_part1;
      m_re_count = remainder_csi_part1;
    }

    // Prepare potential CSI Part 1 RE set.
    re_set_type temp_re_set = ~rvd_re_set;
    temp_re_set &= uci_re_set;

    // Select CSI Part 1 REs.
    csi_part1_re_set = re_set_select(temp_re_set, d, m_re_count);

    // Remove CSI Part 1 REs from the UL-SCH and UCI sets.
    ulsch_re_set &= ~csi_part1_re_set;
    uci_re_set &= ~csi_part1_re_set;

    // Increment count of CSI Part 1 elements.
    m_csi_part1_count += m_re_count * nof_bits_per_re;
  }

  // Step 3bis. Allocate CSI Part 2.
  configure_csi_part2_current_ofdm_symbol();

  // Step 4. Allocate UL-SCH.
  // Nothing to do.

  // Step 5. Allocate HARQ-ACK for 2 or less message bits.
  if ((M_rvd > 0) && (config.nof_harq_ack_bits <= 2) && (remainder_harq_ack > 0)) {
    unsigned d          = 1;
    unsigned m_re_count = M_rvd;
    if (remainder_harq_ack < M_rvd) {
      d          = M_rvd / remainder_harq_ack;
      m_re_count = remainder_harq_ack;
    }

    // Select HARQ ACK REs.
    harq_ack_re_set = re_set_select(rvd_re_set, d, m_re_count);

    // Increment count of HARQ-ACK elements.
    m_harq_ack_count += m_re_count * nof_bits_per_re;
  }

  // Prepare symbol buffers.
  temp_scrambling_seq_ofdm_symbol.resize(nof_softbits);
}

void ulsch_demultiplex_impl::configure_csi_part2_current_ofdm_symbol()
{
  unsigned M_uci               = uci_re_set.count();
  unsigned remainder_csi_part2 = (nof_csi_part2_enc_bits - m_csi_part2_count) / nof_bits_per_re;
  if ((ofdm_symbol_index >= l1_csi) && (M_uci > 0) && (remainder_csi_part2 > 0)) {
    unsigned d          = 1;
    unsigned m_re_count = M_uci;
    if (remainder_csi_part2 < M_uci) {
      d          = M_uci / remainder_csi_part2;
      m_re_count = remainder_csi_part2;
    }

    // Select CSI Part 2 REs.
    csi_part2_re_set = re_set_select(uci_re_set, d, m_re_count);

    // Remove CSI Part 2 REs from the UL-SCH and UCI sets.
    ulsch_re_set &= ~csi_part2_re_set;
    uci_re_set &= ~csi_part2_re_set;

    // Increment count of CSI Part 2 elements.
    m_csi_part2_count += m_re_count * nof_bits_per_re;
  }
}

void ulsch_demultiplex_impl::demux_current_ofdm_symbol()
{
  // Select view of the softbits.
  span<log_likelihood_ratio> data           = span<log_likelihood_ratio>(temp_data_ofdm_symbol).first(nof_softbits);
  const bit_buffer           scrambling_seq = temp_scrambling_seq_ofdm_symbol.first(nof_softbits);

  // Demultiplex HARQ-ACK.
  if (harq_ack_re_set.any()) {
    srsran_assert(harq_ack != nullptr, "Invalid HARQ-ACK decoder buffer.");

    // Extract each of the HARQ-ACK bits.
    harq_ack_re_set.for_each(0, harq_ack_re_set.size(), [this, &data, &scrambling_seq](unsigned i_re) {
      // Extract softbits.
      unsigned                   offset  = i_re * nof_bits_per_re;
      span<log_likelihood_ratio> re_data = data.subspan(offset, nof_bits_per_re);

      // Handle HARQ-ACK placeholders for 1 bit.
      if (config.nof_harq_ack_bits == 1) {
        on_uci_placeholder_1bit(*harq_ack, re_data, scrambling_seq, offset, config.modulation);
        srsvec::zero(re_data);
        return;
      }

      // Handle HARQ-ACK placeholders for 2 bits.
      if (config.nof_harq_ack_bits == 2) {
        on_uci_placeholder_2bit(*harq_ack, re_data, scrambling_seq, offset, config.modulation);
        srsvec::zero(re_data);
        return;
      }

      harq_ack->on_new_softbits(re_data);
    });

    // Notify the end of HARQ-ACK processing bits.
    if (m_harq_ack_count == config.nof_enc_harq_ack_bits) {
      harq_ack->on_end_softbits();
      harq_ack = nullptr;
    }
  }

  // Demultiplex CSI Part 1.
  if (csi_part1_re_set.any()) {
    srsran_assert(csi_part1 != nullptr, "Invalid CSI Part 1 decoder buffer.");

    // Extract each of the CSI Part 1 bits.
    csi_part1_re_set.for_each(0, csi_part1_re_set.size(), [this, &data, &scrambling_seq](unsigned i_re) {
      // Extract softbits.
      unsigned                   offset  = i_re * nof_bits_per_re;
      span<log_likelihood_ratio> re_data = data.subspan(offset, nof_bits_per_re);

      // Handle CSI Part 1 placeholders for 1 bit.
      if (config.nof_csi_part1_bits == 1) {
        on_uci_placeholder_1bit(*csi_part1, re_data, scrambling_seq, offset, config.modulation);
        return;
      }

      // Handle CSI Part 1 placeholders for 2 bits.
      if (config.nof_csi_part1_bits == 2) {
        on_uci_placeholder_2bit(*csi_part1, re_data, scrambling_seq, offset, config.modulation);
        return;
      }

      csi_part1->on_new_softbits(re_data);
    });

    // Notify the end of CSI Part 1 processing bits.
    if (m_csi_part1_count == config.nof_enc_csi_part1_bits) {
      csi_part1->on_end_softbits();
      csi_part1 = nullptr;
    }
  }

  // Demultiplex CSI Part 2.
  if (csi_part2_re_set.any()) {
    srsran_assert(csi_part2 != nullptr, "Invalid CSI Part 2 decoder buffer.");

    // Extract each of the CSI Part 2 bits.
    csi_part2_re_set.for_each(0, csi_part2_re_set.size(), [this, &data, &scrambling_seq](unsigned i_re) {
      // Extract softbits.
      unsigned                   offset  = i_re * nof_bits_per_re;
      span<log_likelihood_ratio> re_data = data.subspan(offset, nof_bits_per_re);

      // Handle CSI Part 2 placeholders for 1 bit.
      if (nof_csi_part2_bits == 1) {
        on_uci_placeholder_1bit(*csi_part2, re_data, scrambling_seq, offset, config.modulation);
        return;
      }

      // Handle CSI Part 2 placeholders for 2 bits.
      if (nof_csi_part2_bits == 2) {
        on_uci_placeholder_2bit(*csi_part2, re_data, scrambling_seq, offset, config.modulation);
        return;
      }

      csi_part2->on_new_softbits(re_data);
    });

    // Notify the end of CSI Part 2 processing bits.
    if (m_csi_part2_count == nof_csi_part2_enc_bits) {
      csi_part2->on_end_softbits();
      csi_part2 = nullptr;
    }
  }

  // Demultiplex SCH data.
  if (ulsch_re_set.any()) {
    srsran_assert(sch_data != nullptr, "Invalid SCH data decoder buffer.");

    // Extract each of the UL-SCH data.
    ulsch_re_set.for_each(0, ulsch_re_set.size(), [this, &data](unsigned i_re) {
      sch_data->on_new_softbits(data.subspan(i_re * nof_bits_per_re, nof_bits_per_re));
    });
  }

  // Empty buffer.
  softbit_count = 0;
}