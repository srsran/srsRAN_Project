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

#include "srsran/phy/upper/channel_processors/ulsch_demultiplex.h"

namespace srsran {

class ulsch_demultiplex_impl : public ulsch_demultiplex
{
public:
  // See interface for documentation.
  void demultiplex_csi_part1(span<log_likelihood_ratio>       csi_part1,
                             span<const log_likelihood_ratio> input,
                             unsigned                         nof_enc_harq_ack_bits,
                             const configuration&             config) override;

  // See interface for documentation.
  void demultiplex_sch_harq_ack_and_csi_part2(span<log_likelihood_ratio>       sch_data,
                                              span<log_likelihood_ratio>       harq_ack,
                                              span<log_likelihood_ratio>       csi_part2,
                                              span<const log_likelihood_ratio> input,
                                              unsigned                         nof_csi_part1_bits,
                                              const configuration&             config) override;

  // See interface for documentation.
  void demultiplex(span<log_likelihood_ratio>       sch_data,
                   span<log_likelihood_ratio>       harq_ack,
                   span<log_likelihood_ratio>       csi_part1,
                   span<log_likelihood_ratio>       csi_part2,
                   span<const log_likelihood_ratio> input,
                   const configuration&             config) override;

  // See interface for documentation.
  ulsch_placeholder_list get_placeholders(const message_information& uci_message_info,
                                          const configuration&       config) override;
};

} // namespace srsran
