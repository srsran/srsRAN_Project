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

#pragma once

#include "pusch_codeword_buffer_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/ran/pusch/ulsch_info.h"

namespace srsran {

class ulsch_demultiplex_spy : public ulsch_demultiplex
{
public:
  struct demultiplex_entry {
    std::vector<log_likelihood_ratio> sch_data;
    std::vector<log_likelihood_ratio> harq_ack;
    std::vector<log_likelihood_ratio> csi_part1;
    std::vector<log_likelihood_ratio> csi_part2;
    unsigned                          nof_csi_part2_bits;
    pusch_codeword_buffer_spy         input;
    configuration                     config;
  };

  ulsch_demultiplex_spy() : softbit_dist(log_likelihood_ratio::min().to_int(), log_likelihood_ratio::max().to_int())
  {
    // Do nothing.
  }

  void set_ulsch_config(unsigned nof_codeword_llr_,
                        unsigned nof_ul_sch_bits_,
                        unsigned nof_harq_ack_enc_bits_,
                        unsigned nof_csi_part1_enc_bits_,
                        unsigned nof_csi_part2_enc_bits_)
  {
    nof_codeword_llr       = nof_codeword_llr_;
    nof_ul_sch_bits        = nof_ul_sch_bits_;
    nof_harq_ack_enc_bits  = nof_harq_ack_enc_bits_;
    nof_csi_part1_enc_bits = nof_csi_part1_enc_bits_;
    nof_csi_part2_enc_bits = nof_csi_part2_enc_bits_;
  }

  void
  set_csi_part2(pusch_decoder_buffer& csi_part2, unsigned nof_csi_part2_bits, unsigned nof_csi_part2_enc_bits_) override
  {
    srsran_assert(nof_csi_part2_enc_bits == nof_csi_part2_enc_bits_,
                  "Number of CSI Part 2 encoded bits do not match (i.e., {} != {}).",
                  nof_csi_part2_enc_bits_,
                  nof_csi_part2_enc_bits);

    demultiplex_entry& entry = demultiplex_entries.back();
    entry.nof_csi_part2_bits = nof_csi_part2_bits;
    entry.csi_part2.resize(nof_csi_part2_enc_bits_);
    generate_softbits(entry.csi_part2);

    csi_part2.on_new_softbits(entry.csi_part2);
    csi_part2.on_end_softbits();
  }

  pusch_codeword_buffer& demultiplex(pusch_decoder_buffer& sch_data,
                                     pusch_decoder_buffer& harq_ack,
                                     pusch_decoder_buffer& csi_part1,
                                     const configuration&  config) override
  {
    demultiplex_entries.emplace_back();

    demultiplex_entry& entry = demultiplex_entries.back();
    entry.config             = config;

    // Set codeword input.
    entry.input = pusch_codeword_buffer_spy(nof_codeword_llr);

    // Set SCH data.
    entry.sch_data.resize(nof_ul_sch_bits);
    generate_softbits(entry.sch_data);
    sch_data.on_new_softbits(entry.sch_data);
    sch_data.on_end_softbits();

    // Set HARQ-ACK data.
    entry.harq_ack.resize(nof_harq_ack_enc_bits);
    generate_softbits(entry.harq_ack);
    harq_ack.on_new_softbits(entry.harq_ack);
    harq_ack.on_end_softbits();

    // Set CSI Part 1 data.
    entry.csi_part1.resize(nof_csi_part1_enc_bits);
    generate_softbits(entry.csi_part1);
    csi_part1.on_new_softbits(entry.csi_part1);
    csi_part1.on_end_softbits();

    return entry.input;
  }

  const std::vector<demultiplex_entry>& get_demultiplex_entries() const { return demultiplex_entries; }

  void clear() { demultiplex_entries.clear(); }

private:
  void generate_softbits(span<log_likelihood_ratio> softbits)
  {
    std::generate(softbits.begin(), softbits.end(), [this]() { return log_likelihood_ratio(softbit_dist(rgen)); });
  }

  unsigned                              nof_codeword_llr       = 0;
  unsigned                              nof_ul_sch_bits        = 0;
  unsigned                              nof_harq_ack_enc_bits  = 0;
  unsigned                              nof_csi_part1_enc_bits = 0;
  unsigned                              nof_csi_part2_enc_bits = 0;
  std::mt19937                          rgen;
  std::uniform_int_distribution<int8_t> softbit_dist;
  std::vector<demultiplex_entry>        demultiplex_entries;
};

PHY_SPY_FACTORY_TEMPLATE(ulsch_demultiplex);

} // namespace srsran