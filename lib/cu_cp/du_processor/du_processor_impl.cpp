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

#include "du_processor_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/ran/cause/f1ap_cause_converters.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/rrc/rrc_du_factory.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

static rrc_cfg_t create_rrc_config(const cu_cp_configuration& cu_cp_cfg)
{
  rrc_cfg_t rrc_cfg;
  rrc_cfg.gnb_id                         = cu_cp_cfg.node.gnb_id;
  rrc_cfg.force_reestablishment_fallback = cu_cp_cfg.rrc.force_reestablishment_fallback;
  rrc_cfg.rrc_procedure_timeout_ms       = cu_cp_cfg.rrc.rrc_procedure_timeout_ms;
  rrc_cfg.int_algo_pref_list             = cu_cp_cfg.security.int_algo_pref_list;
  rrc_cfg.enc_algo_pref_list             = cu_cp_cfg.security.enc_algo_pref_list;
  rrc_cfg.srb2_cfg                       = cu_cp_cfg.bearers.srb2_cfg;
  rrc_cfg.drb_config                     = cu_cp_cfg.bearers.drb_config;
  return rrc_cfg;
}

class du_processor_impl::f1ap_du_processor_adapter : public f1ap_du_processor_notifier
{
public:
  f1ap_du_processor_adapter(du_processor_impl& parent_, common_task_scheduler& common_task_sched_) :
    parent(parent_), common_task_sched(&common_task_sched_)
  {
  }

  du_setup_result on_new_du_setup_request(const du_setup_request& msg) override
  {
    return parent.handle_du_setup_request(msg);
  }

  ue_rrc_context_creation_outcome
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) override
  {
    return parent.handle_ue_rrc_context_creation_request(req);
  }

  void on_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& req) override
  {
    parent.handle_du_initiated_ue_context_release_request(req);
  }

  bool schedule_async_task(async_task<void> task) override
  {
    return common_task_sched->schedule_async_task(std::move(task));
  }

  async_task<void> on_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override
  {
    return parent.cu_cp_notifier.on_transaction_info_loss(ev);
  }

private:
  du_processor_impl&     parent;
  common_task_scheduler* common_task_sched = nullptr;
};

// du_processor_impl

du_processor_impl::du_processor_impl(du_processor_config_t               du_processor_config_,
                                     du_processor_cu_cp_notifier&        cu_cp_notifier_,
                                     f1ap_message_notifier&              f1ap_pdu_notifier_,
                                     rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                                     rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier,
                                     rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier,
                                     common_task_scheduler&              common_task_sched_,
                                     ue_manager&                         ue_mng_) :
  cfg(std::move(du_processor_config_)),
  cu_cp_notifier(cu_cp_notifier_),
  f1ap_pdu_notifier(f1ap_pdu_notifier_),
  rrc_ue_nas_pdu_notifier(rrc_ue_nas_pdu_notifier_),
  ue_mng(ue_mng_),
  f1ap_ev_notifier(std::make_unique<f1ap_du_processor_adapter>(*this, common_task_sched_))
{
  // create f1ap
  f1ap = create_f1ap(cfg.cu_cp_cfg.f1ap,
                     f1ap_pdu_notifier,
                     *f1ap_ev_notifier,
                     *cfg.cu_cp_cfg.services.timers,
                     *cfg.cu_cp_cfg.services.cu_cp_executor);
  f1ap_ue_context_notifier.connect_f1(f1ap->get_f1ap_ue_context_manager());

  // create RRC
  rrc_du_creation_message du_creation_req{
      create_rrc_config(cfg.cu_cp_cfg), rrc_ue_nas_pdu_notifier, rrc_ue_ngap_ctrl_notifier, rrc_du_cu_cp_notifier};
  rrc = create_rrc_du(du_creation_req);
  rrc_du_adapter.connect_rrc_du(rrc->get_rrc_du_cell_manager(), rrc->get_rrc_du_ue_repository());

  cu_cp_notifier.on_du_processor_created(cfg.du_index,
                                         f1ap->get_f1ap_ue_context_removal_handler(),
                                         f1ap->get_f1ap_statistics_handler(),
                                         rrc->get_rrc_ue_handler(),
                                         rrc->get_rrc_du_statistics_handler());
}

