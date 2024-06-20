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
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/rrc/rrc_config.h"

using namespace srsran;
using namespace srs_cu_cp;

static error_type<du_setup_result::rejected> validate_cell_config(const cu_cp_du_served_cells_item& served_cell)
{
  const auto& cell_info = served_cell.served_cell_info;

  if (not config_helpers::is_valid(cell_info.nr_cgi)) {
    return error_type<du_setup_result::rejected>{
        du_setup_result::rejected{cause_protocol_t::semantic_error, "Invalid NR CGI"}};
  }

  if (not served_cell.served_cell_info.five_gs_tac.has_value()) {
    return error_type<du_setup_result::rejected>{du_setup_result::rejected{
        cause_protocol_t::msg_not_compatible_with_receiver_state, fmt::format("Missing TAC for cell")}};
  }

  if (not served_cell.gnb_du_sys_info.has_value()) {
    return error_type<du_setup_result::rejected>{du_setup_result::rejected{
        cause_protocol_t::semantic_error, fmt::format("Missing system information for cell")}};
  }

  return {};
}

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

  validation_result handle_new_du_config(const du_setup_request& req) override
  {
    if (this->ctxt != nullptr) {
      return error_type<du_setup_result::rejected>{
          du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "DU already configured"}};
    }
    auto ret = parent.add_du_config(req);
    if (ret.has_value()) {
      this->ctxt = ret.value();
      return {};
    }
    return ret.error();
  }

  validation_result handle_du_config_update(const du_config_update_request& req) override
  {
    if (this->ctxt == nullptr) {
      return error_type<du_setup_result::rejected>{du_setup_result::rejected{
          cause_protocol_t::msg_not_compatible_with_receiver_state, "DU with same gNB-DU-Id was not setup"}};
    }

    // Reconfiguration.
    auto ret = parent.handle_du_config_update(*this->ctxt, req);
    if (ret.is_error()) {
      return ret.error();
    }
    this->ctxt = ret.value();
    return {};
  }

private:
  du_configuration_manager_impl& parent;
};

du_configuration_manager_impl::du_configuration_manager_impl(const rrc_cfg_t& rrc_cfg_) :
  rrc_cfg(rrc_cfg_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<du_configuration_handler> du_configuration_manager_impl::create_du_handler()
{
  return std::make_unique<du_configuration_handler_impl>(*this);
}

expected<const du_configuration_context*, du_setup_result::rejected>
du_configuration_manager_impl::add_du_config(const du_setup_request& req)
{
  // Validate config.
  auto result = validate_new_du_config(req);
  if (result.is_error()) {
    return result.error();
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

expected<const du_configuration_context*, du_setup_result::rejected>
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

error_type<du_setup_result::rejected>
du_configuration_manager_impl::validate_new_du_config(const du_setup_request& req) const
{
  // Ensure the DU config does not collide with other DUs.
  for (const auto& [du_id, du_cfg] : dus) {
    if (du_cfg.id == req.gnb_du_id) {
      return error_type<du_setup_result::rejected>{
          du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "Duplicate DU ID"}};
    }
  }

  // Validate served cell configurations.
  for (const auto& served_cell : req.gnb_du_served_cells_list) {
    auto ret = validate_cell_config_request(served_cell);
    if (not ret.has_value()) {
      return ret;
    }
  }

  return {};
}

error_type<du_setup_result::rejected>
du_configuration_manager_impl::validate_du_config_update(const du_config_update_request& req) const
{
  // TODO
  return {};
}

error_type<du_setup_result::rejected>
du_configuration_manager_impl::validate_cell_config_request(const cu_cp_du_served_cells_item& cell_req) const
{
  auto ret = validate_cell_config(cell_req);
  if (ret.is_error()) {
    return ret.error();
  }

  // Ensure NCIs match the gNB-Id.
  if (not rrc_cfg.gnb_id.contains_nci(cell_req.served_cell_info.nr_cgi.nci)) {
    return error_type<du_setup_result::rejected>{
        du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                  fmt::format("NCI {:#x} of the served Cell does not match gNB-Id {:#x}",
                                              cell_req.served_cell_info.nr_cgi.nci,
                                              rrc_cfg.gnb_id.id)}};
  }

  return {};
}
