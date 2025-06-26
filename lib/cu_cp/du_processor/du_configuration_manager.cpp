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

#include "du_configuration_manager.h"
#include "srsran/ran/plmn_identity.h"

using namespace srsran;
using namespace srs_cu_cp;

static error_type<du_setup_result::rejected> validate_cell_config(const cu_cp_du_served_cells_item& served_cell)
{
  if (not served_cell.served_cell_info.five_gs_tac.has_value()) {
    return make_unexpected(du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                     fmt::format("Missing TAC for cell")});
  }

  if (not served_cell.gnb_du_sys_info.has_value()) {
    return make_unexpected(du_setup_result::rejected{cause_protocol_t::semantic_error,
                                                     fmt::format("Missing system information for cell")});
  }

  return {};
}

class du_configuration_manager::du_configuration_handler_impl : public du_configuration_handler
{
public:
  du_configuration_handler_impl(du_configuration_manager& parent_) : parent(parent_) {}
  ~du_configuration_handler_impl() override
  {
    if (ctxt != nullptr) {
      parent.rem_du(this->ctxt->id);
    }
  }

  validation_result handle_new_du_config(const du_setup_request& req) override
  {
    if (this->ctxt != nullptr) {
      return make_unexpected(
          du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "DU already configured"});
    }
    auto ret = parent.add_du_config(req);
    if (ret.has_value()) {
      this->ctxt = ret.value();
      return {};
    }
    return make_unexpected(ret.error());
  }

  validation_result handle_du_config_update(const du_config_update_request& req) override
  {
    if (this->ctxt == nullptr) {
      return make_unexpected(du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                       "DU with same gNB-DU-Id was not setup"});
    }

    // Reconfiguration.
    auto ret = parent.handle_du_config_update(*this->ctxt, req);
    if (not ret.has_value()) {
      return make_unexpected(ret.error());
    }
    this->ctxt = ret.value();
    return {};
  }

  void handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req) override
  {
    if (this->ctxt == nullptr) {
      srslog::fetch_basic_logger("CU-CP").debug(
          "Can't handle gNB CU Configuration Update. Cause: DU configuration context not found.");
      return;
    }
    parent.handle_gnb_cu_configuration_update(req, this->ctxt->id);
  }

private:
  du_configuration_manager& parent;
};