du_setup_result du_processor_impl::handle_du_setup_request(const du_setup_request& request)
{
  du_setup_result res;

  // Check if CU-CP is in a state to accept a new DU connection.
  if (not cfg.du_setup_notif->on_du_setup_request(cfg.du_index, request)) {
    res.result = du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                           "CU-CP is not in a state to accept a new DU connection"};
    return res;
  }

  // Validate and update DU configuration.
  auto cfg_res = cfg.du_cfg_hdlr->handle_new_du_config(request);
  if (not cfg_res.has_value()) {
    res.result = cfg_res.error();
    return res;
  }

  // Forward serving cell list to RRC DU
  // TODO: How to handle missing optional freq and timing in meas timing config?
  if (!rrc_du_adapter.on_new_served_cell_list(request.gnb_du_served_cells_list)) {
    res.result =
        du_setup_result::rejected{f1ap_cause_transport_t::unspecified, "Could not establish served cell list in RRC"};
    return res;
  }

  // Prepare DU response with accepted setup.
  auto& accepted              = res.result.emplace<du_setup_result::accepted>();
  accepted.gnb_cu_name        = cfg.cu_cp_cfg.node.ran_node_name;
  accepted.gnb_cu_rrc_version = cfg.cu_cp_cfg.rrc.rrc_version;

  // Accept all cells
  accepted.cells_to_be_activ_list.resize(request.gnb_du_served_cells_list.size());
  for (unsigned i = 0; i != accepted.cells_to_be_activ_list.size(); ++i) {
    accepted.cells_to_be_activ_list[i].nr_cgi = request.gnb_du_served_cells_list[i].served_cell_info.nr_cgi;
    accepted.cells_to_be_activ_list[i].nr_pci = request.gnb_du_served_cells_list[i].served_cell_info.nr_pci;
  }

  return res;
}

bool du_processor_impl::create_rrc_ue(cu_cp_ue&                              ue,
                                      rnti_t                                 c_rnti,
                                      const nr_cell_global_id_t&             cgi,
                                      byte_buffer                            du_to_cu_rrc_container,
                                      std::optional<rrc_ue_transfer_context> rrc_context)
{
  // Create RRC UE to F1AP adapter
  rrc_ue_f1ap_adapters.emplace(std::piecewise_construct,
                               std::forward_as_tuple(ue.get_ue_index()),
                               std::forward_as_tuple(f1ap->get_f1ap_rrc_message_handler(), ue.get_ue_index()));

  const du_cell_configuration& cell = *cfg.du_cfg_hdlr->get_context().find_cell(cgi);

  // Create new RRC UE entity
  rrc_ue_creation_message rrc_ue_create_msg{};
  rrc_ue_create_msg.ue_index              = ue.get_ue_index();
  rrc_ue_create_msg.c_rnti                = c_rnti;
  rrc_ue_create_msg.cell.cgi              = cgi;
  rrc_ue_create_msg.cell.tac              = cell.tac;
  rrc_ue_create_msg.cell.pci              = cell.pci;
  rrc_ue_create_msg.cell.bands            = cell.bands;
  rrc_ue_create_msg.f1ap_pdu_notifier     = &rrc_ue_f1ap_adapters.at(ue.get_ue_index());
  rrc_ue_create_msg.rrc_ue_cu_cp_notifier = &ue.get_rrc_ue_context_update_notifier();
  rrc_ue_create_msg.measurement_notifier  = &ue.get_rrc_ue_measurement_notifier();
  rrc_ue_create_msg.cu_cp_ue_notifier     = &ue.get_rrc_ue_cu_cp_ue_notifier();
  rrc_ue_create_msg.du_to_cu_container    = std::move(du_to_cu_rrc_container);
  rrc_ue_create_msg.rrc_context           = std::move(rrc_context);
  auto* rrc_ue                            = rrc_du_adapter.on_ue_creation_request(std::move(rrc_ue_create_msg));
  if (rrc_ue == nullptr) {
    logger.warning("Could not create RRC UE");
    return false;
  }

  // Create and connect DU Processor to RRC UE adapter
  rrc_ue_adapters[ue.get_ue_index()] = {};
  rrc_ue_adapters.at(ue.get_ue_index())
      .connect_rrc_ue(rrc_ue->get_rrc_ue_control_message_handler(), rrc_ue->get_rrc_ue_srb_handler());
  ue.set_rrc_ue_notifier(rrc_ue_adapters.at(ue.get_ue_index()));
  ue.set_rrc_ue_srb_notifier(rrc_ue_adapters.at(ue.get_ue_index()));

  // Notify CU-CP about the creation of the RRC UE
  cu_cp_notifier.on_rrc_ue_created(ue.get_ue_index(), *rrc_ue);

  return true;
}

