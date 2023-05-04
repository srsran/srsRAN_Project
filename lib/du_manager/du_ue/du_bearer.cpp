/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_bearer.h"
#include "../converters/rlc_config_helpers.h"
#include "srsran/adt/static_vector.h"
#include "srsran/du_manager/du_manager_params.h"

using namespace srsran;
using namespace srs_du;

void du_srb_connector::connect(du_ue_index_t                       ue_index,
                               srb_id_t                            srb_id,
                               f1c_bearer&                         f1_bearer,
                               rlc_entity&                         rlc_bearer,
                               mac_ue_control_information_handler& mac_ue_info_handler)
{
  // > Connect RLC Rx SDU -> F1-C Tx SDU handler.
  rlc_rx_sdu_notif.connect(f1_bearer);

  // > Connect RLC Tx Data Notifier -> F1-C handler
  rlc_tx_data_notif.connect(f1_bearer);

  // > Connect F1-C Rx SDU -> RLC Tx SDU.
  f1c_rx_sdu_notif.connect(*rlc_bearer.get_tx_upper_layer_data_interface());

  // > Connect RLC BSR update notifier -> MAC Control Info Handler.
  rlc_tx_buffer_state_notif.connect(ue_index, srb_id_to_lcid(srb_id), mac_ue_info_handler);

  // > Connect MAC Rx SDU notifier -> RLC Rx PDU.
  mac_rx_sdu_notifier.connect(*rlc_bearer.get_rx_lower_layer_interface());

  // > Connect MAC Tx SDU builder -> RLC Tx PDU builder.
  mac_tx_sdu_notifier.connect(*rlc_bearer.get_tx_lower_layer_interface());
}

void du_drb_connector::connect(du_ue_index_t                       ue_index,
                               drb_id_t                            drb_id,
                               lcid_t                              lcid,
                               f1u_bearer&                         f1_bearer,
                               rlc_entity&                         rlc_bearer,
                               mac_ue_control_information_handler& mac_ue_info_handler)
{
  // > Connect RLC Rx SDU -> F1-U Tx SDU handler.
  rlc_rx_sdu_notif.connect(f1_bearer.get_tx_sdu_handler());

  // > Connect RLC Tx Data Notifier -> F1-U handler
  rlc_tx_data_notif.connect(f1_bearer.get_tx_delivery_handler());

  // > Connect F1-U Rx SDU -> RLC Tx SDU.
  f1u_rx_sdu_notif.connect(*rlc_bearer.get_tx_upper_layer_data_interface());

  // > Connect RLC BSR update notifier -> MAC Control Info Handler.
  rlc_tx_buffer_state_notif.connect(ue_index, lcid, mac_ue_info_handler);

  // > Connect MAC Rx SDU notifier -> RLC Rx PDU.
  mac_rx_sdu_notifier.connect(*rlc_bearer.get_rx_lower_layer_interface());

  // > Connect MAC Tx SDU builder -> RLC Tx PDU builder.
  mac_tx_sdu_notifier.connect(*rlc_bearer.get_tx_lower_layer_interface());
}

void du_drb_connector::disconnect_rx()
{
  // Disconnect F1-U <-> RLC interface.
  rlc_rx_sdu_notif.disconnect();
  f1u_rx_sdu_notif.disconnect();

  // Disconnect MAC -> RLC interface.
  mac_rx_sdu_notifier.disconnect();
}

void du_ue_drb::disconnect_rx()
{
  connector.disconnect_rx();
}

