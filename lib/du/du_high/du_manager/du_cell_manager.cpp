/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_manager.h"
#include "converters/asn1_sys_info_packer.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

du_cell_manager::du_cell_manager(const du_manager_params& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  // Verify that DU cell configuration is valid. Abort application otherwise.
  auto ret = is_du_cell_config_valid(cell_cfg);
  if (not ret.has_value()) {
    report_error("ERROR: Invalid DU Cell Configuration. Cause: {}.\n", ret.error());
  }

  // Generate system information.
  std::vector<byte_buffer> bcch_msgs = asn1_packer::pack_all_bcch_dl_sch_msgs(cell_cfg);
  du_cell_packed_sys_info  si_info;
  si_info.sib1 = std::move(bcch_msgs[0]);
  bcch_msgs.erase(bcch_msgs.begin());
  si_info.si_messages = std::move(bcch_msgs);

  // Save config.
  cells.emplace_back(std::make_unique<du_cell_context>());
  cells.back()->cfg             = cell_cfg;
  cells.back()->active          = false;
  cells.back()->packed_sys_info = std::move(si_info);
}

bool du_cell_manager::handle_cell_reconf_request(const du_cell_param_config_request& req)
{
  du_cell_index_t cell_index = get_cell_index(req.nr_cgi);
  if (cell_index == INVALID_DU_CELL_INDEX) {
    logger.warning("Discarding cell {} changes. Cause: No cell with the provided CGI was found", req.nr_cgi.nci);
    return false;
  }

  du_cell_config& cell_cfg     = cells[cell_index]->cfg;
  bool            sib1_updated = false;

  if (req.ssb_pwr_mod.has_value() and req.ssb_pwr_mod.value() != cell_cfg.ssb_cfg.ssb_block_power) {
    // SSB power changed.
    cell_cfg.ssb_cfg.ssb_block_power = req.ssb_pwr_mod.value();
    sib1_updated                     = true;
  }

  if (sib1_updated) {
    // Need to re-pack SIB1.
    cells[cell_index]->packed_sys_info.sib1 = asn1_packer::pack_sib1(cell_cfg);
  }

  return true;
}

async_task<bool> du_cell_manager::start(du_cell_index_t cell_index)
{
  return launch_async([this, cell_index](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Start called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN(false);
    }
    if (cells[cell_index]->active) {
      logger.warning("cell={}: Start called for an already active cell.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN(false);
    }

    // Start cell in the MAC.
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).start());

    cells[cell_index]->active = true;

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
    if (not cells[cell_index]->active) {
      // Ignore.
      CORO_EARLY_RETURN();
    }

    cells[cell_index]->active = false;

    // Stop cell in the MAC.
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).stop());

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop()
{
  return launch_async([this, i = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    for (; i != cells.size(); ++i) {
      if (cells[i] != nullptr and cells[i]->active) {
        cells[i]->active = false;

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
