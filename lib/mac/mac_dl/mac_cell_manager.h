
#ifndef SRSGNB_MAC_CELL_MANAGER_H
#define SRSGNB_MAC_CELL_MANAGER_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac_resource_manager.h"
#include "srsgnb/ran/du_types.h"

namespace srsgnb {

class mac_cell_manager
{
public:
  void add_cell(const mac_cell_configuration& cell_cfg)
  {
    srsran_assert(cell_cfg.cell_index < MAX_NOF_CELLS, "Invalid cell index");
    srsran_assert(not has_cell(cell_cfg.cell_index), "Cell already exists.");
    cells[cell_cfg.cell_index] = std::make_unique<cell_t>(cell_cfg);
  }

  void remove_cell(du_cell_index_t cell_index) { cells[cell_index] = nullptr; }

  bool has_cell(du_cell_index_t cell_index) const { return cell_index < cells.size() and cells[cell_index] != nullptr; }

  const mac_cell_configuration& get_config(du_cell_index_t cell_index) const
  {
    srsran_assert(has_cell(cell_index), "Inexistent cell index");
    return cells[cell_index]->cfg;
  }

  void start_cell(du_cell_index_t cell_index)
  {
    srsran_assert(has_cell(cell_index), "Inexistent cell index");
    cells[cell_index]->active = true;
  }

  void stop_cell(du_cell_index_t cell_index)
  {
    srsran_assert(has_cell(cell_index), "Inexistent cell index");
    cells[cell_index]->active = false;
  }

  bool is_cell_active(size_t i) const { return has_cell(i) and cells[i]->active; }

private:
  struct cell_t {
    explicit cell_t(const mac_cell_configuration& cfg_) : cfg(cfg_) {}

    const mac_cell_configuration cfg;
    bool                         active = false;
  };
  std::array<std::unique_ptr<cell_t>, MAX_NOF_CELLS> cells;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_MANAGER_H
