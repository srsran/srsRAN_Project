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

#include "du_cell_manager.h"
#include "converters/asn1_sys_info_packer.h"
#include "converters/scheduler_configuration_helpers.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/ran/band_helper.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

du_cell_manager::du_cell_manager(const du_manager_params& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

static void fill_si_scheduler_config(si_scheduling_update_request&        req,
                                     const du_cell_config&                cell_cfg,
                                     const byte_buffer&                   sib1,
                                     span<const bcch_dl_sch_payload_type> si_messages)
{
  req.sib1_len = units::bytes{static_cast<unsigned>(sib1.length())};
  static_vector<units::bytes, MAX_SI_MESSAGES> si_payload_sizes;
  for (const auto& si_msg : si_messages) {
    size_t si_msg_len = si_msg.front().length();
    // If the SI message has multiple segments, check that all segments have the same length.
    if (si_msg.size() > 1) {
      if (!std::all_of(si_msg.begin(), si_msg.end(), [si_msg_len](const byte_buffer& si_msg_) {
            return si_msg_.length() == si_msg_len;
          })) {
        report_error("All segments of an SI message must have the same length.");
      }
    }
    si_payload_sizes.emplace_back(units::bytes{static_cast<unsigned>(si_msg_len)});
  }
  req.si_sched_cfg = make_si_scheduling_info_config(cell_cfg, si_payload_sizes);
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  // Verify that DU cell configuration is valid. Abort application otherwise.
  auto ret = is_du_cell_config_valid(cell_cfg);
  if (not ret.has_value()) {
    report_error("ERROR: Invalid DU Cell Configuration. Cause: {}.\n", ret.error());
  }

  // Generate system information.
  std::vector<bcch_dl_sch_payload_type> bcch_msgs = asn1_packer::pack_all_bcch_dl_sch_msgs(cell_cfg);

  srsran_assert(bcch_msgs[0].size() == 1, "SIB-1 cannot be segmented");
  const byte_buffer& sib1 = bcch_msgs[0].front();

  span<const bcch_dl_sch_payload_type> si_messages =
      span<const bcch_dl_sch_payload_type>(bcch_msgs).last(bcch_msgs.size() - 1);

  // Generate Scheduler SI scheduling config.
  si_scheduling_update_request si_sched_req;
  si_sched_req.cell_index = to_du_cell_index(cells.size());
  si_sched_req.version    = 0;
  fill_si_scheduler_config(si_sched_req, cell_cfg, sib1, si_messages);

  // Save config.
  auto& cell       = *cells.emplace_back(std::make_unique<du_cell_context>());
  cell.cfg         = cell_cfg;
  cell.state       = du_cell_context::state_t::inactive;
  cell.si_cfg.sib1 = sib1.copy();
  cell.si_cfg.si_messages.assign(si_messages.begin(), si_messages.end());
  cell.si_cfg.si_sched_cfg = std::move(si_sched_req);
}

expected<du_cell_reconfig_result>
du_cell_manager::handle_cell_reconf_request(const du_cell_param_config_request& req) const
{
  if (!req.nr_cgi.has_value()) {
    logger.warning("DU Cell Reconfiguration request without NR CGI is not supported");
    return make_unexpected(default_error_t{});
  }

  du_cell_index_t cell_index = get_cell_index(req.nr_cgi.value());
  if (cell_index == INVALID_DU_CELL_INDEX) {
    logger.warning("Discarding cell {} changes. Cause: No cell with the provided CGI was found",
                   req.nr_cgi.value().nci);
    return make_unexpected(default_error_t{});
  }
  auto& cell = *cells[cell_index];

  du_cell_config& cell_cfg   = cell.cfg;
  bool            si_updated = false;

  if (req.ssb_pwr_mod.has_value() and req.ssb_pwr_mod.value() != cell_cfg.ssb_cfg.ssb_block_power) {
    // SSB power changed.
    cell_cfg.ssb_cfg.ssb_block_power = req.ssb_pwr_mod.value();
    si_updated                       = true;
  }

  const unsigned nof_prbs =
      band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(cell_cfg.dl_carrier.carrier_bw_mhz),
                                     cell_cfg.scs_common,
                                     band_helper::get_freq_range(cell_cfg.dl_carrier.band));

  du_cell_reconfig_result result;
  result.slice_reconf_req.emplace();
  for (const auto& rrm_policy_ratio : req.rrm_policy_ratio_list) {
    if (not(rrm_policy_ratio.minimum_ratio.has_value() or rrm_policy_ratio.maximum_ratio.has_value())) {
      continue;
    }

    for (const auto& policy_member : rrm_policy_ratio.policy_members_list) {
      bool found = false;
      for (auto& policy_cfg : cell_cfg.rrm_policy_members) {
        if (policy_cfg.rrc_member == policy_member) {
          found = true;
          // Update the policy member configuration.
          unsigned min_prb_ratio = rrm_policy_ratio.minimum_ratio.value_or(0);
          unsigned max_prb_ratio = rrm_policy_ratio.maximum_ratio.value_or(100);

          min_prb_ratio = std::clamp(min_prb_ratio, static_cast<unsigned>(0), static_cast<unsigned>(100));
          max_prb_ratio = std::clamp(max_prb_ratio, static_cast<unsigned>(0), static_cast<unsigned>(100));

          const unsigned min_prb = static_cast<int>((1.0 * min_prb_ratio / 100) * nof_prbs);
          const unsigned max_prb = static_cast<int>((1.0 * max_prb_ratio / 100) * nof_prbs);

          if (min_prb > max_prb) {
            logger.warning(
                "Invalid min/max PRB policy ratio for {} in cell {}: min_prb={} > max_prb={}. Skipping update.",
                policy_member,
                fmt::underlying(cell_index),
                min_prb,
                max_prb);
            break;
          }

          if ((policy_cfg.rbs.min() != min_prb) or (policy_cfg.rbs.max() != max_prb)) {
            // Policy configuration has been updated.
            result.slice_reconf_req->rrm_policies.push_back(
                du_cell_slice_reconfig_request::rrm_policy_config{policy_member, {min_prb, max_prb}});
          }

          policy_cfg.rbs = {min_prb, max_prb};
          break;
        }
      }
      if (not found) {
        logger.warning("No RRM policy member found for {} in cell {}", policy_member, fmt::underlying(cell_index));
      }

      if (result.slice_reconf_req->rrm_policies.full()) {
        logger.warning("RRM policy update list is full. Discarding further updates for cell {}",
                       fmt::underlying(cell_index));
        break;
      }
    }
  }

  if (si_updated) {
    // Need to re-pack SIB1.
    cell.si_cfg.sib1 = asn1_packer::pack_sib1(cell_cfg);

    // Bump SI version and update SI messages.
    fill_si_scheduler_config(cell.si_cfg.si_sched_cfg, cell_cfg, cell.si_cfg.sib1, cell.si_cfg.si_messages);
    cell.si_cfg.si_sched_cfg.version++;
  }

  result.cell_index           = cell_index;
  result.cu_notif_required    = si_updated;
  result.sched_notif_required = si_updated;
  if (result.slice_reconf_req->rrm_policies.empty()) {
    // No RRM policy changes.
    result.slice_reconf_req.reset();
  }
  return result;
}

