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

#include "ngap_repository.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_context.h"
#include "srsran/ngap/ngap_factory.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_cp;

ngap_repository::ngap_repository(ngap_repository_config cfg_) : cfg(cfg_), logger(cfg.logger)
{
  for (uint16_t amf_idx = 0; amf_idx < cfg.cu_cp.ngaps.size(); amf_idx++) {
    auto* ngap_entity = add_ngap(uint_to_amf_index(amf_idx), cfg.cu_cp.ngaps.at(amf_idx));
    srsran_assert(ngap_entity != nullptr, "Failed to add NGAP for gateway");
  }
}

ngap_interface* ngap_repository::add_ngap(amf_index_t amf_index, const cu_cp_configuration::ngap_params& config)
{
  // Create NGAP object
  auto it = ngap_db.insert(std::make_pair(amf_index, ngap_context{}));
  srsran_assert(it.second, "Unable to insert NGAP in map");
  ngap_context& ngap_ctxt = it.first->second;
  ngap_ctxt.ngap_to_cu_cp_notifier.connect_cu_cp(cfg.cu_cp_notifier, cfg.paging_handler);

  ngap_configuration              ngap_cfg    = {cfg.cu_cp.node.gnb_id,
                                                 cfg.cu_cp.node.ran_node_name,
                                                 config.supported_tas,
                                                 cfg.cu_cp.ue.pdu_session_setup_timeout};
  std::unique_ptr<ngap_interface> ngap_entity = create_ngap(ngap_cfg,
                                                            ngap_ctxt.ngap_to_cu_cp_notifier,
                                                            cfg.start_ho_func,
                                                            *config.n2_gw,
                                                            *cfg.cu_cp.services.timers,
                                                            *cfg.cu_cp.services.cu_cp_executor);

  if (ngap_entity == nullptr) {
    logger.error("Failed to create NGAP");
    return nullptr;
  }

  ngap_ctxt.ngap = std::move(ngap_entity);

  return ngap_ctxt.ngap.get();
}

void ngap_repository::update_plmn_lookup(amf_index_t amf_index)
{
  auto ngap = ngap_db.find(amf_index);
  if (ngap == ngap_db.end()) {
    logger.error("NGAP not found for AMF index {}", amf_index);
    return;
  }

  std::vector<plmn_identity> supported_plmns = ngap->second.ngap->get_ngap_context().get_supported_plmns();
  for (auto plmn : supported_plmns) {
    plmn_to_amf_index.emplace(plmn, amf_index);
  }
}

ngap_interface* ngap_repository::find_ngap(const plmn_identity& plmn)
{
  if (plmn_to_amf_index.find(plmn) == plmn_to_amf_index.end()) {
    return nullptr;
  }

  return ngap_db.at(plmn_to_amf_index.at(plmn)).ngap.get();
}

std::map<amf_index_t, ngap_interface*> ngap_repository::get_ngaps()
{
  std::map<amf_index_t, ngap_interface*> ngaps;
  for (auto& amf : ngap_db) {
    ngaps.emplace(amf.first, amf.second.ngap.get());
  }
  return ngaps;
}

size_t ngap_repository::get_nof_ngap_ues()
{
  size_t nof_ues = 0;
  for (auto& du : ngap_db) {
    nof_ues += du.second.ngap->get_nof_ues();
  }
  return nof_ues;
}
