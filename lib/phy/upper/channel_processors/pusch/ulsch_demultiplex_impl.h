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

#pragma once

#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/ran/pusch/pusch_constants.h"

namespace srsran {

class ulsch_demultiplex_impl : public ulsch_demultiplex, private pusch_codeword_buffer
{
public:
  /// Resource element set type.
  using re_set_type = bounded_bitset<MAX_RB * NRE>;

  // See interface for documentation.
  void
  set_csi_part2(pusch_decoder_buffer& csi_part2, unsigned nof_csi_part2_bits, unsigned nof_csi_part2_enc_bits) override;

  // See interface for documentation.
  pusch_codeword_buffer& demultiplex(pusch_decoder_buffer& sch_data,
                                     pusch_decoder_buffer& harq_ack,
                                     pusch_decoder_buffer& csi_part1,
                                     const configuration&  config) override;

private:
  static constexpr unsigned max_nof_bits_ofdm_symbol =
      MAX_RB * NRE * pusch_constants::MAX_MODULATION_ORDER * pusch_constants::MAX_NOF_LAYERS;

  // See pusch_codeword_buffer for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override;

  // See pusch_codeword_buffer for documentation.
  void on_new_block(span<const log_likelihood_ratio> new_data, const bit_buffer& new_scrambling_seq) override;

  // See pusch_codeword_buffer for documentation.
  void on_end_codeword() override;

  pusch_decoder_buffer* sch_data  = nullptr;
  pusch_decoder_buffer* harq_ack  = nullptr;
  pusch_decoder_buffer* csi_part1 = nullptr;
  pusch_decoder_buffer* csi_part2 = nullptr;

  /// Configures the current OFDM symbol.
  void configure_current_ofdm_symbol();

  /// Configures CSI Part 2 for the current OFDM symbol.
  void configure_csi_part2_current_ofdm_symbol();

  /// Demultiplexes the current OFDM symbol.
  void demux_current_ofdm_symbol();

  /// Transmission configuration.
  configuration config;
  /// Number of CSI Part 2 information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{CSI-2}\f$.
  unsigned nof_csi_part2_bits;
  /// Number of encoded and rate-matched CSI Part 2 data bits. Parameter \f$G^\textup{CSI-2}\f$.
  unsigned nof_csi_part2_enc_bits;
  /// Number of bits per resource element.
  unsigned nof_bits_per_re;
  /// OFDM symbol index of the first OFDM symbol after the first set of consecutive OFDM symbol(s) carrying DM-RS in the
  /// first hop.
  unsigned l1;
  /// OFDM symbol index of the first OFDM symbol that does not carry DM-RS in the first hop.
  unsigned l1_csi;
  /// Number of UL-SCH REs in OFDM symbols containing DM-RS.
  unsigned nof_re_dmrs;
  /// Count the number of reserved bits.
  unsigned m_rvd_count;
  /// Count the number of HARQ-ACK bits.
  unsigned m_harq_ack_count;
  /// Count the number of CSI Part1 bits.
  unsigned m_csi_part1_count;
  /// Count the number of CSI Part2 bits.
  unsigned m_csi_part2_count;
  /// Current OFDM symbol index.
  unsigned ofdm_symbol_index;
  /// Number of softbits available for UL-SCH in the current OFDM symbol.
  unsigned nof_softbits;
  /// Keeps count of softbits in \c temp_softbit_buffer.
  unsigned softbit_count;
  /// Parameter \f$\Phi^\textup{UL-SCH}_\textup{l}\f$. Set of resource elements available for transmission of data in
  /// the OFDM symbol \f$l\f$.
  re_set_type ulsch_re_set;
  /// Parameter \f$\Phi^\textup{UCI}_\textup{l}\f$. Set of resource elements available for transmission of UCI in the
  /// OFDM symbol \f$l\f$.
  re_set_type uci_re_set;
  /// Parameter \f$\Phi^\textup{rvd}_\textup{l}\f$. Set of reserved resource elements for potential HARQ-ACK
  /// transmission.
  re_set_type rvd_re_set;
  /// Set of resource elements for HARQ-ACK transmission.
  re_set_type harq_ack_re_set;
  /// Set of resource elements for CSI Part 1 transmission.
  re_set_type csi_part1_re_set;
  /// Set of resource elements for CSI Part 2 transmission.
  re_set_type csi_part2_re_set;
  /// Demodulated soft bits buffer for an entire OFDM symbol.
  std::array<log_likelihood_ratio, max_nof_bits_ofdm_symbol> temp_data_ofdm_symbol;
  /// Scrambling sequence for an entire OFDM symbol.
  static_bit_buffer<max_nof_bits_ofdm_symbol> temp_scrambling_seq_ofdm_symbol;
};

} // namespace srsran
