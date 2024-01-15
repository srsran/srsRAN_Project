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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "srsran/ran/uci/uci_part2_size_calculator.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

auto to_tuple(const csi_report_data& data)
{
  return std::tie(data.cri, data.ri, data.li, data.pmi, data.first_tb_wideband_cqi, data.second_tb_wideband_cqi);
}

bool operator==(const csi_report_pmi& left, const csi_report_pmi& right)
{
  if (variant_holds_alternative<csi_report_pmi::two_antenna_port>(left.type) &&
      variant_holds_alternative<csi_report_pmi::two_antenna_port>(right.type)) {
    csi_report_pmi::two_antenna_port left2  = variant_get<csi_report_pmi::two_antenna_port>(left.type);
    csi_report_pmi::two_antenna_port right2 = variant_get<csi_report_pmi::two_antenna_port>(right.type);
    return left2.pmi == right2.pmi;
  }
  if (variant_holds_alternative<csi_report_pmi::typeI_single_panel_4ports_mode1>(left.type) &&
      variant_holds_alternative<csi_report_pmi::typeI_single_panel_4ports_mode1>(right.type)) {
    csi_report_pmi::typeI_single_panel_4ports_mode1 left2 =
        variant_get<csi_report_pmi::typeI_single_panel_4ports_mode1>(left.type);
    csi_report_pmi::typeI_single_panel_4ports_mode1 right2 =
        variant_get<csi_report_pmi::typeI_single_panel_4ports_mode1>(right.type);
    return (left2.i_1_1 == right2.i_1_1) && (left2.i_1_3 == right2.i_1_3) && (left2.i_2 == right2.i_2);
  }

  return false;
}

bool operator==(const csi_report_data& left, const csi_report_data& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const uci_part2_size_description::parameter left, const uci_part2_size_description::parameter right)
{
  return (left.offset == right.offset) && (left.width == right.width);
}

bool operator==(const uci_part2_size_description::entry left, const uci_part2_size_description::entry right)
{
  if (!std::equal(left.parameters.begin(), left.parameters.end(), right.parameters.begin(), right.parameters.end())) {
    return false;
  }

  return std::equal(left.map.begin(), left.map.end(), right.map.begin(), right.map.end());
}

bool operator==(const uci_part2_size_description& left, const uci_part2_size_description& right)
{
  return std::equal(left.entries.begin(), left.entries.end(), right.entries.begin(), right.entries.end());
}

bool operator!=(const uci_part2_size_description& left, const uci_part2_size_description& right)
{
  return !(left == right);
}

bool operator==(const csi_report_pusch_size& left, const csi_report_pusch_size& right)
{
  if (left.part1_size != right.part1_size) {
    return false;
  }

  if (left.part2_correspondence != right.part2_correspondence) {
    return false;
  }

  if (left.part2_min_size != right.part2_min_size) {
    return false;
  }

  if (left.part2_max_size != right.part2_max_size) {
    return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, const csi_report_pusch_size& data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, csi_report_data data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, units::bits data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, pmi_codebook_type codebook)
{
  fmt::print(os, "{}", to_string(codebook));
  return os;
}

std::ostream& operator<<(std::ostream& os, csi_report_quantities quantities)
{
  fmt::print(os, "{}", to_string(quantities));
  return os;
}

} // namespace srsran

namespace {

using Repetitions = unsigned;

using csi_report_size_params = std::tuple<pmi_codebook_type, csi_report_quantities, Repetitions>;

class CsiReportPuschFixture : public ::testing::TestWithParam<csi_report_size_params>
{
protected:
  csi_report_configuration configuration            = {};
  csi_report_pusch_size    expected_csi_report_size = {};
  csi_report_data          expected_unpacked_data;
  csi_report_packed        csi1_packed;
  csi_report_packed        csi2_packed;

  void SetUp() override
  {
    const pmi_codebook_type&     pmi_codebook = std::get<0>(GetParam());
    const csi_report_quantities& quantities   = std::get<1>(GetParam());

    unsigned nof_csi_rs_antenna_ports = get_nof_csi_rs_antenna_ports(pmi_codebook);

    // Prepare CSI report configuration.
    configuration.nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen);
    configuration.pmi_codebook         = pmi_codebook;
    configuration.ri_restriction       = ~ri_restriction_type(nof_csi_rs_antenna_ports);
    configuration.quantities           = quantities;

