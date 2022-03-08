
#include "sched.h"

using namespace srsgnb;

sched::sched(sched_cfg_notifier& notifier) : mac_notifier(notifier), logger(srslog::fetch_basic_logger("MAC")) {}

bool sched::handle_cell_configuration_request(const cell_configuration_request_message& msg)
{
  // Verify cell configuration correctness
  auto result = is_cell_configuration_request_valid(msg);
  if (result.is_error()) {
    logger.error("Failed to create Cell. Cause: {}", result.error());
    return false;
  }

  if (msg.cell_index < cells.size() and cells[msg.cell_index] != nullptr) {
    logger.error("Failed to create Cell={}. Cause: cell_index is already under use.", msg.cell_index);
    return false;
  }
  if (msg.cell_index >= cells.size()) {
    cells.resize(msg.cell_index + 1);
  }

  cells[msg.cell_index] = std::make_unique<cell_sched>(msg);
  return true;
}

const dl_sched_result* sched::get_dl_sched(slot_point sl_tx, du_cell_index_t cell_index)
{
  // TODO
  return &cells[cell_index]->res_grid[sl_tx].dl_grants;
}

const ul_sched_result* sched::get_ul_sched(slot_point sl_tx, du_cell_index_t cell_index)
{
  // TODO
  return &cells[cell_index]->res_grid[sl_tx + 6].ul_grants;
}
