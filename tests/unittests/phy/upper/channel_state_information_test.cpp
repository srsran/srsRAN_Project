/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_state_information_formatters.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace {

using test_params = std::tuple<channel_state_information::sinr_type, bool, bool, bool, bool, bool, bool, bool>;

class ChannelStateInformationFixture : public ::testing::TestWithParam<test_params>
{
protected:
  void SetUp() override
  {
    Test::SetUp();
    const test_params& params = GetParam();
    sinr_type                 = std::get<0>(params);
    enable_ce_sinr            = std::get<1>(params);
    enable_eq_sinr            = std::get<2>(params);
    enable_ev_sinr            = std::get<3>(params);
    enable_rsrp               = std::get<4>(params);
    enable_epre               = std::get<5>(params);
    enable_cfo                = std::get<6>(params);
    enable_ta                 = std::get<7>(params);
  }

  channel_state_information generate_random_csi()
  {
    channel_state_information csi(sinr_type);
    if (enable_ce_sinr) {
      csi.set_sinr_dB(channel_state_information::sinr_type::channel_estimator, real_dist(rgen));
    }
    if (enable_eq_sinr) {
      csi.set_sinr_dB(channel_state_information::sinr_type::post_equalization, real_dist(rgen));
    }
    if (enable_ev_sinr) {
      csi.set_sinr_dB(channel_state_information::sinr_type::evm, real_dist(rgen));
      csi.set_total_evm(real_dist(rgen));
    }
    if (enable_rsrp) {
      csi.set_rsrp_dB(real_dist(rgen));
    }
    if (enable_epre) {
      csi.set_epre(real_dist(rgen));
    }
    if (enable_cfo) {
      csi.set_cfo(real_dist(rgen));
    }
    if (enable_ta) {
      csi.set_time_alignment(phy_time_unit::from_seconds(real_dist(rgen)));
    }
    return csi;
  }

private:
  static std::mt19937                          rgen;
  static std::uniform_real_distribution<float> real_dist;
  channel_state_information::sinr_type         sinr_type;
  bool                                         enable_ce_sinr;
  bool                                         enable_eq_sinr;
  bool                                         enable_ev_sinr;
  bool                                         enable_rsrp;
  bool                                         enable_epre;
  bool                                         enable_cfo;
  bool                                         enable_ta;
};

std::mt19937                          ChannelStateInformationFixture::rgen;
std::uniform_real_distribution<float> ChannelStateInformationFixture::real_dist(-100, +100);

TEST_P(ChannelStateInformationFixture, FormatterDefault)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", generate_random_csi());
}

TEST_P(ChannelStateInformationFixture, FormatterShort)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "{:s}", generate_random_csi());
}

INSTANTIATE_TEST_SUITE_P(PucchProcessorFormat1,
                         ChannelStateInformationFixture,
                         ::testing::Combine(::testing::Values(channel_state_information::sinr_type::channel_estimator,
                                                              channel_state_information::sinr_type::post_equalization,
                                                              channel_state_information::sinr_type::evm),
                                            ::testing::Bool(),
                                            ::testing::Bool(),
                                            ::testing::Bool(),
                                            ::testing::Bool(),
                                            ::testing::Bool(),
                                            ::testing::Bool(),
                                            ::testing::Bool()));

} // namespace
