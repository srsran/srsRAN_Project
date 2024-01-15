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
#include "precoding_matrix_mapper_functions.h"
#include "precoding_matrix_repository_builder.h"
#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/ran/precoding/precoding_codebooks.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Maximum number of codebooks for 1-port, 2-port and 4-port configurations.
static constexpr std::array<unsigned, 4> max_num_codebooks = {{6, 11, 0, 261}};

/// Generates SSB codebooks and precoding matrices for the given number of ports.
static unsigned generate_ssb(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_ssb_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDCCH codebooks and precoding matrices for the given number of ports.
static unsigned generate_pdcch(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdcch_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates CSI-RS codebooks and precoding matrices for the given number of ports.
static unsigned generate_csi_rs(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_identity(nof_ports);
  unsigned                pm_index  = offset + get_csi_rs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDSCH omnidirectional codebook and precoding matrices for the given number of ports.
static unsigned
generate_pdsch_omnidirectional(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdsch_omnidirectional_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port PDSCH codebooks and precoding matrices.
static unsigned generate_pdsch_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_single_port();
  unsigned                pm_index  = offset + get_pdsch_one_port_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates the identity matrix.
static unsigned
generate_identity_matrix(unsigned offset, precoding_matrix_repository_builder& repo_builder, unsigned nof_layers)
{
  precoding_weight_matrix precoding = make_identity(nof_layers);
  repo_builder.add(0, precoding);

  return ++offset;
}

/// Generates one-port codebooks and precoding matrices.
static void generate_single_port_table(precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets,
                                       precoding_matrix_repository_builder&                   repo_builder)
{
  unsigned                  offset    = 0U;
  static constexpr unsigned nof_ports = 1U;

  offset = generate_identity_matrix(offset, repo_builder, nof_ports);
  mapper_offsets.ssb_codebook_offsets.push_back(offset);
  offset                           = generate_ssb(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_omni_offset = offset;
  offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_one_port(offset, repo_builder);
  mapper_offsets.pdcch_codebook_offsets.push_back(offset);
  offset = generate_pdcch(offset, nof_ports, repo_builder);
  mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
  generate_csi_rs(offset, nof_ports, repo_builder);
}

/// Generates two-port PDSCH codebooks and precoding matrices for one layer.
static unsigned generate_pdsch_2_ports_1_layer(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 4; i != e; ++i) {
    precoding_weight_matrix precoding = make_one_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }
  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding matrices for two layers.
static unsigned generate_pdsch_2_ports_2_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 2; i != e; ++i) {
    precoding_weight_matrix precoding = make_two_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }

  return ++offset;
}

/// Generates two-port codebooks and precoding matrices.
static void generate_2_ports_table(precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets,
                                   precoding_matrix_repository_builder&                   repo_builder)
{
  unsigned                  offset    = 0U;
  static constexpr unsigned nof_ports = 2U;

  offset = generate_identity_matrix(offset, repo_builder, nof_ports);
  mapper_offsets.ssb_codebook_offsets.push_back(offset);
  offset                           = generate_ssb(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_omni_offset = offset;
  offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_2_ports_1_layer(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_2_ports_2_layers(offset, repo_builder);
  mapper_offsets.pdcch_codebook_offsets.push_back(offset);
  offset = generate_pdcch(offset, nof_ports, repo_builder);
  mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
  generate_csi_rs(offset, nof_ports, repo_builder);
}

/// Generates four-port PDSCH codebooks and precoding matrices for one layer.
static unsigned generate_pdsch_4_ports_1_layer(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i_1_1 = 0, e_1_1 = 8; i_1_1 != e_1_1; ++i_1_1) {
    for (unsigned i_2 = 0, e_2 = 4; i_2 != e_2; ++i_2) {
      precoding_weight_matrix precoding = make_one_layer_four_ports_type1_sp_mode1(i_1_1, i_2);
      unsigned                pm_index  = base_offset + get_pdsch_four_port_precoding_matrix_index(i_1_1, 0, i_2);
      repo_builder.add(pm_index, precoding);
      ++offset = pm_index;
    }
  }

  return ++offset;
}

/// Generates four-port PDSCH codebooks and precoding matrices for two layers.
static unsigned generate_pdsch_4_ports_2_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i_1_1 = 0, e_1_1 = 8; i_1_1 != e_1_1; ++i_1_1) {
    for (unsigned i_1_3 = 0, e_1_3 = 2; i_1_3 != e_1_3; ++i_1_3) {
      for (unsigned i_2 = 0, e_2 = 2; i_2 != e_2; ++i_2) {
        precoding_weight_matrix precoding = make_two_layer_four_ports_type1_sp_mode1(i_1_1, i_1_3, i_2);
        unsigned                pm_index  = base_offset + get_pdsch_four_port_precoding_matrix_index(i_1_1, i_1_3, i_2);
        repo_builder.add(pm_index, precoding);
        offset = pm_index;
      }
    }
  }

  return ++offset;
}

/// Generates four-port PDSCH codebooks and precoding matrices for three layers.
static unsigned generate_pdsch_4_ports_3_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i_1_1 = 0, e_1_1 = 8; i_1_1 != e_1_1; ++i_1_1) {
    for (unsigned i_2 = 0, e_2 = 2; i_2 != e_2; ++i_2) {
      precoding_weight_matrix precoding = make_three_layer_four_ports_type1_sp(i_1_1, i_2);
      unsigned                pm_index  = base_offset + get_pdsch_four_port_precoding_matrix_index(i_1_1, 0, i_2);
      repo_builder.add(pm_index, precoding);
      ++offset = pm_index;
    }
  }

  return ++offset;
}