    if (configuration.ri_restriction.count() > 2) {
      // Set a random RI restriction element to false.
      std::uniform_int_distribution<unsigned> ri_restriction_dist(1, nof_csi_rs_antenna_ports - 1);
      configuration.ri_restriction.set(ri_restriction_dist(rgen), false);
    }

    // Fill CRI and calculate CRI size if enabled.
    unsigned cri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      cri_size = fill_cri(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill RI and calculate RI size if enabled.
    unsigned ri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      ri_size = fill_ri(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill wideband CQI for the first TB and calculated its size if enabled.
    unsigned wideband_cqi_1st_tb_size = 0;
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      wideband_cqi_1st_tb_size = fill_wideband_cqi_1st_tb(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill Subband differential CQI for the first TB if enabled.
    // ... Not supported.

    // Calculate CSI Part 1 size as described in TS38.212 Table 6.3.2.1.2-3.
    expected_csi_report_size.part1_size = units::bits{cri_size + ri_size + wideband_cqi_1st_tb_size};

    // Skip CSI Part 2 if it is not present.
    if ((nof_csi_rs_antenna_ports == 1) || ((configuration.quantities != csi_report_quantities::cri_ri_li_pmi_cqi) &&
                                            (configuration.quantities != csi_report_quantities::cri_ri_pmi_cqi))) {
      return;
    }

    // Fill wideband CQI for the second TB if enabled and calculate its possible sizes.
    std::vector<unsigned> wideband_cqi_2nd_tb_size = {};
    if ((nof_csi_rs_antenna_ports > 4) &&
        ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
         (quantities == csi_report_quantities::cri_ri_li_pmi_cqi))) {
      fill_wideband_cqi_2nd_tb(csi2_packed, expected_unpacked_data, configuration);

      for (unsigned i_nof_layers = 1; i_nof_layers <= nof_csi_rs_antenna_ports; ++i_nof_layers) {
        wideband_cqi_2nd_tb_size.emplace_back(get_second_tb_wideband_cqi_size(i_nof_layers));
      }
    }

    // Fill LI and calculate its possible sizes if enabled.
    std::vector<unsigned> li_size = {};
    if (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      fill_li(csi2_packed, expected_unpacked_data, configuration);

      for (unsigned i_nof_layers = 1; i_nof_layers <= nof_csi_rs_antenna_ports; ++i_nof_layers) {
        li_size.emplace_back(get_li_size(configuration, i_nof_layers));
      }
    }

    // Fill PMI and calculate the possible PMI sizes if enabled.
    std::vector<unsigned> pmi_size = {};
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_pmi(csi2_packed, expected_unpacked_data, configuration);

      for (unsigned i_nof_layers = 1; i_nof_layers <= nof_csi_rs_antenna_ports; ++i_nof_layers) {
        pmi_size.emplace_back(get_pmi_size(configuration, i_nof_layers));
      }
    }

    // Select CSI report entry.
    uci_part2_size_description::entry& entry = expected_csi_report_size.part2_correspondence.entries.emplace_back();

    // Select CSI Part 1 parameter and setup for matching RI.
    uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
    parameter.offset                                 = cri_size;
    parameter.width                                  = ri_size;

    // Create table.
    for (unsigned rank_idx = 0; rank_idx != nof_csi_rs_antenna_ports; ++rank_idx) {
      // Skip the CSI Part 2 sizes that correspond with forbidden RI values.
      if (!configuration.ri_restriction.test(rank_idx)) {
        continue;
      }

      unsigned csi_part2_size = 0;

      // Add second TB wideband CQI if available.
      if (!wideband_cqi_2nd_tb_size.empty()) {
        csi_part2_size += wideband_cqi_2nd_tb_size[rank_idx];
      }

      // Add LI if available.
      if (!li_size.empty()) {
        csi_part2_size += li_size[rank_idx];
      }

      // Add PMI if available.
      if (!pmi_size.empty()) {
        csi_part2_size += pmi_size[rank_idx];
      }

      entry.map.emplace_back(csi_part2_size);
    }

    expected_csi_report_size.part2_min_size = units::bits(*std::min_element(entry.map.begin(), entry.map.end()));
    expected_csi_report_size.part2_max_size = units::bits(*std::max_element(entry.map.begin(), entry.map.end()));
  }

private:
  static unsigned get_cri_size(const csi_report_configuration& config)
  {
    switch (config.pmi_codebook) {
      case pmi_codebook_type::one:
      case pmi_codebook_type::two:
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        return log2_ceil(config.nof_csi_rs_resources);
      case pmi_codebook_type::other:
      default:
        return 0;
    }
  }

