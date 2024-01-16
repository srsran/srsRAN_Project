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

#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_puxch_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;

static constexpr unsigned NUM_CSI_RESOURCES = 1;

namespace {

class uci_part2_correspondence_generator_test
  : public ::testing::TestWithParam<std::tuple<pmi_codebook_type, csi_report_quantities>>
{
protected:
  unsigned              nof_csi_rs_resources = NUM_CSI_RESOURCES;
  pmi_codebook_type     codebook             = std::get<0>(GetParam());
  csi_report_quantities quantities           = std::get<1>(GetParam());
};

} // namespace

TEST_P(uci_part2_correspondence_generator_test, correct_generation_test)
{
  std::unique_ptr<uci_part2_correspondence_mapper>     mapper;
  std::unique_ptr<uci_part2_correspondence_repository> repository;
  std::tie(mapper, repository) = generate_uci_part2_correspondence(NUM_CSI_RESOURCES);

  unsigned            nof_csi_rs_ports = csi_report_get_nof_csi_rs_antenna_ports(codebook);
  ri_restriction_type ri_restiction(nof_csi_rs_ports);
  ri_restiction.fill(0, nof_csi_rs_ports);

  csi_report_configuration report_cfg;
  report_cfg.nof_csi_rs_resources = nof_csi_rs_resources;
  report_cfg.pmi_codebook         = codebook;
  report_cfg.ri_restriction       = ri_restiction;
  report_cfg.quantities           = quantities;

  uci_part2_size_description part2_correspondence = get_csi_report_pusch_size(report_cfg).part2_correspondence;

  // Check that the number of part2 entries match.
  span<const uci_part2_correspondence_information> info = mapper->map(report_cfg);
  ASSERT_EQ(info.size(), part2_correspondence.entries.size());

  for (unsigned i = 0, e = info.size(); i != e; ++i) {
    // Check that we have the same number of part1 parameters.
    ASSERT_EQ(info[i].part1_param_sizes.size(), part2_correspondence.entries[i].parameters.size());
    // Check the fields of each part1 parameter.
    for (unsigned j = 0, je = info[i].part1_param_sizes.size(); j != je; ++j) {
      ASSERT_EQ(info[i].part1_param_offsets[j], part2_correspondence.entries[i].parameters[j].offset);
      ASSERT_EQ(info[i].part1_param_sizes[j], part2_correspondence.entries[i].parameters[j].width);
    }

    span<const uint16_t> indices = repository->get_uci_part2_correspondence(info[i].part2_map_index);
    // Check the map field matches to the contents of the repository.
    for (unsigned j = 0, je = indices.size(); j != je; ++j) {
      ASSERT_EQ(indices[j], part2_correspondence.entries[i].map[j]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(uci_part2,
                         uci_part2_correspondence_generator_test,
                         testing::Combine(testing::Values(pmi_codebook_type::one,
                                                          pmi_codebook_type::two,
                                                          pmi_codebook_type::typeI_single_panel_4ports_mode1),
                                          testing::Values(csi_report_quantities::cri_ri_cqi,
                                                          csi_report_quantities::cri_ri_pmi_cqi,
                                                          csi_report_quantities::cri_ri_li_pmi_cqi)));