/// Generates four-port PDSCH codebooks and precoding matrices for four layers.
static unsigned generate_pdsch_4_ports_4_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i_1_1 = 0, e_1_1 = 8; i_1_1 != e_1_1; ++i_1_1) {
    for (unsigned i_2 = 0, e_2 = 2; i_2 != e_2; ++i_2) {
      precoding_weight_matrix precoding = make_four_layer_four_ports_type1_sp(i_1_1, i_2);
      unsigned                pm_index  = base_offset + get_pdsch_four_port_precoding_matrix_index(i_1_1, 0, i_2);
      repo_builder.add(pm_index, precoding);
      offset = pm_index;
    }
  }

  return ++offset;
}

/// Generates four-port codebooks and precoding matrices.
static void generate_4_ports_table(precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets,
                                   precoding_matrix_repository_builder&                   repo_builder)
{
  unsigned                  offset    = 0U;
  static constexpr unsigned nof_ports = 4U;

  offset = generate_identity_matrix(offset, repo_builder, nof_ports);
  mapper_offsets.ssb_codebook_offsets.push_back(offset);
  offset                           = generate_ssb(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_omni_offset = offset;
  offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_4_ports_1_layer(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_4_ports_2_layers(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_4_ports_3_layers(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_4_ports_4_layers(offset, repo_builder);
  mapper_offsets.pdcch_codebook_offsets.push_back(offset);
  offset = generate_pdcch(offset, nof_ports, repo_builder);
  mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
  generate_csi_rs(offset, nof_ports, repo_builder);
}

std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>>
srsran::fapi_adaptor::generate_precoding_matrix_tables(unsigned nof_antenna_ports)
{
  srsran_assert(nof_antenna_ports > 0, "Invalid number of antenna ports={}", nof_antenna_ports);
  srsran_assert(nof_antenna_ports != 3, "Unsupported number of antenna ports={}", nof_antenna_ports);

  unsigned                                              nof_ports_index = nof_antenna_ports - 1U;
  precoding_matrix_mapper_codebook_offset_configuration mapper_offsets;
  precoding_matrix_repository_builder                   repo_builder(max_num_codebooks[nof_ports_index]);

  if (nof_antenna_ports == 1U) {
    generate_single_port_table(mapper_offsets, repo_builder);

    return {std::make_unique<precoding_matrix_mapper>(nof_antenna_ports, mapper_offsets), repo_builder.build()};
  }

  if (nof_antenna_ports == 2U) {
    generate_2_ports_table(mapper_offsets, repo_builder);

    return {std::make_unique<precoding_matrix_mapper>(nof_antenna_ports, mapper_offsets), repo_builder.build()};
  }

  if (nof_antenna_ports == 4U) {
    generate_4_ports_table(mapper_offsets, repo_builder);

    return {std::make_unique<precoding_matrix_mapper>(nof_antenna_ports, mapper_offsets), repo_builder.build()};
  }

  srsran_assert(false, "Unsupported number of ports", nof_antenna_ports);
  return {};
}
