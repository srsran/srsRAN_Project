/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

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

class csi_report_size_pusch_fixture : public ::testing::TestWithParam<csi_report_size_params>
{
protected:
  csi_report_configuration configuration            = {};
  csi_report_pusch_size    expected_csi_report_size = {};

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

    // Calculate CRI size if enabled.
    unsigned cri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      cri_size = get_cri_size(configuration);
    }

    // Calculate RI size if enabled.
    unsigned ri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      ri_size = get_ri_size(configuration);
    }

    // Calculate LI size if enabled.
    std::vector<unsigned> li_size = {};
    if (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      for (unsigned i_nof_layers = 0; i_nof_layers != nof_csi_rs_antenna_ports; ++i_nof_layers) {
        li_size.emplace_back(get_li_size(configuration, i_nof_layers + 1));
      }
    }

    // Calculate PMI size if enabled.
    std::vector<unsigned> pmi_size = {};
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      for (unsigned i_nof_layers = 0; i_nof_layers != nof_csi_rs_antenna_ports; ++i_nof_layers) {
        pmi_size.emplace_back(get_pmi_size(configuration, i_nof_layers + 1));
      }
    }

    // Calculate first TB wideband CQI size if enabled.
    unsigned first_tb_wideband_cqi_size = 0;
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      first_tb_wideband_cqi_size = get_first_tb_wideband_cqi_size();
    }

    // Calculate second TB wideband CQI size if enabled.
    std::vector<unsigned> second_tb_wideband_cqi_size = {};
    if ((nof_csi_rs_antenna_ports > 4) &&
        ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
         (quantities == csi_report_quantities::cri_ri_li_pmi_cqi))) {
      for (unsigned i_nof_layers = 0; i_nof_layers != nof_csi_rs_antenna_ports; ++i_nof_layers) {
        second_tb_wideband_cqi_size.emplace_back(get_second_tb_wideband_cqi_size(i_nof_layers + 1));
      }
    }

    // Calculate CSI Part 1 size as described in TS38.212 Table 6.3.2.1.2-3.
    expected_csi_report_size.part1_size = units::bits{cri_size + ri_size + first_tb_wideband_cqi_size};

    // Skip CSI Part 2 if it is not present.
    if ((nof_csi_rs_antenna_ports == 1) ||
        (second_tb_wideband_cqi_size.empty() && li_size.empty() && pmi_size.empty())) {
      return;
    }

    // Select CSI report entry.
    uci_part2_size_description::entry& entry = expected_csi_report_size.part2_correspondence.entries.emplace_back();

    // Select CSI Part 1 parameter and setup for matching RI.
    uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
    parameter.offset                                 = cri_size;
    parameter.width                                  = ri_size;

    // Create table.
    for (unsigned i_nof_layers = 0; i_nof_layers != nof_csi_rs_antenna_ports; ++i_nof_layers) {
      unsigned csi_part2_size = 0;

      // Add second TB wideband CQI if available.
      if (!second_tb_wideband_cqi_size.empty()) {
        csi_part2_size += second_tb_wideband_cqi_size[i_nof_layers];
      }

      // Add LI if available.
      if (!li_size.empty()) {
        csi_part2_size += li_size[i_nof_layers];
      }

      // Add PMI if available.
      if (!pmi_size.empty()) {
        csi_part2_size += pmi_size[i_nof_layers];
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

  static unsigned get_ri_size(const csi_report_configuration& config)
  {
    unsigned nof_ri = static_cast<unsigned>(config.ri_restriction.count());

    switch (config.pmi_codebook) {
      case pmi_codebook_type::two:
        return std::min(1U, log2_ceil(nof_ri));
      case pmi_codebook_type::typeI_single_panel_4ports_mode1:
        return std::min(2U, log2_ceil(nof_ri));
      case pmi_codebook_type::one:
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

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> nof_csi_rs_resources_dist;
};

std::mt19937                            csi_report_size_pusch_fixture::rgen;
std::uniform_int_distribution<unsigned> csi_report_size_pusch_fixture::nof_csi_rs_resources_dist(1, 16);

} // namespace

TEST_P(csi_report_size_pusch_fixture, csi_report_size_pusch)
{
  // Get report size.
  csi_report_pusch_size csi_report_size = get_csi_report_pusch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size, expected_csi_report_size);
}

INSTANTIATE_TEST_SUITE_P(CsiReportSize,
                         csi_report_size_pusch_fixture,
                         ::testing::Combine(::testing::Values(pmi_codebook_type::one,
                                                              pmi_codebook_type::two,
                                                              pmi_codebook_type::typeI_single_panel_4ports_mode1),
                                            ::testing::Values(csi_report_quantities::cri_ri_pmi_cqi,
                                                              csi_report_quantities::cri_ri_cqi,
                                                              csi_report_quantities::cri_ri_li_pmi_cqi),
                                            ::testing::Range(0U, 10U)));
