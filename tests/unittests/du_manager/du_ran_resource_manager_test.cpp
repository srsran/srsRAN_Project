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

#include "lib/du_manager/ran_resource_management/du_ran_resource_manager_impl.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

struct params {
  duplex_mode duplx_mode;
};

class du_ran_resource_manager_tester : public ::testing::TestWithParam<params>
{
protected:
  explicit du_ran_resource_manager_tester(
      const cell_config_builder_params& params = {.dl_arfcn = GetParam().duplx_mode == duplex_mode::FDD ? 365000U
                                                                                                        : 520002U}) :
    cell_cfg_list({config_helpers::make_default_du_cell_config(params)}),
    qos_cfg_list(config_helpers::make_default_du_qos_config_list(1000)),
    default_ue_cell_cfg(config_helpers::create_default_initial_ue_serving_cell_config(params)),
    res_mng(std::make_unique<du_ran_resource_manager_impl>(cell_cfg_list, srb_cfg_list, qos_cfg_list))
  {
  }

  ue_ran_resource_configurator& create_ue(du_ue_index_t ue_index)
  {
    ues.emplace(ue_index, res_mng->create_ue_resource_configurator(ue_index, to_du_cell_index(0)));
    return ues[ue_index];
  }

  static f1ap_ue_context_update_request srb1_creation_req(du_ue_index_t ue_index)
  {
    f1ap_ue_context_update_request req;
    req.ue_index = ue_index;
    req.srbs_to_setup.resize(1);
    req.srbs_to_setup[0] = srb_id_t::srb1;
    return req;
  }

  unsigned get_config_sr_period() const
  {
    return sr_periodicity_to_slot(default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].period);
  }

  unsigned get_nof_sr_slot_offsets() const
  {
    const unsigned slots_per_frame = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg_list[0].scs_common);
    const auto&    sr_res_list     = default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    unsigned       nof_offsets     = 0;
    for (unsigned i = 0; i != sr_res_list.size(); ++i) {
      const unsigned sr_period_slots = sr_periodicity_to_slot(sr_res_list[i].period);
      if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
        for (unsigned j = 0; j != sr_period_slots; ++j) {
          if (get_active_tdd_ul_symbols(
                  *cell_cfg_list[0].tdd_ul_dl_cfg_common, j % slots_per_frame, cyclic_prefix::NORMAL)
                  .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
            nof_offsets++;
          }
        }
      } else {
        nof_offsets += sr_period_slots;
      }
    }
    // Note: right now we are using two PUCCH resources for SR.
    return nof_offsets * 2;
  }

  bool verify_pucch_cfg(const pucch_config& pucch_cfg, optional<unsigned> csi_pucch_res)
  {
    const du_cell_config& du_cfg                = cell_cfg_list[0];
    const unsigned        nof_sr_f1_res_per_ue  = 1U;
    const unsigned        nof_csi_f2_res_per_ue = 1U;
    bool pucch_checker = pucch_cfg.pucch_res_list.size() == du_cfg.pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() +
                                                                du_cfg.pucch_cfg.nof_ue_pucch_f2_res_harq.to_uint() +
                                                                nof_sr_f1_res_per_ue + nof_csi_f2_res_per_ue;

    // Check whether the SR resource point to the correct one (we give a range where the SR resource is located), each
    // UE can have different values within this range.
    pucch_checker =
        pucch_checker and
        pucch_cfg.sr_res_list.front().pucch_res_id >= du_cfg.pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() and
        pucch_cfg.sr_res_list.front().pucch_res_id <
            du_cfg.pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() + du_cfg.pucch_cfg.nof_sr_resources;

    // We always put the CSI PUCCH resource is always at the end of the list.
    if (csi_pucch_res.has_value()) {
      pucch_checker =
          pucch_checker and csi_pucch_res.value() >= du_cfg.pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() +
                                                         du_cfg.pucch_cfg.nof_sr_resources +
                                                         du_cfg.pucch_cfg.nof_ue_pucch_f2_res_harq.to_uint();
    }

    return pucch_checker;
  }

  std::vector<du_cell_config>                                 cell_cfg_list;
  std::map<srb_id_t, du_srb_config>                           srb_cfg_list;
  std::map<five_qi_t, du_qos_config>                          qos_cfg_list;
  const serving_cell_config                                   default_ue_cell_cfg;
  std::unique_ptr<du_ran_resource_manager>                    res_mng;
  slotted_array<ue_ran_resource_configurator, MAX_NOF_DU_UES> ues;
};