du_configuration_manager::du_configuration_manager(const gnb_id_t& gnb_id_, const std::vector<plmn_identity>& plmns_) :
  gnb_id(gnb_id_), plmns(plmns_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<du_configuration_handler> du_configuration_manager::create_du_handler()
{
  return std::make_unique<du_configuration_handler_impl>(*this);
}

static du_cell_configuration create_du_cell_config(du_cell_index_t                   cell_idx,
                                                   const cu_cp_du_served_cells_item& f1ap_cell_cfg)
{
  const auto&           cell_req = f1ap_cell_cfg.served_cell_info;
  du_cell_configuration cell;
  cell.cell_index        = cell_idx;
  cell.cgi               = cell_req.nr_cgi;
  cell.tac               = cell_req.five_gs_tac.value();
  cell.pci               = cell_req.nr_pci;
  cell.served_plmns      = cell_req.served_plmns;
  cell.deactivated_plmns = {};
  // Add band information.
  if (cell_req.nr_mode_info.fdd.has_value()) {
    for (const auto& band : cell_req.nr_mode_info.fdd.value().dl_nr_freq_info.freq_band_list_nr) {
      cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
    }
  } else if (cell_req.nr_mode_info.tdd.has_value()) {
    for (const auto& band : cell_req.nr_mode_info.tdd.value().nr_freq_info.freq_band_list_nr) {
      cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
    }
  }
  // Add packed MIB and SIB1
  cell.sys_info.packed_mib  = f1ap_cell_cfg.gnb_du_sys_info->mib_msg.copy();
  cell.sys_info.packed_sib1 = f1ap_cell_cfg.gnb_du_sys_info->sib1_msg.copy();
  return cell;
}

expected<const du_configuration_context*, du_setup_result::rejected>
du_configuration_manager::add_du_config(const du_setup_request& req)
{
  // Validate config.
  auto result = validate_new_du_config(req);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  // Create new DU config context.
  auto                      ret  = dus.emplace(req.gnb_du_id, du_configuration_context{});
  du_configuration_context& ctxt = ret.first->second;
  ctxt.id                        = req.gnb_du_id;
  ctxt.name                      = req.gnb_du_name;
  ctxt.rrc_version               = req.gnb_du_rrc_version;
  ctxt.served_cells.resize(req.gnb_du_served_cells_list.size());
  for (unsigned i = 0; i != ctxt.served_cells.size(); ++i) {
    ctxt.served_cells[i] = create_du_cell_config(uint_to_du_cell_index(i), req.gnb_du_served_cells_list[i]);
  }
  return &ctxt;
}

expected<const du_configuration_context*, du_setup_result::rejected>
du_configuration_manager::handle_du_config_update(const du_configuration_context& current_ctxt,
                                                  const du_config_update_request& req)
{
  if (current_ctxt.id != req.gnb_du_id) {
    logger.warning("du_id={}: Failed to update DU. Cause: DU ID mismatch", fmt::underlying(current_ctxt.id));
    return nullptr;
  }
  auto it = dus.find(current_ctxt.id);
  if (it == dus.end()) {
    logger.error("du_id={}: DU config update called for non-existent DU", fmt::underlying(current_ctxt.id));
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
    auto cell_it = std::find_if(it->second.served_cells.begin(),
                                it->second.served_cells.end(),
                                [&cgi](const du_cell_configuration& item) { return item.cgi == cgi; });
    if (cell_it != it->second.served_cells.end()) {
      du_context.served_cells.erase(cell_it);
    } else {
      logger.warning("du_id={}: Failed to remove cell nci={}. Cause: It was not previously set",
                     fmt::underlying(current_ctxt.id),
                     cgi.nci);
    }
  }
  // > Add new cells.
  for (const auto& cell_to_add : req.served_cells_to_add) {
    // Allocate cell index.
    du_cell_index_t cell_idx = du_cell_index_t::invalid;
    for (unsigned i = 0; i != MAX_NOF_DU_CELLS; ++i) {
      if (std::none_of(
              du_context.served_cells.begin(), du_context.served_cells.end(), [i](const du_cell_configuration& item) {
                return item.cell_index == uint_to_du_cell_index(i);
              })) {
        cell_idx = uint_to_du_cell_index(i);
        break;
      }
    }
    // Note: Existence of a free cell index should be guaranteed during validation.
    srsran_assert(cell_idx != du_cell_index_t::invalid, "Failed to allocate cell index");

    du_context.served_cells.push_back(create_du_cell_config(cell_idx, cell_to_add));
  }
  return &it->second;
}

void du_configuration_manager::handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req,
                                                                  gnb_du_id_t                             du_id)
{
  if (dus.find(du_id) == dus.end()) {
    logger.warning("du_id={}: Failed to update DU. Cause: DU context not found", fmt::underlying(du_id));
    return;
  }
  auto& du_ctxt = dus.at(du_id);

  for (const auto& cell : req.cells_to_be_activated_list) {
    // Find cell in the served cells.
    auto it = std::find_if(
        du_ctxt.served_cells.begin(), du_ctxt.served_cells.end(), [&cell](const auto& c) { return c.cgi == cell.cgi; });

    // If the cell is already in the served cells, update the PLMNs.
    if (it != du_ctxt.served_cells.end()) {
      // If a PLMN from the update is not in the currently served PLMN list, add it to the served PLMNs.
      for (const auto& updated_plmn : cell.available_plmn_list) {
        if (std::find(it->served_plmns.begin(), it->served_plmns.end(), updated_plmn) == it->served_plmns.end()) {
          it->served_plmns.push_back(updated_plmn);
        }
        // If it is in the deactivated PLMN list, remove it from there.
        if (std::find(it->deactivated_plmns.begin(), it->deactivated_plmns.end(), updated_plmn) ==
            it->served_plmns.end()) {
          it->deactivated_plmns.erase(
              std::remove(it->deactivated_plmns.begin(), it->deactivated_plmns.end(), updated_plmn),
              it->deactivated_plmns.end());
        }
      }

      // If a currently served PLMN is not in the update, add the PLMN to the deactivated.
      for (const auto& served_plmn : it->served_plmns) {
        if (std::find(cell.available_plmn_list.begin(), cell.available_plmn_list.end(), served_plmn) ==
            cell.available_plmn_list.end()) {
          it->deactivated_plmns.push_back(served_plmn);
        }
      }

      // Remove deactivated PLMNs from served PLMNs.
      for (const auto& deactivated_plmn : it->deactivated_plmns) {
        it->served_plmns.erase(std::remove(it->served_plmns.begin(), it->served_plmns.end(), deactivated_plmn),
                               it->served_plmns.end());
      }
    }

    // If the cell is deactivated, move it to the served cells.
    auto deactivated_it = std::find_if(du_ctxt.deactivated_cells.begin(),
                                       du_ctxt.deactivated_cells.end(),
                                       [&cell](const auto& c) { return c.cgi == cell.cgi; });
    if (deactivated_it != du_ctxt.deactivated_cells.end()) {
      // Activate all PLMNs from the update.
      for (const auto& plmn_to_activate : cell.available_plmn_list) {
        // Add PLMN to served PLMNs.
        deactivated_it->served_plmns.push_back(plmn_to_activate);
        // Remove PLMN from deactivated PLMNs.
        deactivated_it->deactivated_plmns.erase(std::remove(deactivated_it->deactivated_plmns.begin(),
                                                            deactivated_it->deactivated_plmns.end(),
                                                            plmn_to_activate),
                                                deactivated_it->deactivated_plmns.end());
      }

      // Move cell to served cells.
      du_ctxt.served_cells.push_back(*deactivated_it);
      du_ctxt.deactivated_cells.erase(deactivated_it);
    }
  }

  for (const auto& cell : req.cells_to_be_deactivated_list) {
    // Find cell in the served cells.
    auto it = std::find_if(
        du_ctxt.served_cells.begin(), du_ctxt.served_cells.end(), [&cell](const auto& c) { return c.cgi == cell.cgi; });

    // If the cell in the served cells, deactivate it.
    if (it != du_ctxt.served_cells.end()) {
      // Move all served PLMNs to deactivated PLMNs.
      for (const auto& plmn : it->served_plmns) {
        it->deactivated_plmns.push_back(plmn);
      }
      // Remove deactivated PLMNs from served PLMNs.
      for (const auto& deactivated_plmn : it->deactivated_plmns) {
        it->served_plmns.erase(std::remove(it->served_plmns.begin(), it->served_plmns.end(), deactivated_plmn),
                               it->served_plmns.end());
      }

      // Move cell to deactivated cells.
      du_ctxt.deactivated_cells.push_back(*it);
      du_ctxt.served_cells.erase(it);
    }
  }
}

