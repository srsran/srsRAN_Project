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

#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "precoding_matrix_mapper_functions.h"
#include "precoding_matrix_repository_builder.h"
#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/ran/precoding/precoding_codebooks.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Maximum number of codebooks for 1-port and 2-port configurations.
static constexpr std::array<unsigned, 2> max_num_codebooks = {{4, 9}};

/// Generates one-port SSB codebooks and precoding configurations.
static unsigned generate_ssb_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_single_port();
  unsigned                pm_index  = offset + get_ssb_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port PDSCH codebooks and precoding configurations.
static unsigned generate_pdsch_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_single_port();
  unsigned                pm_index  = offset + get_pdsch_one_port_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port PDCCH codebooks and precoding configurations.
static unsigned generate_pdcch_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_single_port();
  unsigned                pm_index  = offset + get_pdcch_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port CSI-RS codebooks and precoding configurations.
static unsigned generate_csi_rs_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_single_port();
  unsigned                pm_index  = offset + get_csi_rs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates the identity matrix.
static unsigned
generate_identity_matrix(unsigned offset, precoding_matrix_repository_builder& repo_builder, unsigned nof_layers)
{
  precoding_configuration precoding = make_wideband_identity(nof_layers);
  repo_builder.add(0, precoding);

  return ++offset;
}

/// Generates one-port codebooks and precoding configurations.
static void generate_single_port_table(precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets,
                                       precoding_matrix_repository_builder&                   repo_builder)
{
  unsigned offset = 0U;

  offset = generate_identity_matrix(offset, repo_builder, 1);
  mapper_offsets.ssb_codebook_offsets.push_back(offset);
  offset = generate_ssb_one_port(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_one_port(offset, repo_builder);
  mapper_offsets.pdcch_codebook_offsets.push_back(offset);
  offset = generate_pdcch_one_port(offset, repo_builder);
  mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
  generate_csi_rs_one_port(offset, repo_builder);
}

/// Generates two-port SSB codebooks and precoding configurations.
static unsigned generate_ssb_2_ports(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_wideband_one_layer_one_port(2, 0);
  unsigned                pm_index  = offset + get_ssb_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding configurations for one layer.
static unsigned generate_pdsch_2_ports_1_layer(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 4; i != e; ++i) {
    precoding_configuration precoding = make_wideband_one_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    ++offset;
  }
  return offset;
}

/// Generates two-port PDSCH codebooks and precoding configurations for two layers.
static unsigned generate_pdsch_2_ports_2_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 2; i != e; ++i) {
    precoding_configuration precoding = make_wideband_two_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    ++offset;
  }

  return offset;
}

/// Generates two-port PDCCH codebooks and precoding configurations.
static unsigned generate_pdcch_2_ports(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_wideband_one_layer_one_port(2, 0);
  unsigned                pm_index  = offset + get_pdcch_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates two-port CSI-RS codebooks and precoding configurations.
static unsigned generate_csi_rs_2_ports(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_configuration precoding = make_wideband_identity(2);
  unsigned                pm_index  = offset + get_csi_rs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates two-port codebooks and precoding configurations.
static void generate_2_ports_table(precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets,
                                   precoding_matrix_repository_builder&                   repo_builder)
{
  unsigned offset = 0U;

  offset = generate_identity_matrix(offset, repo_builder, 2);
  mapper_offsets.ssb_codebook_offsets.push_back(offset);
  offset = generate_ssb_2_ports(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_2_ports_1_layer(offset, repo_builder);
  mapper_offsets.pdsch_codebook_offsets.push_back(offset);
  offset = generate_pdsch_2_ports_2_layers(offset, repo_builder);
  mapper_offsets.pdcch_codebook_offsets.push_back(offset);
  offset = generate_pdcch_2_ports(offset, repo_builder);
  mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
  generate_csi_rs_2_ports(offset, repo_builder);
}

std::tuple<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>>
srsran::fapi_adaptor::generate_precoding_matrix_tables(unsigned nof_antenna_ports)
{
  srsran_assert(nof_antenna_ports > 0, "Invalid number of antenna ports={}", nof_antenna_ports);

  unsigned                                              nof_ports_index = nof_antenna_ports - 1U;
  precoding_matrix_mapper_codebook_offset_configuration mapper_offsets;
  precoding_matrix_repository_builder                   repo_builder(max_num_codebooks[nof_ports_index]);

  if (nof_antenna_ports == 1U) {
    generate_single_port_table(mapper_offsets, repo_builder);

    return {std::make_unique<precoding_matrix_mapper>(mapper_offsets), repo_builder.build()};
  }

  if (nof_antenna_ports == 2U) {
    generate_2_ports_table(mapper_offsets, repo_builder);

    return {std::make_unique<precoding_matrix_mapper>(mapper_offsets), repo_builder.build()};
  }

  srsran_assert(false, "Unsupported number of ports", nof_antenna_ports);
  return {};
}
