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

#include "srsran/e2/e2_du_factory.h"
#include "e2_entity.h"
#include "e2_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_action_du_executor.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_service_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_impl.h"
#include "srsran/e2/e2_agent_dependencies.h"

using namespace srsran;

std::unique_ptr<e2_agent> srsran::create_e2_du_agent(const e2ap_configuration&      e2ap_cfg_,
                                                     e2_connection_client&          e2_client_,
                                                     e2_du_metrics_interface*       e2_metrics_,
                                                     srs_du::f1ap_ue_id_translator* f1ap_ue_id_translator_,
                                                     srs_du::du_configurator*       du_configurator_,
                                                     timer_factory                  timers_,
                                                     task_executor&                 e2_exec_)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("E2-DU");
  e2_agent_dependencies dependencies;
  dependencies.logger    = &logger;
  dependencies.cfg       = e2ap_cfg_;
  dependencies.e2_client = &e2_client_;
  dependencies.timers    = &timers_;
  dependencies.task_exec = &e2_exec_;

  // E2SM-KPM
  auto e2sm_kpm_meas_provider = std::make_unique<e2sm_kpm_du_meas_provider_impl>(*f1ap_ue_id_translator_);
  std::unique_ptr<e2sm_kpm_asn1_packer> e2sm_kpm_packer =
      std::make_unique<e2sm_kpm_asn1_packer>(*e2sm_kpm_meas_provider);
  std::unique_ptr<e2sm_kpm_impl> e2sm_kpm_iface =
      std::make_unique<e2sm_kpm_impl>(logger, *e2sm_kpm_packer, *e2sm_kpm_meas_provider);
  e2_metrics_->connect_e2_du_meas_provider(std::move(e2sm_kpm_meas_provider));

  dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_kpm_asn1_packer::ran_func_id,
                                                     e2sm_kpm_asn1_packer::oid,
                                                     std::move(e2sm_kpm_packer),
                                                     std::move(e2sm_kpm_iface)});

  // E2SM-RC
  auto e2sm_rc_packer = std::make_unique<e2sm_rc_asn1_packer>();
  auto e2sm_rc_iface  = std::make_unique<e2sm_rc_impl>(logger, *e2sm_rc_packer);
  // Add Supported Control Styles.
  int                                   control_service_style_id = 2;
  std::unique_ptr<e2sm_control_service> rc_control_service_style =
      std::make_unique<e2sm_rc_control_service>(control_service_style_id);
  std::unique_ptr<e2sm_control_action_executor> rc_control_action_executor =
      std::make_unique<e2sm_rc_control_action_2_6_du_executor>(*du_configurator_, *f1ap_ue_id_translator_);
  rc_control_service_style->add_e2sm_rc_control_action_executor(std::move(rc_control_action_executor));
  e2sm_rc_packer->add_e2sm_control_service(rc_control_service_style.get());
  e2sm_rc_iface->add_e2sm_control_service(std::move(rc_control_service_style));
  dependencies.e2sm_modules.emplace_back(e2sm_module{
      e2sm_rc_asn1_packer::ran_func_id, e2sm_rc_asn1_packer::oid, std::move(e2sm_rc_packer), std::move(e2sm_rc_iface)});

  auto e2_ext = std::make_unique<e2_entity>(std::move(dependencies));
  return e2_ext;
}
