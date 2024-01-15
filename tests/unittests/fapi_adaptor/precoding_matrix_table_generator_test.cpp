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

#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const precoding_weight_matrix& matrix)
{
  fmt::print(os, "{}", matrix);
  return os;
}

} // namespace srsran

TEST(precoding_matrix_table_generator, one_port)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(1);

  mac_pdsch_precoding_info info;
  info.report.reset();

  unsigned index = mapper->map(info, 1);

  precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

  precoding_weight_matrix expected_matrix = make_single_port();

  ASSERT_EQ(matrix, expected_matrix);
}

TEST(precoding_matrix_table_generator, two_port_one_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(2);

  // Iterate over all possible PMI.
  for (unsigned pmi = 0; pmi != 4; ++pmi) {
    mac_pdsch_precoding_info info;
    info.report.emplace(csi_report_pmi{csi_report_pmi::two_antenna_port{pmi}});

    unsigned index = mapper->map(info, 1);

    precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

    precoding_weight_matrix expected_matrix = make_one_layer_two_ports(pmi);

    ASSERT_EQ(matrix, expected_matrix);
  }
}

TEST(precoding_matrix_table_generator, two_port_two_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(2);

  // Iterate over all possible PMI.
  for (unsigned pmi = 0; pmi != 2; ++pmi) {
    mac_pdsch_precoding_info info;
    info.report.emplace(csi_report_pmi{csi_report_pmi::two_antenna_port{pmi}});

    unsigned index = mapper->map(info, 2);

    precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

    precoding_weight_matrix expected_matrix = make_two_layer_two_ports(pmi);

    ASSERT_EQ(matrix, expected_matrix);
  }
}

TEST(precoding_matrix_table_generator, four_port_typeI_single_panel_mode1_one_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(4);

  // Iterate over all possible PMI.
  for (unsigned i_1_1 = 0; i_1_1 != 8; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != 4; ++i_2) {
      mac_pdsch_precoding_info info;
      info.report.emplace(csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{i_1_1, nullopt, i_2}});

      unsigned index = mapper->map(info, 1);

      precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

      precoding_weight_matrix expected_matrix = make_one_layer_four_ports_type1_sp_mode1(i_1_1, i_2);

      ASSERT_EQ(matrix, expected_matrix);
    }
  }
}

TEST(precoding_matrix_table_generator, four_port_typeI_single_panel_mode1_two_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(4);

  // Iterate over all possible PMI.
  for (unsigned i_1_1 = 0; i_1_1 != 8; ++i_1_1) {
    for (unsigned i_1_3 = 0; i_1_3 != 2; ++i_1_3) {
      for (unsigned i_2 = 0; i_2 != 2; ++i_2) {
        mac_pdsch_precoding_info info;
        info.report.emplace(csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{i_1_1, {i_1_3}, i_2}});

        unsigned index = mapper->map(info, 2);

        precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

        precoding_weight_matrix expected_matrix = make_two_layer_four_ports_type1_sp_mode1(i_1_1, i_1_3, i_2);

        ASSERT_EQ(matrix, expected_matrix);
      }
    }
  }
}

TEST(precoding_matrix_table_generator, four_port_typeI_single_panel_three_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(4);

  // Iterate over all possible PMI.
  for (unsigned i_1_1 = 0; i_1_1 != 8; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != 2; ++i_2) {
      mac_pdsch_precoding_info info;
      info.report.emplace(csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{i_1_1, {}, i_2}});

      unsigned index = mapper->map(info, 3);

      precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

      precoding_weight_matrix expected_matrix = make_three_layer_four_ports_type1_sp(i_1_1, i_2);

      ASSERT_EQ(matrix, expected_matrix);
    }
  }
}

TEST(precoding_matrix_table_generator, four_port_typeI_single_panel_four_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(4);

  // Iterate over all possible PMI.
  for (unsigned i_1_1 = 0; i_1_1 != 8; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != 2; ++i_2) {
      mac_pdsch_precoding_info info;
      info.report.emplace(csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{i_1_1, {}, i_2}});

      unsigned index = mapper->map(info, 4);

      precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

      precoding_weight_matrix expected_matrix = make_four_layer_four_ports_type1_sp(i_1_1, i_2);

      ASSERT_EQ(matrix, expected_matrix);
    }
  }
}