  static unsigned get_li_size(const csi_report_configuration& config, unsigned nof_layers)
  {
    switch (config.pmi_codebook) {
      case pmi_codebook_type::one:
        return 0;
      case pmi_codebook_type::two:
        return log2_ceil(nof_layers);
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        return std::min(2U, log2_ceil(nof_layers));
      case pmi_codebook_type::other:
      default:
        return 0;
    }
  }

  static unsigned get_pmi_size(const csi_report_configuration& config, unsigned ri)
  {
    switch (config.pmi_codebook) {
      case pmi_codebook_type::two:
        return (ri == 1) ? 2 : 1;
      case pmi_codebook_type::typeI_single_panel_4ports_mode1: {
        unsigned nof_i_1_1_bits = log2_ceil(8U);
        unsigned nof_i_1_3_bits = 0;
        if (ri == 2) {
          nof_i_1_3_bits = 1;
        }
        unsigned nof_i_2_bits = 1;
        if (ri == 1) {
          nof_i_2_bits = 2;
        }

        return nof_i_1_1_bits + nof_i_1_3_bits + nof_i_2_bits;
      }
      case pmi_codebook_type::one:
      case pmi_codebook_type::other:
      default:
        return 0;
    }
  }

  static unsigned get_first_tb_wideband_cqi_size() { return 4; }

  static unsigned get_second_tb_wideband_cqi_size(unsigned ri) { return (ri > 4) ? 4 : 0; }

  static unsigned get_nof_csi_rs_antenna_ports(pmi_codebook_type pmi_codebook)
  {
    switch (pmi_codebook) {
      case pmi_codebook_type::one:
        return 1;
      case pmi_codebook_type::two:
        return 2;
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        return 4;
      default:
        return 0;
    }
  }

  static unsigned fill_cri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_cri_bits = get_cri_size(config);

    unsigned cri = rgen() & mask_lsb_ones<unsigned>(nof_cri_bits);
    unpacked.cri.emplace(cri);

    if (nof_cri_bits > 0) {
      packed.push_back(cri, nof_cri_bits);
    }

    return nof_cri_bits;
  }

  static unsigned fill_ri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_ri_bits = 0;
    unsigned nof_ri      = static_cast<unsigned>(config.ri_restriction.count());

    switch (config.pmi_codebook) {
      case pmi_codebook_type::two:
        nof_ri_bits = std::min(1U, log2_ceil(nof_ri));
        break;
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        nof_ri_bits = std::min(2U, log2_ceil(nof_ri));
        break;
      case pmi_codebook_type::one:
      case pmi_codebook_type::other:
      default:
        // Ignore.
        break;
    }

    // Create a uniform distribution to select a random rank index.
    std::uniform_int_distribution<unsigned> rank_idx_dist(0, nof_ri - 1);
    unsigned                                rank_idx = rank_idx_dist(rgen);

    // Select a random rank from the allowed options given by the RI restriction bitset (see TS38.214
    // Section 5.2.2.2.1.).
    unsigned rank = config.ri_restriction.get_bit_positions()[rank_idx] + 1;

    // The unpacked RI value indicates the chosen rank.
    unpacked.ri.emplace(rank);

    // The packed RI value indicates the chosen rank index (see TS38.212 Section 6.3.1.1.2.).
    packed.push_back(rank_idx, nof_ri_bits);