TEST_P(du_ran_resource_manager_tester, when_ue_resource_config_is_created_then_pcell_is_configured)
{
  const du_ue_index_t                 ue_idx1 = to_du_ue_index(0);
  const ue_ran_resource_configurator& ue_res  = create_ue(ue_idx1);

  ASSERT_FALSE(ue_res.empty());
  ASSERT_EQ(ue_res->cells.size(), 1);
  ASSERT_TRUE(ue_res->cells.contains(0));
  ASSERT_TRUE(ue_res->rlc_bearers.empty());
  ASSERT_EQ(ue_res->cells[0].serv_cell_cfg.cell_index, to_du_cell_index(0));
  ASSERT_EQ(ue_res->cells[0].serv_cell_idx, SERVING_CELL_PCELL_IDX);
  ASSERT_FALSE(ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list.empty());
  ASSERT_FALSE(ue_res->mcg_cfg.scheduling_request_config.empty());
}

TEST_P(du_ran_resource_manager_tester, when_srb1_is_added_then_ue_resource_config_is_updated)
{
  const du_ue_index_t           ue_idx1 = to_du_ue_index(0);
  ue_ran_resource_configurator& ue_res  = create_ue(ue_idx1);
  auto                          resp    = ue_res.update(to_du_cell_index(0), srb1_creation_req(ue_idx1));

  ASSERT_FALSE(resp.release_required);
  ASSERT_TRUE(resp.failed_srbs.empty());
  ASSERT_EQ(ue_res->rlc_bearers.size(), 1);
  ASSERT_EQ(ue_res->rlc_bearers[0].lcid, srsran::LCID_SRB1);
  ASSERT_EQ(ue_res->rlc_bearers[0].rlc_cfg.mode, rlc_mode::am);
}

TEST_P(du_ran_resource_manager_tester, when_multiple_ues_are_created_then_they_use_different_sr_offsets)
{
  const unsigned sr_period       = get_config_sr_period();
  const unsigned slots_per_frame = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg_list[0].scs_common);
  const unsigned nof_avail_sr_offsets = this->get_nof_sr_slot_offsets();
  du_ue_index_t  next_ue_index        = to_du_ue_index(0);

  // > Created UEs have unique (PUCCH resource, SR offset) pairs.
  std::set<std::pair<unsigned, unsigned>> sr_offsets;
  for (unsigned i = 0; i != nof_avail_sr_offsets; ++i) {
    const ue_ran_resource_configurator& ue_res = create_ue(next_ue_index);
    ASSERT_FALSE(ue_res.empty());
    const auto& sr_res_list = ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    ASSERT_FALSE(sr_res_list.empty());
    ASSERT_EQ(sr_periodicity_to_slot(sr_res_list[0].period), sr_period);
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      ASSERT_TRUE(get_active_tdd_ul_symbols(*cell_cfg_list[0].tdd_ul_dl_cfg_common,
                                            sr_res_list[0].offset % slots_per_frame,
                                            cyclic_prefix::NORMAL)
                      .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    }
    ASSERT_EQ(sr_offsets.count(std::make_pair(sr_res_list[0].pucch_res_id, sr_res_list[0].offset)), 0);
    sr_offsets.insert(std::make_pair(sr_res_list[0].pucch_res_id, sr_res_list[0].offset));

    // Check if PUCCH config is correctly updated.
    const serving_cell_config serving_cell_cfg = ue_res->cells[0].serv_cell_cfg;
    optional<unsigned>        csi_pucch_res{};
    const bool                has_csi_cfg =
        serving_cell_cfg.csi_meas_cfg.has_value() and
        not serving_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty() and
        srsran::variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
            serving_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
    if (has_csi_cfg) {
      csi_pucch_res.emplace(srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                serving_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type)
                                .pucch_csi_res_list.front()
                                .pucch_res_id);
    }
    ASSERT_TRUE(
        verify_pucch_cfg(ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value(), csi_pucch_res));

    next_ue_index = to_du_ue_index((unsigned)next_ue_index + 1);
  }

  {
    // > No more SR offsets available. UE Resource Allocation fails.
    const ue_ran_resource_configurator& empty_ue_res = create_ue(next_ue_index);
    ASSERT_TRUE(empty_ue_res.empty());
    ues.erase(next_ue_index);
  }

  // Removing one UE, should make one SR offset available.
  const du_ue_index_t ue_idx_to_rem = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, ues.size() - 1));
  const unsigned      rem_pucch_resource =
      ues[ue_idx_to_rem]->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].pucch_res_id;
  const unsigned rem_sr_offset =
      ues[ue_idx_to_rem]->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset;
  ues.erase(ue_idx_to_rem);
  next_ue_index                              = to_du_ue_index((unsigned)next_ue_index + 1);
  const ue_ran_resource_configurator& ue_res = create_ue(next_ue_index);
  ASSERT_FALSE(ue_res.empty());
  ASSERT_EQ(rem_pucch_resource,
            ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].pucch_res_id);
  ASSERT_EQ(rem_sr_offset, ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset);
}

