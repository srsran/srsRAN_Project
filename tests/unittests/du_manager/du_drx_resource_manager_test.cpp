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

#include "lib/du/du_high/du_manager/ran_resource_management/du_drx_resource_manager.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/support/test_utils.h"
#include "gtest/gtest.h"

using namespace srsran;
using namespace srs_du;

static cell_config_builder_params get_tdd_builder_params()
{
  cell_config_builder_params params;
  params.scs_common     = subcarrier_spacing::kHz30;
  params.dl_f_ref_arfcn = 520002;
  params.band           = nr_band::n41;
  return params;
}

static du_cell_config create_du_cell_config(const cell_config_builder_params& params,
                                            const std::optional<drx_params>&  drx)
{
  du_cell_config cell = config_helpers::make_default_du_cell_config(params);
  cell.mcg_params.drx = drx;
  return cell;
}

class base_du_drx_resource_manager_test
{
protected:
  base_du_drx_resource_manager_test(const std::optional<drx_params>& drx_params_) :
    drx_args(drx_params_), cell_cfg_list({create_du_cell_config(params, drx_args)})
  {
  }

  cell_group_config& add_ue(du_ue_index_t ue_idx)
  {
    auto ret = ues.insert(std::make_pair(ue_idx, cell_group_config{}));
    report_fatal_error_if_not(ret.second, "Repeated UE index");

    auto& u = ues.at(ue_idx);
    u.cells.emplace(SERVING_CELL_PCELL_IDX,
                    cell_config_dedicated{serv_cell_index_t::SERVING_CELL_PCELL_IDX,
                                          config_helpers::create_default_initial_ue_serving_cell_config(params)});

    // On UE creation, handle_ue_creation is always called.
    drx.handle_ue_creation(u);

    return u;
  }

  bool is_valid_drx_config(const std::optional<drx_config>& ue_drx_cfg) const
  {
    if (drx_args.has_value() != ue_drx_cfg.has_value()) {
      return false;
    }
    if (not drx_args.has_value()) {
      return true;
    }
    if (drx_args->inactivity_timer != ue_drx_cfg->inactivity_timer or drx_args->long_cycle != ue_drx_cfg->long_cycle or
        drx_args->on_duration != ue_drx_cfg->on_duration_timer or
        drx_args->retx_timer_dl != ue_drx_cfg->retx_timer_dl or drx_args->retx_timer_ul != ue_drx_cfg->retx_timer_ul) {
      return false;
    }
    return true;
  }

  cell_config_builder_params      params{get_tdd_builder_params()};
  const std::optional<drx_params> drx_args;
  std::vector<du_cell_config>     cell_cfg_list;
  du_drx_resource_manager         drx{cell_cfg_list};

  std::map<du_ue_index_t, cell_group_config> ues;
};

class du_no_drx_resource_manager_test : public base_du_drx_resource_manager_test, public ::testing::Test
{
protected:
  du_no_drx_resource_manager_test() : base_du_drx_resource_manager_test(std::nullopt)
  {
    test_ue = &add_ue(to_du_ue_index(0));
  }

  cell_group_config* test_ue = nullptr;
};

TEST_F(du_no_drx_resource_manager_test, when_gnb_does_not_support_drx_but_ue_does_then_no_drx_config_is_generated)
{
  // The UE starts without DRX config.
  ASSERT_FALSE(test_ue->mcg_cfg.drx_cfg.has_value());

  // Once UE capabilities are configured, the result should still an empty DRX config.
  drx.handle_ue_cap_update(*test_ue, true);
  ASSERT_FALSE(test_ue->mcg_cfg.drx_cfg.has_value());

  // Deallocation should be a no-op, but it is good practice to see if it does not crash.
  drx.handle_ue_removal(*test_ue);
  ASSERT_FALSE(test_ue->mcg_cfg.drx_cfg.has_value());
}

class du_drx_resource_manager_test : public base_du_drx_resource_manager_test, public ::testing::Test
{
protected:
  du_drx_resource_manager_test() :
    base_du_drx_resource_manager_test(
        drx_params{std::chrono::milliseconds{10}, std::chrono::milliseconds{60}, std::chrono::milliseconds{10}})
  {
  }
};

