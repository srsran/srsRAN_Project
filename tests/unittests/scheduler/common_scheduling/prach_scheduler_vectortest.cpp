/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/common_scheduling/prach_scheduler.h"
#include "prach_scheduler_test_data.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/test_utils.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

namespace srsran {

bool operator==(const prach_occasion_info& left, const prach_occasion_info& right)
{
  return (left.pci == right.pci) && (left.nof_prach_occasions == right.nof_prach_occasions) &&
         (left.format == right.format) && (left.index_fd_ra == right.index_fd_ra) &&
         (left.start_symbol == right.start_symbol) && (left.nof_cs == right.nof_cs) &&
         (left.nof_fd_ra == right.nof_fd_ra) && (left.start_preamble_index == right.start_preamble_index) &&
         (left.nof_preamble_indexes == right.nof_preamble_indexes);
}

bool operator==(span<const prach_occasion_info> left, span<const prach_occasion_info> right)
{
  return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

std::ostream& operator<<(std::ostream& os, const prach_occasion_info& occasion)
{
  fmt::print(
      os,
      "pci={} nof_prach_occasions={} format={} index_fd_ra={} start_symbol={} nof_cs={} nof_fd_ra={} preambles={}",
      occasion.pci,
      occasion.nof_prach_occasions,
      to_string(occasion.format),
      occasion.index_fd_ra,
      occasion.start_symbol,
      occasion.nof_cs,
      occasion.nof_fd_ra,
      interval<unsigned>::start_and_len(occasion.start_preamble_index, occasion.nof_preamble_indexes));
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const prach_occasion_info> occasions)
{
  if (occasions.empty()) {
    os << "<empty>";
    return os;
  }

  for (const auto& occasion : occasions) {
    os << std::endl << occasion;
  }

  return os;
}

} // namespace srsran

using namespace srsran;

namespace prach_vectortest {

using prach_test_params = test_case_t;

static sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(subcarrier_spacing scs_common,
                                             nr_band            band,
                                             unsigned           prach_config_index,
                                             pci_t              pci,
                                             uint16_t           zcz,
                                             uint8_t            nof_fd_ra)
{
  cell_config_builder_params params = {
      .pci = pci, .scs_common = scs_common, .channel_bw_mhz = srsran::bs_channel_bandwidth::MHz20, .band = band};

  // For TDD, set DL ARFCN according to the band.
  if (not band_helper::is_paired_spectrum(band)) {
    if (band_helper::get_freq_range(band) == frequency_range::FR1) {
      params.dl_f_ref_arfcn = 520002;
    } else {
      params.dl_f_ref_arfcn = 2074171;
    }
  }
  if (band_helper::get_freq_range(band) == frequency_range::FR2) {
    params.channel_bw_mhz = bs_channel_bandwidth::MHz100;
  }
  sched_cell_configuration_request_message sched_req =
      sched_config_helper::make_default_sched_cell_configuration_request(params);

  sched_req.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index = prach_config_index;
  sched_req.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config = zcz;
  sched_req.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm                     = nof_fd_ra;

  // NOTE: For this test we modify the TDD pattern so that we can test several PRACH configuration indices.
  if (not band_helper::is_paired_spectrum(band)) {
    sched_req.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_slots = 0;
    sched_req.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_slots = 10;
  }

  return sched_req;
}
} // namespace prach_vectortest

using namespace prach_vectortest;

class prach_tester_vector : public ::testing::TestWithParam<prach_test_params>
{
protected:
  void SetUp() override
  {
    // Extract test parameters.
    const prach_test_params& params     = GetParam();
    duplex_mode              dplx_mode  = params.dplx_mode;
    subcarrier_spacing       scs_common = params.pusch_scs;
    nr_band                  band       = nr_band::n3;

    if (scs_common > subcarrier_spacing::kHz60) {
      band = nr_band::n261;
    } else if (scs_common == subcarrier_spacing::kHz30) {
      band = nr_band::n78;
    }

    frequency_range fr                 = band_helper::get_freq_range(band);
    unsigned        prach_config_index = params.prach_config_index;
    nof_slots_period                   = params.nof_slots_period;
    span<const crb_interval> crbs      = params.crbs;
    const ofdm_symbol_range& symbols   = params.symbols;
    uint8_t                  nof_fd_ra = static_cast<uint8_t>(crbs.size());

    // Set the actual bitset size to the number of slots per period for this testcase.
    active_slots = bounded_bitset<max_nof_slots_per_period>(nof_slots_period);

    // Convert the active slots set in the testcase into a bounded bitset.
    for (const unsigned sl : params.active_slots) {
      active_slots.set(sl % nof_slots_period);
    }

    // Generate random parameters.
    pci_t    pci = test_rgen::uniform_int<pci_t>();
    uint16_t zcz = test_rgen::uniform_int<uint16_t>();

    // Obtain PRACH configuration. Skip if the preamble format is invalid.
    prach_cfg = prach_configuration_get(fr, dplx_mode, prach_config_index);
    if (prach_cfg.format == prach_format_type::invalid) {
      GTEST_SKIP() << fmt::format("Ignored configuration index {}.", prach_config_index);
    }

    // Make cell configuration.
    cell_cfg = std::make_unique<cell_configuration>(
        sched_cfg,
        make_custom_sched_cell_configuration_request(scs_common, band, prach_config_index, pci, zcz, nof_fd_ra));

    // Make PRACH scheduler.
    prach_sch = std::make_unique<prach_scheduler>(*cell_cfg);

    // Make scheduler resource grid.
    res_grid = std::make_unique<cell_resource_allocator>(*cell_cfg);

    // Configure initial and end slot. The test shall run for the maximum PRACH System Frame Number (SFN) period which
    // is 16 frames.
    slot_begin = slot_point(to_numerology_value(scs_common), 0);
    slot_end   = slot_begin + nof_frames_period * slot_begin.nof_slots_per_frame();

    prach_preamble_information prach_info =
        is_long_preamble(prach_cfg.format)
            ? get_prach_preamble_long_info(prach_cfg.format)
            : get_prach_preamble_short_info(prach_cfg.format, to_ra_subcarrier_spacing(scs_common), false);

    // Calculate PRACH number of cyclic shifts.
    uint16_t nof_cs = prach_cyclic_shifts_get(
        prach_info.scs, cell_cfg->ul_cfg_common.init_ul_bwp.rach_cfg_common->restricted_set, zcz);

    // Select the number of time-domain occasions within a slot.
    uint8_t nof_prach_occasions = prach_cfg.nof_occasions_within_slot;

    // Generate PRACH occasions for each frequency domain occasion.
    for (uint8_t index_fd_ra = 0; index_fd_ra != nof_fd_ra; ++index_fd_ra) {
      expected_occasions.emplace_back(prach_occasion_info{.pci                  = pci,
                                                          .nof_prach_occasions  = nof_prach_occasions,
                                                          .format               = prach_cfg.format,
                                                          .index_fd_ra          = index_fd_ra,
                                                          .start_symbol         = symbols.start(),
                                                          .nof_cs               = nof_cs,
                                                          .nof_fd_ra            = nof_fd_ra,
                                                          .start_preamble_index = preamble_index_range.start(),
                                                          .nof_preamble_indexes = preamble_index_range.length()});
    }
  }