INSTANTIATE_TEST_SUITE_P(du_ran_resource_manager_tester,
                         du_ran_resource_manager_tester,
                         ::testing::Values(params{duplex_mode::FDD}, params{duplex_mode::TDD}));

/////////     Test DU RAN resource manager for multiple UE PUCCH configurations      /////////

namespace du_test_multiple_pucch_cfg {

struct pucch_cfg_builder_params {
  unsigned nof_res_f1_harq = 3;
  unsigned nof_res_f2_harq = 6;
  unsigned nof_harq_cfg    = 1;
  unsigned nof_res_sr      = 2;
  unsigned nof_res_csi     = 1;
};

} // namespace du_test_multiple_pucch_cfg

using namespace du_test_multiple_pucch_cfg;

static du_cell_config make_custom_du_cell_config(const pucch_cfg_builder_params& pucch_params_)
{
  du_cell_config du_cfg                     = config_helpers::make_default_du_cell_config();
  auto&          pucch_params               = du_cfg.pucch_cfg;
  pucch_params.nof_ue_pucch_f1_res_harq     = pucch_params_.nof_res_f1_harq;
  pucch_params.nof_ue_pucch_f2_res_harq     = pucch_params_.nof_res_f2_harq;
  pucch_params.nof_sr_resources             = pucch_params_.nof_res_sr;
  pucch_params.nof_csi_resources            = pucch_params_.nof_res_csi;
  pucch_params.nof_cell_harq_pucch_res_sets = pucch_params_.nof_harq_cfg;
  pucch_params.f1_params.nof_cyc_shifts     = srsran::nof_cyclic_shifts::six;
  pucch_params.f1_params.occ_supported      = true;

  return du_cfg;
}

static const auto* get_pucch_resource_with_id(const pucch_config& pucch_cfg, unsigned res_id)
{
  return std::find_if(pucch_cfg.pucch_res_list.begin(),
                      pucch_cfg.pucch_res_list.end(),
                      [res_id](const pucch_resource& res) { return res.res_id == res_id; });
};