    return nof_ri_bits;
  }

  static void fill_li(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_layers  = unpacked.ri.value().to_uint();
    unsigned nof_li_bits = get_li_size(config, nof_layers);

    unsigned li = (rgen() & mask_lsb_ones<unsigned>(nof_li_bits));
    unpacked.li.emplace(li);

    if (nof_li_bits > 0) {
      packed.push_back(li, nof_li_bits);
    }
  }

  static void fill_pmi(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned ri = unpacked.ri.value().to_uint();

    switch (config.pmi_codebook) {
      case pmi_codebook_type::two: {
        unsigned nof_pmi_bits = (ri == 1) ? 2 : 1;

        csi_report_pmi::two_antenna_port type;
        type.pmi = rgen() & mask_lsb_ones<unsigned>(nof_pmi_bits);

        csi_report_pmi pmi;
        pmi.type.emplace<csi_report_pmi::two_antenna_port>(type);
        unpacked.pmi.emplace(pmi);

        packed.push_back(type.pmi, nof_pmi_bits);
        break;
      }
      case pmi_codebook_type::typeI_single_panel_4ports_mode1: {
        unsigned nof_i_1_1_bits = log2_ceil(8U);
        unsigned nof_i_1_3_bits = 0;
        if (ri == 2) {
          nof_i_1_3_bits = 1;
        }
        unsigned nof_i_2_bits = 1;
        if (ri == 1) {
          nof_i_2_bits = 2;
        }

        unsigned i_1_1 = rgen() & mask_lsb_ones<unsigned>(nof_i_1_1_bits);
        unsigned i_1_3 = rgen() & mask_lsb_ones<unsigned>(nof_i_1_3_bits);
        unsigned i_2   = rgen() & mask_lsb_ones<unsigned>(nof_i_2_bits);

        // Set PMI values.
        csi_report_pmi::typeI_single_panel_4ports_mode1 type;
        type.i_1_1 = i_1_1;
        if (ri > 1) {
          type.i_1_3.emplace(i_1_3);
        }
        type.i_2 = i_2;

        csi_report_pmi pmi;
        pmi.type.emplace<csi_report_pmi::typeI_single_panel_4ports_mode1>(type);
        unpacked.pmi.emplace(pmi);

        // Pack PMI values.
        packed.push_back(i_1_1, nof_i_1_1_bits);
        packed.push_back(i_1_3, nof_i_1_3_bits);
        packed.push_back(i_2, nof_i_2_bits);

        break;
      }

      case pmi_codebook_type::one:
      case pmi_codebook_type::other:
      default:
        // Ignore.
        break;
    }
  }

  static unsigned
  fill_wideband_cqi_1st_tb(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned wideband_cqi1_size = get_first_tb_wideband_cqi_size();
    unsigned wideband_cqi1      = rgen() & mask_lsb_ones<unsigned>(4);

    unpacked.first_tb_wideband_cqi.emplace(wideband_cqi1);
    packed.push_back(wideband_cqi1, wideband_cqi1_size);

    return wideband_cqi1_size;
  }

  static void
  fill_wideband_cqi_2nd_tb(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned ri                 = unpacked.ri.value().to_uint();
    unsigned wideband_cqi2_size = get_second_tb_wideband_cqi_size(ri);

    if (wideband_cqi2_size > 0) {
      unsigned wideband_cqi2 = rgen() & mask_lsb_ones<unsigned>(4);
      unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
      packed.push_back(wideband_cqi2, 4);
    }
  }

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> nof_csi_rs_resources_dist;
};

std::mt19937                            CsiReportPuschFixture::rgen;
std::uniform_int_distribution<unsigned> CsiReportPuschFixture::nof_csi_rs_resources_dist(1, 16);

} // namespace

TEST_P(CsiReportPuschFixture, csiReportPuschSize)
{
  // Get report size.
  csi_report_pusch_size csi_report_size = get_csi_report_pusch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size, expected_csi_report_size);
}

TEST_P(CsiReportPuschFixture, csiReportPuschUnpacking)
{
  // Unpack.
  ASSERT_TRUE(validate_pusch_csi_payload(csi1_packed, csi2_packed, configuration));
  csi_report_data unpacked = (csi2_packed.empty()) ? csi_report_unpack_pusch(csi1_packed, configuration)
                                                   : csi_report_unpack_pusch(csi1_packed, csi2_packed, configuration);

  // Assert unpacked CSI information.
  ASSERT_EQ(expected_unpacked_data, unpacked);
}

INSTANTIATE_TEST_SUITE_P(CsiReportPuschHelpersTest,
                         CsiReportPuschFixture,
                         ::testing::Combine(::testing::Values(pmi_codebook_type::one,
                                                              pmi_codebook_type::two,
                                                              pmi_codebook_type::typeI_single_panel_4ports_mode1),
                                            ::testing::Values(csi_report_quantities::cri_ri_pmi_cqi,
                                                              csi_report_quantities::cri_ri_cqi,
                                                              csi_report_quantities::cri_ri_li_pmi_cqi),
                                            ::testing::Range(0U, 10U)));
