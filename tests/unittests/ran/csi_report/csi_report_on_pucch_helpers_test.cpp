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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
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

using CsiReportUnpackingParams = std::tuple<pmi_codebook_type, csi_report_quantities, Repetitions>;

class CsiReportPucchFixture : public ::testing::TestWithParam<CsiReportUnpackingParams>
{
protected:
  csi_report_configuration configuration;
  csi_report_data          unpacked_data;
  csi_report_packed        packed_pucch_data;

  void SetUp() override
  {
    const pmi_codebook_type&     pmi_codebook = std::get<0>(GetParam());
    const csi_report_quantities& quantities   = std::get<1>(GetParam());

    unsigned nof_csi_rs_antenna_ports = get_nof_csi_rs_antenna_ports(pmi_codebook);

    configuration.nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen);
    configuration.pmi_codebook         = pmi_codebook;
    configuration.ri_restriction       = ~ri_restriction_type(nof_csi_rs_antenna_ports);
    configuration.quantities           = quantities;

    if (configuration.ri_restriction.count() > 2) {
      // Set a random RI restriction element to false.
      std::uniform_int_distribution<unsigned> ri_restriction_dist(1, nof_csi_rs_antenna_ports - 1);
      configuration.ri_restriction.set(ri_restriction_dist(rgen), false);
    }

    // Pack CRI if enabled.
    if (configuration.quantities < csi_report_quantities::other) {
      fill_cri(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack RI if enabled.
    if (configuration.quantities < csi_report_quantities::other) {
      fill_ri(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack LI if enabled.
    if (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      fill_li(packed_pucch_data, unpacked_data, configuration);
    }

    // Fill with padding.
    fill_padding(packed_pucch_data, unpacked_data, configuration);

    // Pack PMI if enabled.
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_pmi(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack Wideband CQI if enabled.
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_wideband_cqi(packed_pucch_data, unpacked_data, configuration);
    }
  }

private:
  static void fill_cri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_cri_bits = 0;

    switch (config.pmi_codebook) {
      case pmi_codebook_type::one:
      case pmi_codebook_type::two:
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        nof_cri_bits = log2_ceil(config.nof_csi_rs_resources);
        break;
      case pmi_codebook_type::other:
      default:
        // Ignore.
        break;
    }
    unsigned cri = rgen() & mask_lsb_ones<unsigned>(nof_cri_bits);
    unpacked.cri.emplace(cri);
    packed.push_back(cri, nof_cri_bits);
  }

  static void fill_ri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
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
  }

  static void fill_li(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_li_bits = 0;
    unsigned nof_layers  = unpacked.ri.value().to_uint();

    switch (config.pmi_codebook) {
      case pmi_codebook_type::one:
        nof_li_bits = 0;
        break;
      case pmi_codebook_type::two:
        nof_li_bits = log2_ceil(nof_layers);
        break;
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        nof_li_bits = std::min(2U, log2_ceil(nof_layers));
        break;
      case pmi_codebook_type::other:
      default:
        // Ignore.
        break;
    }

    unsigned li = (rgen() & mask_lsb_ones<unsigned>(nof_li_bits));
    unpacked.li.emplace(li);
    packed.push_back(li, nof_li_bits);
  }

  static void fill_padding(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    csi_report_data::ri_type ri = unpacked.ri.value();

    switch (config.pmi_codebook) {
      case pmi_codebook_type::two:
        if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) && (ri == 2)) {
          packed.push_back(1U, 1);
        }
        break;
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) && (ri > 2)) {
          packed.push_back(1U, 1);
        }
        if ((config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) && (ri == 1)) {
          packed.push_back(1U, 1);
        }
        break;
      case pmi_codebook_type::one:
      case pmi_codebook_type::other:
      default:
        // Ignore.
        break;
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

  static void
  fill_wideband_cqi(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned ri            = unpacked.ri.value().to_uint();
    unsigned wideband_cqi1 = rgen() & mask_lsb_ones<unsigned>(4);
    unsigned wideband_cqi2 = rgen() & mask_lsb_ones<unsigned>(4);

    unpacked.first_tb_wideband_cqi.emplace(wideband_cqi1);
    packed.push_back(wideband_cqi1, 4);

    if (ri > 4) {
      unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
      packed.push_back(wideband_cqi2, 4);
    }
  }

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

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> nof_csi_rs_resources_dist;
};

std::mt19937                            CsiReportPucchFixture::rgen;
std::uniform_int_distribution<unsigned> CsiReportPucchFixture::nof_csi_rs_resources_dist(1, 16);

} // namespace

TEST_P(CsiReportPucchFixture, CsiReportPucchUnpacking)
{
  // Get report size.
  units::bits csi_report_size = get_csi_report_pucch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size, units::bits(packed_pucch_data.size()));

  // Unpack.
  ASSERT_TRUE(validate_pucch_csi_payload(packed_pucch_data, configuration));
  csi_report_data unpacked = csi_report_unpack_pucch(packed_pucch_data, configuration);

  // Assert CRI.
  ASSERT_EQ(unpacked_data, unpacked);
}

INSTANTIATE_TEST_SUITE_P(CsiReportPucchHelpersTest,
                         CsiReportPucchFixture,
                         ::testing::Combine(::testing::Values(pmi_codebook_type::one,
                                                              pmi_codebook_type::two,
                                                              pmi_codebook_type::typeI_single_panel_4ports_mode1),
                                            ::testing::Values(csi_report_quantities::cri_ri_pmi_cqi,
                                                              csi_report_quantities::cri_ri_cqi,
                                                              csi_report_quantities::cri_ri_li_pmi_cqi),
                                            ::testing::Range(0U, 10U)));
