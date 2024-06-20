/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_configuration_manager_impl.h"

using namespace srsran::srs_cu_cp;

class du_configuration_manager_impl::du_configuration_handler_impl : public du_configuration_handler
{
public:
  du_configuration_handler_impl(du_configuration_manager_impl& parent_) : parent(parent_) {}
  ~du_configuration_handler_impl() override
  {
    if (ctxt != nullptr) {
      parent.rem_du(this->ctxt->id);
    }
  }

  bool handle_new_du_config(const du_setup_request& req) override
  {
    if (this->ctxt != nullptr) {
      parent.logger.error("du_id={}: Failed to create DU config. Cause: DU already configured", req.gnb_du_id);
      return false;
    }
    this->ctxt = parent.add_du_config(req);
    return this->ctxt != nullptr;
  }

  bool handle_du_config_update(const du_config_update_request& req) override
  {
    if (this->ctxt == nullptr) {
      parent.logger.error("du_id={}: Failed to update DU config. Cause: DU was not setup", req.gnb_du_id);
      return false;
    }

    // Reconfiguration.
    const du_configuration_context* result = parent.handle_du_config_update(*this->ctxt, req);
    if (result == nullptr) {
      return false;
    }
    this->ctxt = result;
    return true;
  }

private:
  du_configuration_manager_impl& parent;
};

du_configuration_manager_impl::du_configuration_manager_impl() : logger(srslog::fetch_basic_logger("CU-CP")) {}

std::unique_ptr<du_configuration_handler> du_configuration_manager_impl::create_du_handler()
{
  return std::make_unique<du_configuration_handler_impl>(*this);
}

const du_configuration_context* du_configuration_manager_impl::add_du_config(const du_setup_request& req)
{
  // Validate config.
  if (not validate_new_du_config(req)) {
    return nullptr;
  }

  // Create new DU config context.
  auto                      ret  = dus.emplace(req.gnb_du_id, du_configuration_context{});
  du_configuration_context& ctxt = ret.first->second;
  ctxt.id                        = req.gnb_du_id;
  ctxt.name                      = req.gnb_du_name;
  ctxt.rrc_version               = req.gnb_du_rrc_version;
  ctxt.gnb_du_served_cells_list  = req.gnb_du_served_cells_list;
  return &ctxt;
}

const du_configuration_context*
du_configuration_manager_impl::handle_du_config_update(const du_configuration_context& current_ctxt,
                                                       const du_config_update_request& req)
{
  if (current_ctxt.id != req.gnb_du_id) {
    logger.warning("du_id={}: Failed to update DU. Cause: DU ID mismatch", current_ctxt.id);
    return nullptr;
  }
  auto it = dus.find(current_ctxt.id);
  if (it == dus.end()) {
    logger.error("du_id={}: DU config update called for non-existent DU", current_ctxt.id);
    return nullptr;
  }

  // Validate config.
  if (not validate_du_config_update(req)) {
    return nullptr;
  }

  // Update DU config.
  du_configuration_context& du_context = it->second;
  // > Remove cells.
  for (const nr_cell_global_id_t& cgi : req.served_cells_to_rem) {
    auto cell_it =
        std::find_if(it->second.gnb_du_served_cells_list.begin(),
                     it->second.gnb_du_served_cells_list.end(),
                     [&cgi](const cu_cp_du_served_cells_item& item) { return item.served_cell_info.nr_cgi == cgi; });
    if (cell_it != it->second.gnb_du_served_cells_list.end()) {
      du_context.gnb_du_served_cells_list.erase(cell_it);
    } else {
      logger.warning(
          "du_id={}: Failed to remove cell nci={}. Cause: It was not previously set", current_ctxt.id, cgi.nci);
    }
  }
  // > Add new cells.
  for (const auto& cell_to_add : req.served_cells_to_add) {
    du_context.gnb_du_served_cells_list.push_back(cell_to_add);
  }
  return &it->second;
}

void du_configuration_manager_impl::rem_du(gnb_du_id_t du_id)
{
  auto it = dus.find(du_id);
  if (it == dus.end()) {
    logger.warning("du_id={}: Failed to remove DU. Cause: DU not found", du_id);
    return;
  }
  dus.erase(it);
}

bool du_configuration_manager_impl::validate_new_du_config(const du_setup_request& req) const
{
  for (const auto& [du_id, du_cfg] : dus) {
    if (du_cfg.id == req.gnb_du_id) {
      logger.warning("du_id={}: Failed to create DU config. Cause: Duplicate DU ID", req.gnb_du_id);
    }
    if (du_cfg.name == req.gnb_du_name) {
      logger.warning("du_id={}: Failed to create DU config. Cause: Duplicate DU name", req.gnb_du_name);
      return false;
    }
  }
  return true;
}

bool du_configuration_manager_impl::validate_du_config_update(const du_config_update_request& req) const
{
  // TODO
  return true;
}