  /// Gets the PRACH scheduler results for the last executed slot.
  span<const prach_occasion_info> get_prach_sched_result() const { return (*res_grid)[0].result.ul.prachs; }

  /// Gets the PRACH scheduler expected results for the last executed slot.
  span<const prach_occasion_info> get_expected_prach_sched_result(slot_point slot) const
  {
    // Return the expected occasions if the slot matches with an active PRACH slot.
    if (active_slots.test(slot.system_slot() % nof_slots_period)) {
      return expected_occasions;
    }

    return {};
  }

  static constexpr interval<unsigned> preamble_index_range = interval<unsigned>(0, 64);
  /// Maximum number of frames in a PRACH period.
  static constexpr unsigned nof_frames_period = 16;
  /// Maximum number of slots in a PRACH period, based on the maximum subcarrier spacing supported.
  static constexpr unsigned max_nof_slots_per_period =
      get_nof_slots_per_subframe(subcarrier_spacing::kHz120) * NOF_SUBFRAMES_PER_FRAME * nof_frames_period;

  prach_configuration                      prach_cfg;
  const scheduler_expert_config            sched_cfg{config_helpers::make_default_scheduler_expert_config()};
  std::unique_ptr<cell_configuration>      cell_cfg;
  std::unique_ptr<prach_scheduler>         prach_sch;
  std::unique_ptr<cell_resource_allocator> res_grid;
  unsigned                                 nof_slots_period;
  bounded_bitset<max_nof_slots_per_period> active_slots;
  slot_point                               slot_begin;
  slot_point                               slot_end;
  std::vector<prach_occasion_info>         expected_occasions;
};

TEST_P(prach_tester_vector, prach_sched_allocation)
{
  for (slot_point slot = slot_begin; slot != slot_end; ++slot) {
    res_grid->slot_indication(slot);
    prach_sch->run_slot(*res_grid);

    ASSERT_EQ(get_prach_sched_result(), get_expected_prach_sched_result(slot)) << fmt::format(
        "slot={} active_slots=[{}] idx={}", slot, GetParam().active_slots, GetParam().prach_config_index);
  }
}

INSTANTIATE_TEST_SUITE_P(prach_scheduler_from_test_data,
                         prach_tester_vector,
                         testing::ValuesIn(prach_scheduler_test_data),
                         [](const testing::TestParamInfo<prach_tester_vector::ParamType>& info_) {
                           return fmt::format("duplex_mode_{}_pusch_scs_{}_prach_cfg_idx_{}",
                                              to_string(info_.param.dplx_mode),
                                              to_string(info_.param.pusch_scs),
                                              info_.param.prach_config_index);
                         });
