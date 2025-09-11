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

expected<du_cell_reconfig_result> du_cell_manager::handle_cell_reconf_request(const du_cell_param_config_request& req)
{
  du_cell_index_t cell_index = get_cell_index(req.nr_cgi);
  if (cell_index == INVALID_DU_CELL_INDEX) {
    logger.warning("Discarding cell {} changes. Cause: No cell with the provided CGI was found", req.nr_cgi.nci);
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

  if (si_updated) {
    // Need to re-pack SIB1.
    cell.si_cfg.sib1 = asn1_packer::pack_sib1(cell_cfg);

    // Bump SI version and update SI messages.
    fill_si_scheduler_config(cell.si_cfg.si_sched_cfg, cell_cfg, cell.si_cfg.sib1, cell.si_cfg.si_messages);
    cell.si_cfg.si_sched_cfg.version++;
  }

  return du_cell_reconfig_result{cell_index, si_updated, si_updated};
}

async_task<bool> du_cell_manager::start(du_cell_index_t cell_index)
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
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).start());

    cells[cell_index]->state = du_cell_context::state_t::active;

    CORO_RETURN(true);
  });
}

async_task<void> du_cell_manager::stop(du_cell_index_t cell_index)
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
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).stop());

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop_all()
{
  return launch_async([this, i = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    for (; i != cells.size(); ++i) {
      if (cells[i] != nullptr and cells[i]->state == du_cell_context::state_t::active) {
        cells[i]->state = du_cell_context::state_t::inactive;

        CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(to_du_cell_index(i)).stop());
      }
    }

    CORO_RETURN();
  });
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