TEST_F(du_drx_resource_manager_test, when_gnb_and_ue_support_drx_then_drx_config_is_valid)
{
  cell_group_config& test_ue = add_ue(to_du_ue_index(0));

  // DRX supported.
  drx.handle_ue_cap_update(test_ue, true);
  ASSERT_TRUE(test_ue.mcg_cfg.drx_cfg.has_value());
  ASSERT_TRUE(this->is_valid_drx_config(test_ue.mcg_cfg.drx_cfg));

  // Removal of UE should remove DRX config.
  drx.handle_ue_removal(test_ue);
  ASSERT_FALSE(test_ue.mcg_cfg.drx_cfg.has_value());
}

TEST_F(du_drx_resource_manager_test, when_gnb_supports_drx_but_ue_does_not_then_drx_is_not_set)
{
  cell_group_config& test_ue = add_ue(to_du_ue_index(0));

  // DRX supported.
  drx.handle_ue_cap_update(test_ue, false);
  ASSERT_FALSE(test_ue.mcg_cfg.drx_cfg.has_value());
  ASSERT_FALSE(this->is_valid_drx_config(test_ue.mcg_cfg.drx_cfg));

  // Removal of UE should be no-op.
  drx.handle_ue_removal(test_ue);
  ASSERT_FALSE(test_ue.mcg_cfg.drx_cfg.has_value());
}

TEST_F(du_drx_resource_manager_test, when_multiple_ues_are_created_then_drx_configs_avoid_same_offset)
{
  const unsigned                  nof_ues = 2;
  std::vector<cell_group_config*> test_ues;
  for (unsigned i = 0; i < nof_ues; ++i) {
    test_ues.emplace_back(&add_ue(to_du_ue_index(i)));
  }

  std::set<std::chrono::milliseconds> offsets;
  for (cell_group_config* u : test_ues) {
    drx.handle_ue_cap_update(*u, true);
    ASSERT_TRUE(u->mcg_cfg.drx_cfg.has_value());
    ASSERT_TRUE(this->is_valid_drx_config(u->mcg_cfg.drx_cfg));
    auto p = offsets.insert(u->mcg_cfg.drx_cfg->long_start_offset);
    ASSERT_TRUE(p.second) << "DRX offset should have been unique";
  }
}

TEST_F(du_drx_resource_manager_test, when_ue_drx_resources_are_deallocated_then_they_are_reused_by_new_ues)
{
  const unsigned nof_ues = (drx_args->long_cycle / drx_args->on_duration) * 10;

  std::vector<cell_group_config*> test_ues;
  test_ues.reserve(nof_ues);
  for (unsigned i = 0; i < nof_ues; ++i) {
    test_ues.emplace_back(&add_ue(to_du_ue_index(i)));
  }

  // Allocate DRX config and build histogram of DRX offsets.
  std::map<std::chrono::milliseconds, unsigned> histogram;
  for (cell_group_config* u : test_ues) {
    drx.handle_ue_cap_update(*u, true);
    ASSERT_TRUE(this->is_valid_drx_config(u->mcg_cfg.drx_cfg));
    if (histogram.count(u->mcg_cfg.drx_cfg->long_start_offset)) {
      histogram[u->mcg_cfg.drx_cfg->long_start_offset]++;
    } else {
      histogram.emplace(u->mcg_cfg.drx_cfg->long_start_offset, 1);
    }
  }

  // Select one DRX offset at random, and deallocate DRX resources of some UEs that picked it.
  // > Select offset at random.
  unsigned offset_to_rem_idx = test_rgen::uniform_int<unsigned>(0, histogram.size() - 1);
  auto     it                = histogram.begin();
  std::advance(it, offset_to_rem_idx);
  const std::chrono::milliseconds selected_offset = it->first;
  // > Make the selected offset the new minimum of the histogram.
  unsigned min_count = std::min_element(histogram.begin(), histogram.end(), [](const auto& lhs, const auto& rhs) {
                         return lhs.second < rhs.second;
                       })->second;
  unsigned nof_rems  = std::min(it->second - min_count + 1, it->second);
  unsigned rem_count = 0;
  for (cell_group_config* u : test_ues) {
    if (rem_count == nof_rems) {
      break;
    }
    if (u->mcg_cfg.drx_cfg->long_start_offset == selected_offset) {
      drx.handle_ue_removal(*u);
      rem_count++;
    }
  }

  // Next UE allocation should get assigned the previously selected DRX offset that is now in minority.
  auto& u = add_ue(to_du_ue_index(nof_ues));
  drx.handle_ue_cap_update(u, true);
  ASSERT_TRUE(this->is_valid_drx_config(u.mcg_cfg.drx_cfg));
  ASSERT_EQ(u.mcg_cfg.drx_cfg->long_start_offset, selected_offset);
}