std::unique_ptr<du_ue_drb> srsran::srs_du::create_drb(du_ue_index_t                       ue_index,
                                                      du_cell_index_t                     pcell_index,
                                                      drb_id_t                            drb_id,
                                                      lcid_t                              lcid,
                                                      const rlc_config&                   rlc_cfg,
                                                      span<const up_transport_layer_info> uluptnl_info_list,
                                                      const du_manager_params&            du_params)
{
  srsran_assert(not is_srb(lcid), "Invalid DRB LCID={}", lcid);
  srsran_assert(not uluptnl_info_list.empty(), "Invalid UP TNL Info list");

  // > Setup DL UP TNL info.
  // TODO: Handle more than one tnl. Set IP.
  // Note: We are computing the DL GTP-TEID as a concatenation of the UE index and DRB-id.
  std::array<up_transport_layer_info, 1> dluptnl_info_list = {up_transport_layer_info{
      transport_layer_address{"127.0.0.1"}, int_to_gtp_teid((ue_index << 5U) + (unsigned)drb_id)}};

  // > Create DRB instance.
  std::unique_ptr<du_ue_drb> drb = std::make_unique<du_ue_drb>();

  // > Setup DRB config
  drb->drb_id  = drb_id;
  drb->lcid    = lcid;
  drb->rlc_cfg = rlc_cfg;
  drb->uluptnl_info_list.assign(uluptnl_info_list.begin(), uluptnl_info_list.end());
  drb->dluptnl_info_list.assign(dluptnl_info_list.begin(), dluptnl_info_list.end());

  // > Create F1-U bearer.
  f1u_bearer* f1u_drb = du_params.f1u.f1u_gw.create_du_bearer(
      ue_index,
      drb->dluptnl_info_list[0].gtp_teid.value(),
      drb->uluptnl_info_list[0].gtp_teid.value(),
      drb->connector.f1u_rx_sdu_notif,
      timer_factory{du_params.services.timers, du_params.services.ue_execs.executor(ue_index)});
  if (f1u_drb == nullptr) {
    // Failed to connect F1-U bearer to CU-UP.
    return nullptr;
  }
  auto f1u_bearer_deleter = [f1u_gw  = &du_params.f1u.f1u_gw,
                             dl_teid = drb->dluptnl_info_list[0].gtp_teid.value()](f1u_bearer* p) {
    if (p != nullptr) {
      f1u_gw->remove_du_bearer(dl_teid);
    }
  };
  drb->drb_f1u = std::unique_ptr<f1u_bearer, std::function<void(f1u_bearer*)>>(f1u_drb, f1u_bearer_deleter);

  // > Create RLC DRB entity.
  drb->rlc_bearer =
      create_rlc_entity(make_rlc_entity_creation_message(ue_index, pcell_index, *drb, du_params.services));
  if (drb->rlc_bearer == nullptr) {
    // Failed to create RLC DRB entity.
    du_params.f1u.f1u_gw.remove_du_bearer(drb->dluptnl_info_list[0].gtp_teid.value());
    return nullptr;
  }

  // > Connect DRB F1-U with RLC, and RLC with MAC logical channel notifier.
  drb->connector.connect(
      ue_index, drb->drb_id, drb->lcid, *drb->drb_f1u, *drb->rlc_bearer, du_params.rlc.mac_ue_info_handler);

  return drb;
}

du_ue_srb& du_ue_bearer_manager::add_srb(srb_id_t srb_id, const rlc_config& rlc_cfg)
{
  srsran_assert(not srbs().contains(srb_id), "SRB-Id={} already exists", srb_id);
  srbs_.emplace(srb_id);
  srbs_[srb_id].srb_id  = srb_id;
  srbs_[srb_id].rlc_cfg = rlc_cfg;
  return srbs_[srb_id];
}

void du_ue_bearer_manager::add_drb(std::unique_ptr<du_ue_drb> drb)
{
  srsran_assert(drbs().count(drb->drb_id) == 0, "DRB-Id={} already exists", drb->drb_id);
  drbs_.emplace(drb->drb_id, std::move(drb));
}

std::unique_ptr<du_ue_drb> du_ue_bearer_manager::remove_drb(drb_id_t drb_id)
{
  srsran_assert(drbs().count(drb_id) > 0, "DRB-Id={} does not exist", drb_id);
  std::unique_ptr<du_ue_drb> drb = std::move(drbs_.at(drb_id));
  drbs_.erase(drb_id);
  return drb;
}

optional<lcid_t> du_ue_bearer_manager::allocate_lcid() const
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& drb : drbs()) {
    used_lcids.push_back(drb.second->lcid);
  }
  std::sort(used_lcids.begin(), used_lcids.end());
  if (used_lcids.empty() or used_lcids[0] > LCID_MIN_DRB) {
    return LCID_MIN_DRB;
  }
  auto it = std::adjacent_find(used_lcids.begin(), used_lcids.end(), [](lcid_t l, lcid_t r) { return l + 1 < r; });
  if (it == used_lcids.end()) {
    // no gaps found. Use the last value + 1.
    --it;
  }
  // beginning of the gap + 1.
  lcid_t lcid = uint_to_lcid(static_cast<unsigned>(*it) + 1U);
  if (lcid > LCID_MAX_DRB) {
    return {};
  }
  return lcid;
}
