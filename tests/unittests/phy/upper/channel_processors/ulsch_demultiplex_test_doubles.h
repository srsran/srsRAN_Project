/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_processors/ulsch_demultiplex.h"

namespace srsran {

class ulsch_demultiplex_spy : public ulsch_demultiplex
{
public:
  struct demultiplex_harq_ack_and_csi_part1_entry {
    span<const log_likelihood_ratio> harq_ack;
    span<const log_likelihood_ratio> csi_part1;
    span<const log_likelihood_ratio> input;
    configuration                    config;
  };

  struct demultiplex_sch_and_csi_part2_entry {
    span<const log_likelihood_ratio> sch_data;
    span<const log_likelihood_ratio> csi_part2;
    span<const log_likelihood_ratio> input;
    unsigned                         nof_enc_harq_ack_bits;
    unsigned                         nof_csi_part1_bits;
    configuration                    config;
  };

  struct demultiplex_entry {
    span<const log_likelihood_ratio> sch_data;
    span<const log_likelihood_ratio> harq_ack;
    span<const log_likelihood_ratio> csi_part1;
    span<const log_likelihood_ratio> csi_part2;
    span<const log_likelihood_ratio> input;
    configuration                    config;
  };

  struct placeholders_entry {
    message_information    msg_info;
    configuration          config;
    ulsch_placeholder_list list;
  };

  ulsch_demultiplex_spy() : placeholder_dist(0, ulsch_placeholder_list::MAX_NOF_PLACEHOLDERS - 1)
  {
    // Do nothing.
  }

  void demultiplex_harq_ack_and_csi_part1(span<log_likelihood_ratio>       harq_ack,
                                          span<log_likelihood_ratio>       csi_part1,
                                          span<const log_likelihood_ratio> input,
                                          const configuration&             config) override
  {
    demultiplex_harq_ack_and_csi_part1_entries.emplace_back();
    demultiplex_harq_ack_and_csi_part1_entry& entry = demultiplex_harq_ack_and_csi_part1_entries.back();
    entry.harq_ack                                  = harq_ack;
    entry.csi_part1                                 = csi_part1;
    entry.input                                     = input;
    entry.config                                    = config;
  }

  void demultiplex_sch_and_csi_part2(span<log_likelihood_ratio>       sch_data,
                                     span<log_likelihood_ratio>       csi_part2,
                                     span<const log_likelihood_ratio> input,
                                     unsigned int                     nof_enc_harq_ack_bits,
                                     unsigned int                     nof_csi_part1_bits,
                                     const configuration&             config) override
  {
    demultiplex_sch_and_csi_part2_entries.emplace_back();
    demultiplex_sch_and_csi_part2_entry& entry = demultiplex_sch_and_csi_part2_entries.back();
    entry.sch_data                             = sch_data;
    entry.csi_part2                            = csi_part2;
    entry.input                                = input;
    entry.nof_enc_harq_ack_bits                = nof_enc_harq_ack_bits;
    entry.nof_csi_part1_bits                   = nof_csi_part1_bits;
    entry.config                               = config;
  }

  void demultiplex(span<log_likelihood_ratio>       sch_data,
                   span<log_likelihood_ratio>       harq_ack,
                   span<log_likelihood_ratio>       csi_part1,
                   span<log_likelihood_ratio>       csi_part2,
                   span<const log_likelihood_ratio> input,
                   const configuration&             config) override
  {
    demultiplex_entries.emplace_back();
    demultiplex_entry& entry = demultiplex_entries.back();
    entry.sch_data           = sch_data;
    entry.harq_ack           = harq_ack;
    entry.csi_part1          = csi_part1;
    entry.csi_part2          = csi_part2;
    entry.input              = input;
    entry.config             = config;
  }

  ulsch_placeholder_list get_placeholders(const message_information& uci_message_info,
                                          const configuration&       config) override
  {
    placeholder_entries.emplace_back();
    placeholders_entry& entry = placeholder_entries.back();
    entry.msg_info            = uci_message_info;
    entry.config              = config;

    // Generate a random list of trivial placeholders.
    for (unsigned count = 0, nof_placeholders = placeholder_dist(rgen); count != nof_placeholders; ++count) {
      entry.list.push_back(placeholder_dist(rgen));
    }

    return entry.list;
  }

  const std::vector<demultiplex_harq_ack_and_csi_part1_entry>& get_demultiplex_harq_ack_and_csi_part1_entries() const
  {
    return demultiplex_harq_ack_and_csi_part1_entries;
  }
  const std::vector<demultiplex_sch_and_csi_part2_entry>& get_demultiplex_sch_and_csi_part2_entries() const
  {
    return demultiplex_sch_and_csi_part2_entries;
  }
  const std::vector<demultiplex_entry>&  get_demultiplex_entries() const { return demultiplex_entries; }
  const std::vector<placeholders_entry>& get_placeholders_entries() const { return placeholder_entries; }
  void                                   clear()
  {
    demultiplex_entries.clear();
    placeholder_entries.clear();
  }

private:
  std::mt19937                                          rgen;
  std::uniform_int_distribution<unsigned>               placeholder_dist;
  std::vector<demultiplex_harq_ack_and_csi_part1_entry> demultiplex_harq_ack_and_csi_part1_entries;
  std::vector<demultiplex_sch_and_csi_part2_entry>      demultiplex_sch_and_csi_part2_entries;
  std::vector<demultiplex_entry>                        demultiplex_entries;
  std::vector<placeholders_entry>                       placeholder_entries;
};

PHY_SPY_FACTORY_TEMPLATE(ulsch_demultiplex);

} // namespace srsran