class du_ran_res_mng_multiple_cfg_tester : public ::testing::TestWithParam<pucch_cfg_builder_params>
{
protected:
  du_ran_res_mng_multiple_cfg_tester() :
    cell_cfg_list({make_custom_du_cell_config(GetParam())}),
    qos_cfg_list(config_helpers::make_default_du_qos_config_list(1000)),
    default_ue_cell_cfg(config_helpers::create_default_initial_ue_serving_cell_config()),
    res_mng(std::make_unique<du_ran_resource_manager_impl>(cell_cfg_list, srb_cfg_list, qos_cfg_list))
  {
    srsran_assert(default_ue_cell_cfg.csi_meas_cfg.has_value() and
                      not default_ue_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty() and
                      srsran::variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                          default_ue_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type),
                  "CSI report configuration is required for this unittest;");
    default_csi_pucch_res_cfg = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        default_ue_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
  }

  ue_ran_resource_configurator& create_ue(du_ue_index_t ue_index)
  {
    ues.emplace(ue_index, res_mng->create_ue_resource_configurator(ue_index, to_du_cell_index(0)));
    return ues[ue_index];
  }

  bool has_ue_csi_cfg(const serving_cell_config& serv_cell_cfg) const
  {
    return serv_cell_cfg.csi_meas_cfg.has_value() and
           not serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty() and
           srsran::variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
               serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
  }

  const csi_report_config::periodic_or_semi_persistent_report_on_pucch&
  get_ue_csi_cfg(const serving_cell_config& serv_cell_cfg) const
  {
    srsran_assert(has_ue_csi_cfg(serv_cell_cfg), "CSI configuration not found");

    return srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
  }

  unsigned get_config_sr_period() const
  {
    return sr_periodicity_to_slot(default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].period);
  }

  unsigned get_config_csi_period() const
  {
    return csi_report_periodicity_to_uint(default_csi_pucch_res_cfg.report_slot_period);
  }

  // Retrieve the interval [ ID_start, ID_stop] with the initial and final IDs of the UE's PUCCH resources used for
  // HARQ from the UE's PUCCH-Config.
  interval<unsigned, true> get_pucch_res_id_interval(const pucch_config& pucch_cfg, pucch_format format) const
  {
    const unsigned pucch_res_set_id            = format == pucch_format::FORMAT_1 ? 0U : 1U;
    const auto&    pucch_res_set               = pucch_cfg.pucch_res_set[pucch_res_set_id].pucch_res_id_list;
    const unsigned expected_pucch_res_set_size = format == pucch_format::FORMAT_1
                                                     ? cell_cfg_list[0].pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint()
                                                     : cell_cfg_list[0].pucch_cfg.nof_ue_pucch_f2_res_harq.to_uint();
    if (expected_pucch_res_set_size != pucch_res_set.size()) {
      return {};
    }

    const auto* pucch_res_id_start_it = get_pucch_resource_with_id(pucch_cfg, pucch_res_set.front());
    const auto* pucch_res_id_stop_it  = get_pucch_resource_with_id(pucch_cfg, pucch_res_set.back());
    if (pucch_res_id_start_it != pucch_cfg.pucch_res_list.end() and
        pucch_res_id_stop_it != pucch_cfg.pucch_res_list.end()) {
      return {pucch_res_id_start_it->res_id, pucch_res_id_stop_it->res_id};
    }

    return {};
  }

  // Retrieve the expected interval [ ID_start, ID_stop] with the initial and final IDs of the UE's PUCCH resources used
  // for HARQ.
  interval<unsigned, true> get_expected_pucch_res_id_interval(unsigned ue_idx, pucch_format format) const
  {
    const unsigned expected_nof_pucch_res = format == pucch_format::FORMAT_1
                                                ? cell_cfg_list[0].pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint()
                                                : cell_cfg_list[0].pucch_cfg.nof_ue_pucch_f2_res_harq.to_uint();

    if (expected_nof_pucch_res == 0) {
      return interval<unsigned, true>{};
    }

    const unsigned nof_harq_cfgs = cell_cfg_list[0].pucch_cfg.nof_cell_harq_pucch_res_sets;
    const unsigned f2_res_idx_offset =
        format == pucch_format::FORMAT_1
            ? 0U
            : cell_cfg_list[0].pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() * nof_harq_cfgs +
                  cell_cfg_list[0].pucch_cfg.nof_sr_resources;
    return {f2_res_idx_offset + (ue_idx % nof_harq_cfgs) * expected_nof_pucch_res,
            f2_res_idx_offset + (ue_idx % nof_harq_cfgs) * expected_nof_pucch_res + expected_nof_pucch_res - 1};
  }

  // Get the number of available resources and offsets for SR/CSI. The return value is the minimum(available_SR_offset *
  // available_SR_resources, available_CSI_offset * available_CSI_resources). The booleans indicates whether the
  // resources and offsets are limited by the SR (bool_0 == true) or CSI (bool_1 == true), or both (bool_0 == true and
  // bool_1 == true).
  std::tuple<unsigned, bool, bool> get_nof_available_resources() const
  {
    const unsigned slots_per_frame = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg_list[0].scs_common);
    const auto&    sr_res_list     = default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    unsigned       nof_sr_offsets  = 0;
    unsigned       nof_csi_offsets = 0;

    // Get the available offsets for SR.
    const unsigned sr_period_slots = sr_periodicity_to_slot(sr_res_list.front().period);
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      for (unsigned j = 0; j != sr_period_slots; ++j) {
        if (get_active_tdd_ul_symbols(
                *cell_cfg_list[0].tdd_ul_dl_cfg_common, j % slots_per_frame, cyclic_prefix::NORMAL)
                .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
          nof_sr_offsets++;
        }
      }
    } else {
      nof_sr_offsets += sr_period_slots;
    }
    const unsigned nof_avail_sr_res = nof_sr_offsets * cell_cfg_list[0].pucch_cfg.nof_sr_resources;

    // Get the available offsets for CSI.
    const unsigned csi_period_slots = csi_report_periodicity_to_uint(default_csi_pucch_res_cfg.report_slot_period);
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      for (unsigned j = 0; j != csi_period_slots; ++j) {
        if (get_active_tdd_ul_symbols(
                *cell_cfg_list[0].tdd_ul_dl_cfg_common, j % slots_per_frame, cyclic_prefix::NORMAL)
                .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
          nof_csi_offsets++;
        }
      }
    } else {
      nof_csi_offsets += csi_period_slots;
    }

    const unsigned nof_avail_csi_res = nof_csi_offsets * cell_cfg_list[0].pucch_cfg.nof_csi_resources;

    // Note: right now we are using two PUCCH resources for SR.
    return {std::min(nof_avail_sr_res, nof_avail_csi_res),
            nof_avail_sr_res <= nof_avail_csi_res,
            nof_avail_csi_res <= nof_avail_sr_res};
  }

  std::vector<du_cell_config>                                            cell_cfg_list;
  std::map<srb_id_t, du_srb_config>                                      srb_cfg_list;
  std::map<five_qi_t, du_qos_config>                                     qos_cfg_list;
  const serving_cell_config                                              default_ue_cell_cfg;
  srsran::csi_report_config::periodic_or_semi_persistent_report_on_pucch default_csi_pucch_res_cfg;
  std::unique_ptr<du_ran_resource_manager>                               res_mng;
  slotted_array<ue_ran_resource_configurator, MAX_NOF_DU_UES>            ues;
};