void du_configuration_manager::rem_du(gnb_du_id_t du_id)
{
  auto it = dus.find(du_id);
  if (it == dus.end()) {
    logger.warning("du={}: Failed to remove DU. Cause: DU not found", fmt::underlying(du_id));
    return;
  }
  dus.erase(it);
}

error_type<du_setup_result::rejected>
du_configuration_manager::validate_new_du_config(const du_setup_request& req) const
{
  if (req.gnb_du_served_cells_list.size() > MAX_NOF_DU_CELLS) {
    return make_unexpected(
        du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "Too many served cells"});
  }

  // Validate served cell configurations provided in the configuration request.
  for (const auto& served_cell : req.gnb_du_served_cells_list) {
    auto ret = validate_cell_config_request(served_cell);
    if (not ret.has_value()) {
      return ret;
    }

    if (std::find(plmns.begin(), plmns.end(), served_cell.served_cell_info.nr_cgi.plmn_id) == plmns.end()) {
      return make_unexpected(du_setup_result::rejected{f1ap_cause_radio_network_t::plmn_not_served_by_the_gnb_cu,
                                                       "Served Cell CGI PLMN is not supported by the CU-CP"});
    }

    if (std::none_of(
            served_cell.served_cell_info.served_plmns.begin(),
            served_cell.served_cell_info.served_plmns.end(),
            [this](const plmn_identity& plmn) { return std::find(plmns.begin(), plmns.end(), plmn) != plmns.end(); })) {
      return make_unexpected(du_setup_result::rejected{f1ap_cause_radio_network_t::plmn_not_served_by_the_gnb_cu,
                                                       "None of the served cell PLMNs is available in the CU-CP"});
    }
  }

  // Ensure the DU config does not collide with other DUs.
  for (const auto& [du_id, du_cfg] : dus) {
    if (du_cfg.id == req.gnb_du_id) {
      return make_unexpected(
          du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "Duplicate DU ID"});
    }
    for (const auto& cell : du_cfg.served_cells) {
      for (const auto& new_cell : req.gnb_du_served_cells_list) {
        if (cell.cgi == new_cell.served_cell_info.nr_cgi) {
          return make_unexpected(du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                           "Duplicate served cell CGI"});
        }
      }
    }
  }

  return {};
}

error_type<du_setup_result::rejected>
du_configuration_manager::validate_du_config_update(const du_config_update_request& req) const
{
  // TODO
  return {};
}

error_type<du_setup_result::rejected>
du_configuration_manager::validate_cell_config_request(const cu_cp_du_served_cells_item& cell_req) const
{
  auto ret = validate_cell_config(cell_req);
  if (not ret.has_value()) {
    return make_unexpected(ret.error());
  }

  // Ensure NCIs match the gNB-Id.
  gnb_id_t served_gnb_id = cell_req.served_cell_info.nr_cgi.nci.gnb_id(gnb_id.bit_length);
  if (served_gnb_id != gnb_id) {
    return make_unexpected(du_setup_result::rejected{
        cause_protocol_t::msg_not_compatible_with_receiver_state,
        fmt::format("NCI {:#x} of the served Cell does not match this gNB-Id ({:#x} != {:#x})",
                    cell_req.served_cell_info.nr_cgi.nci,
                    gnb_id.id,
                    served_gnb_id.id)});
  }

  return {};
}