async_task<bool> du_cell_manager::start(du_cell_index_t cell_index) const
{
  return launch_async([this, cell_index](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Start called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN(false);
    }
    if (cells[cell_index]->state != du_cell_context::state_t::inactive) {
      logger.warning("cell={}: Start called for an already active cell.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN(false);
    }

    // Start cell in the MAC.
    CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(cell_index).start());

    cells[cell_index]->state = du_cell_context::state_t::active;

    CORO_RETURN(true);
  });
}

async_task<void> du_cell_manager::stop(du_cell_index_t cell_index) const
{
  return launch_async([this, cell_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Stop called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN();
    }
    if (cells[cell_index]->state == du_cell_context::state_t::inactive) {
      // Ignore.
      CORO_EARLY_RETURN();
    }
    cells[cell_index]->state = du_cell_context::state_t::inactive;

    // Stop cell in the MAC.
    CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(cell_index).stop());

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop_all() const
{
  return launch_async([this, i = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    for (; i != cells.size(); ++i) {
      if (cells[i] != nullptr and cells[i]->state == du_cell_context::state_t::active) {
        cells[i]->state = du_cell_context::state_t::inactive;

        CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(to_du_cell_index(i)).stop());
      }
    }

    CORO_RETURN();
  });
}

void du_cell_manager::remove_all_cells()
{
  for (unsigned i = 0; i != cells.size(); ++i) {
    srsran_assert(cells[i] != nullptr, "Cell {} is null", i);
    srsran_assert(cells[i]->state != du_cell_context::state_t::active, "Cell {} is still active", i);
    cfg.mac.mgr.get_cell_manager().remove_cell(to_du_cell_index(i));
  }
  cells.clear();
}

du_cell_index_t du_cell_manager::get_cell_index(nr_cell_global_id_t nr_cgi) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.nr_cgi == nr_cgi) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}

du_cell_index_t du_cell_manager::get_cell_index(pci_t pci) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.pci == pci) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}