ue_rrc_context_creation_outcome
du_processor_impl::handle_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req)
{
  srsran_assert(req.c_rnti != rnti_t::INVALID_RNTI, "ue={} c-rnti={}: Invalid C-RNTI", req.ue_index, req.c_rnti);

  // Check that creation message is valid
  const du_cell_configuration* pcell = cfg.du_cfg_hdlr->get_context().find_cell(req.cgi);
  if (pcell == nullptr) {
    logger.warning("ue={} c-rnti={}: Could not find cell with NCI={}", req.ue_index, req.c_rnti, req.cgi.nci);
    // Return the RRCReject container
    return make_unexpected(rrc_du_adapter.on_rrc_reject_required());
  }
  const pci_t pci = pcell->pci;

  ue_index_t ue_index = req.ue_index;
  cu_cp_ue*  ue       = nullptr;

  if (ue_index == ue_index_t::invalid) {
    // Add new CU-CP UE
    ue_index = ue_mng.add_ue(cfg.du_index, cfg.du_cfg_hdlr->get_context().id, pci, req.c_rnti, pcell->cell_index);
    if (ue_index == ue_index_t::invalid) {
      logger.warning("CU-CP UE creation failed");
      return make_unexpected(rrc_du_adapter.on_rrc_reject_required());
    }
    ue = ue_mng.find_ue(ue_index);

    /// NOTE: From this point on the UE exists in the UE manager and must be removed if any error occurs.

  } else {
    ue = ue_mng.set_ue_du_context(ue_index, cfg.du_cfg_hdlr->get_context().id, pci, req.c_rnti, pcell->cell_index);
    if (ue == nullptr) {
      logger.warning("ue={}: Could not create UE context", ue_index);
      // A UE with the same PCI and RNTI already exists, so we don't remove it and only reject the new UE.
      return make_unexpected(rrc_du_adapter.on_rrc_reject_required());
    }
  }

  // Create RRC UE. If the DU-to-CU-RRC-Container is empty, the UE will be rejected.
  if (not create_rrc_ue(*ue, req.c_rnti, req.cgi, req.du_to_cu_rrc_container.copy(), std::move(req.prev_context))) {
    logger.warning("ue={}: Could not create RRC UE object", ue_index);
    // Remove the UE from the UE manager
    ue_mng.remove_ue(ue_index);
    // Return the RRCReject container
    return make_unexpected(rrc_du_adapter.on_rrc_reject_required());
  }
  rrc_ue_interface* rrc_ue       = rrc->find_ue(ue_index);
  f1ap_rrc_ue_adapters[ue_index] = {};
  f1ap_rrc_ue_adapters.at(ue_index).connect_rrc_ue(rrc_ue->get_ul_ccch_pdu_handler(),
                                                   rrc_ue->get_ul_dcch_pdu_handler());

  // Signal back that the UE was successfully created.
  logger.info("ue={} c-rnti={}: UE created", ue->get_ue_index(), req.c_rnti);
  return ue_rrc_context_creation_response{ue_index, &f1ap_rrc_ue_adapters.at(ue_index)};
}

void du_processor_impl::handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request)
{
  srsran_assert(request.ue_index != ue_index_t::invalid, "Invalid UE index", request.ue_index);

  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Dropping DU initiated UE context release request. UE does not exist", request.ue_index);
    return;
  }

  logger.debug("ue={}: Handling DU initiated UE context release request", request.ue_index);

  // Schedule on UE task scheduler
  ue->get_task_sched().schedule_async_task(
      launch_async([this, request, ue](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT(cu_cp_notifier.on_ue_release_required(
            {request.ue_index, ue->get_up_resource_manager().get_pdu_sessions(), f1ap_to_ngap_cause(request.cause)}));
        CORO_RETURN();
      }));
}

bool du_processor_impl::has_cell(pci_t pci)
{
  return cfg.du_cfg_hdlr->get_context().find_cell(pci) != nullptr;
}

bool du_processor_impl::has_cell(nr_cell_global_id_t cgi)
{
  return cfg.du_cfg_hdlr->get_context().find_cell(cgi) != nullptr;
}

std::optional<nr_cell_global_id_t> du_processor_impl::get_cgi(pci_t pci)
{
  const du_cell_configuration* cell = cfg.du_cfg_hdlr->get_context().find_cell(pci);
  if (cell != nullptr) {
    return cell->cgi;
  }
  return std::nullopt;
}

byte_buffer du_processor_impl::get_packed_sib1(nr_cell_global_id_t cgi)
{
  const auto& cells = cfg.du_cfg_hdlr->get_context().served_cells;
  for (const auto& cell : cells) {
    if (cell.cgi == cgi) {
      return cell.sys_info.packed_sib1.copy();
    }
  }
  return byte_buffer{};
}

metrics_report::du_info du_processor_impl::handle_du_metrics_report_request() const
{
  metrics_report::du_info report;
  report.id = gnb_du_id_t::invalid;
  if (cfg.du_cfg_hdlr->has_context()) {
    report.id         = cfg.du_cfg_hdlr->get_context().id;
    const auto& cells = cfg.du_cfg_hdlr->get_context().served_cells;
    for (const auto& cell : cells) {
      report.cells.emplace_back();
      report.cells.back().cgi = cell.cgi;
      report.cells.back().pci = cell.pci;
    }
  }
  return report;
}