TEST_P(du_ran_res_mng_multiple_cfg_tester, test_correct_resource_creation_indexing)
{
  const unsigned sr_period       = get_config_sr_period();
  const unsigned csi_period      = get_config_csi_period();
  const unsigned slots_per_frame = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg_list[0].scs_common);
  std::tuple<unsigned, bool, bool> avail_res     = get_nof_available_resources();
  du_ue_index_t                    next_ue_index = to_du_ue_index(0);

  // > Created UEs have unique (PUCCH resource, SR offset) pairs.
  std::set<std::pair<unsigned, unsigned>> sr_offsets;
  std::set<std::pair<unsigned, unsigned>> csi_offsets;
  for (unsigned i = 0; i != std::get<0>(avail_res); ++i) {
    const ue_ran_resource_configurator& ue_res = create_ue(next_ue_index);
    ASSERT_FALSE(ue_res.empty());

    // Check if the SR has been assigned to the UE.
    const auto& sr_res_list = ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    ASSERT_FALSE(sr_res_list.empty());
    ASSERT_EQ(sr_periodicity_to_slot(sr_res_list[0].period), sr_period);
    // Make sure the SR is in a fully-UL slot.
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      ASSERT_TRUE(get_active_tdd_ul_symbols(*cell_cfg_list[0].tdd_ul_dl_cfg_common,
                                            sr_res_list[0].offset % slots_per_frame,
                                            cyclic_prefix::NORMAL)
                      .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    }
    ASSERT_EQ(sr_offsets.count(std::make_pair(sr_res_list[0].pucch_res_id, sr_res_list[0].offset)), 0);
    sr_offsets.insert(std::make_pair(sr_res_list[0].pucch_res_id, sr_res_list[0].offset));

    // Check if the CSI has been assigned to the UE.
    ASSERT_TRUE(has_ue_csi_cfg(ue_res->cells[0].serv_cell_cfg));
    const auto& ue_csi_cfg = get_ue_csi_cfg(ue_res->cells[0].serv_cell_cfg);
    ASSERT_FALSE(ue_csi_cfg.pucch_csi_res_list.empty());
    const unsigned ue_csi_pucch_res_id = ue_csi_cfg.pucch_csi_res_list.front().pucch_res_id;
    const unsigned ue_csi_pucch_offset = ue_csi_cfg.report_slot_offset;
    ASSERT_EQ(csi_period, csi_report_periodicity_to_uint(ue_csi_cfg.report_slot_period));
    // Make sure the CSI is in a fully-UL slot.
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      ASSERT_TRUE(get_active_tdd_ul_symbols(*cell_cfg_list[0].tdd_ul_dl_cfg_common,
                                            ue_csi_pucch_offset % slots_per_frame,
                                            cyclic_prefix::NORMAL)
                      .length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    }
    ASSERT_EQ(csi_offsets.count(std::make_pair(ue_csi_pucch_res_id, ue_csi_pucch_offset)), 0);
    csi_offsets.insert(std::make_pair(ue_csi_pucch_res_id, ue_csi_pucch_offset));

    // Check if PUCCH resources for HARQ indexing is distributed over different HARQ configurations.
    const interval<unsigned, true> expected_f1 =
        get_expected_pucch_res_id_interval(static_cast<unsigned>(next_ue_index), srsran::pucch_format::FORMAT_1);
    const interval<unsigned, true> actual_f1 = get_pucch_res_id_interval(
        ue_res->cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value(), srsran::pucch_format::FORMAT_1);
    const interval<unsigned, true> expected_f2 =
        get_expected_pucch_res_id_interval(static_cast<unsigned>(next_ue_index), srsran::pucch_format::FORMAT_2);
    const interval<unsigned, true> actual_f2 = get_pucch_res_id_interval(
        ue_res->cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value(), srsran::pucch_format::FORMAT_2);

    ASSERT_TRUE(expected_f1.start() == actual_f1.start() and expected_f1.stop() == actual_f1.stop());
    ASSERT_TRUE(expected_f2.start() == actual_f2.start() and expected_f2.stop() == actual_f2.stop());

    next_ue_index = to_du_ue_index((unsigned)next_ue_index + 1);
  }

  {
    // > No more SR offsets available. UE Resource Allocation fails.
    const ue_ran_resource_configurator& empty_ue_res = create_ue(next_ue_index);
    ASSERT_TRUE(empty_ue_res.empty());
    ues.erase(next_ue_index);
  }

  // Remove 1 UE and verify if the new resource can be allocated to another UE.
  const du_ue_index_t ue_idx_to_rem = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, ues.size() - 1));
  const unsigned      rem_sr_pucch_resource =
      ues[ue_idx_to_rem]->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].pucch_res_id;
  const unsigned rem_sr_offset =
      ues[ue_idx_to_rem]->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset;
  const unsigned rem_csi_pucch_resource_id =
      get_ue_csi_cfg(ues[ue_idx_to_rem]->cells[0].serv_cell_cfg).pucch_csi_res_list.front().pucch_res_id;
  const unsigned rem_csi_offset = get_ue_csi_cfg(ues[ue_idx_to_rem]->cells[0].serv_cell_cfg).report_slot_offset;

  ues.erase(ue_idx_to_rem);
  next_ue_index                              = to_du_ue_index((unsigned)next_ue_index + 1);
  const ue_ran_resource_configurator& ue_res = create_ue(next_ue_index);
  ASSERT_FALSE(ue_res.empty());

  // If the resources and offset were limited by the SR, then check if a new SR can be allocated.
  const bool nof_ue_limited_by_sr_resources = std::get<1>(avail_res);
  if (nof_ue_limited_by_sr_resources) {
    ASSERT_EQ(rem_sr_pucch_resource,
              ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].pucch_res_id);
    ASSERT_EQ(rem_sr_offset, ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset);
  }

  // If the resources and offset were limited by the CSI, then check if a new CSI can be allocated.
  const bool nof_ue_limited_by_csi_resources = std::get<2>(avail_res);
  if (nof_ue_limited_by_csi_resources) {
    ASSERT_EQ(rem_csi_pucch_resource_id,
              get_ue_csi_cfg(ue_res->cells[0].serv_cell_cfg).pucch_csi_res_list.front().pucch_res_id);
    ASSERT_EQ(rem_csi_offset, get_ue_csi_cfg(ue_res->cells[0].serv_cell_cfg).report_slot_offset);
  }
}

INSTANTIATE_TEST_SUITE_P(different_f1_f2_resources,
                         du_ran_res_mng_multiple_cfg_tester,
                         // clang-format off
                         //                                   nof:  f1  |  f2  | harq | sr | csi
                         //                                   nof:  f1  |  f2  | cfg  | sr | csi
                         ::testing::Values(pucch_cfg_builder_params{ 3,     6,     1,    4,   1 },
                                           pucch_cfg_builder_params{ 3,     6,     3,    6,   3 },
                                           pucch_cfg_builder_params{ 8,     8,     5,    4,   7 },
                                           pucch_cfg_builder_params{ 8,     3,     1,    1,   1 },
                                           pucch_cfg_builder_params{ 5,     7,     7,    4,   9 },
                                           pucch_cfg_builder_params{ 8,     8,     8,    4,   10 })
                         // clang-format on
);
