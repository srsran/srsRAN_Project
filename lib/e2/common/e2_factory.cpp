/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_entity.h"
#include "e2_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_cu_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_action_cu_executor.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_action_du_executor.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_service_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_impl.h"
#include "srsran/e2/e2_agent_dependencies.h"
#include "srsran/e2/e2_cu_cp_factory.h"
#include "srsran/e2/e2_cu_up_factory.h"
#include "srsran/e2/e2_du_factory.h"

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
      std::make_unique<e2sm_rc_control_action_2_6_du_executor>(*du_configurator_);
  rc_control_service_style->add_e2sm_rc_control_action_executor(std::move(rc_control_action_executor));
  e2sm_rc_packer->add_e2sm_control_service(rc_control_service_style.get());
  e2sm_rc_iface->add_e2sm_control_service(std::move(rc_control_service_style));
  dependencies.e2sm_modules.emplace_back(e2sm_module{
      e2sm_rc_asn1_packer::ran_func_id, e2sm_rc_asn1_packer::oid, std::move(e2sm_rc_packer), std::move(e2sm_rc_iface)});

  auto e2_ext = std::make_unique<e2_entity>(std::move(dependencies));
  return e2_ext;
}

std::unique_ptr<e2_agent> srsran::create_e2_cu_up_agent(const e2ap_configuration& e2ap_cfg_,
                                                        e2_connection_client&     e2_client_,
                                                        e2_cu_metrics_interface*  e2_metrics_,
                                                        cu_configurator*          cu_configurator_,
                                                        timer_factory             timers_,
                                                        task_executor&            e2_exec_)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("E2-CU-UP");
  e2_agent_dependencies dependencies;
  dependencies.logger    = &logger;
  dependencies.cfg       = e2ap_cfg_;
  dependencies.e2_client = &e2_client_;
  dependencies.timers    = &timers_;
  dependencies.task_exec = &e2_exec_;

  // E2SM-KPM
  auto                                  e2sm_kpm_meas_provider = std::make_unique<e2sm_kpm_cu_up_meas_provider_impl>();
  std::unique_ptr<e2sm_kpm_asn1_packer> e2sm_kpm_packer =
      std::make_unique<e2sm_kpm_asn1_packer>(*e2sm_kpm_meas_provider);
  std::unique_ptr<e2sm_kpm_impl> e2sm_kpm_iface =
      std::make_unique<e2sm_kpm_impl>(logger, *e2sm_kpm_packer, *e2sm_kpm_meas_provider);
  e2_metrics_->connect_e2_cu_meas_provider(std::move(e2sm_kpm_meas_provider));
  dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_kpm_asn1_packer::ran_func_id,
                                                     e2sm_kpm_asn1_packer::oid,
                                                     std::move(e2sm_kpm_packer),
                                                     std::move(e2sm_kpm_iface)});

  // E2SM-RC
  auto e2sm_rc_packer = std::make_unique<e2sm_rc_asn1_packer>();
  auto e2sm_rc_iface  = std::make_unique<e2sm_rc_impl>(logger, *e2sm_rc_packer);
  // No Supported Control Styles.
  dependencies.e2sm_modules.emplace_back(e2sm_module{
      e2sm_rc_asn1_packer::ran_func_id, e2sm_rc_asn1_packer::oid, std::move(e2sm_rc_packer), std::move(e2sm_rc_iface)});

  auto e2_ext = std::make_unique<e2_entity>(std::move(dependencies));
  return e2_ext;
}

std::unique_ptr<e2_agent> srsran::create_e2_cu_cp_agent(const e2ap_configuration& e2ap_cfg_,
                                                        e2_connection_client&     e2_client_,
                                                        e2_cu_metrics_interface*  e2_metrics_,
                                                        cu_configurator*          cu_configurator_,
                                                        timer_factory             timers_,
                                                        task_executor&            e2_exec_)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("E2-CU-CP");
  e2_agent_dependencies dependencies;
  dependencies.logger    = &logger;
  dependencies.cfg       = e2ap_cfg_;
  dependencies.e2_client = &e2_client_;
  dependencies.timers    = &timers_;
  dependencies.task_exec = &e2_exec_;

  // E2SM-KPM
  auto                                  e2sm_kpm_meas_provider = std::make_unique<e2sm_kpm_cu_cp_meas_provider_impl>();
  std::unique_ptr<e2sm_kpm_asn1_packer> e2sm_kpm_packer =
      std::make_unique<e2sm_kpm_asn1_packer>(*e2sm_kpm_meas_provider);
  std::unique_ptr<e2sm_kpm_impl> e2sm_kpm_iface =
      std::make_unique<e2sm_kpm_impl>(logger, *e2sm_kpm_packer, *e2sm_kpm_meas_provider);
  e2_metrics_->connect_e2_cu_meas_provider(std::move(e2sm_kpm_meas_provider));
  dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_kpm_asn1_packer::ran_func_id,
                                                     e2sm_kpm_asn1_packer::oid,
                                                     std::move(e2sm_kpm_packer),
                                                     std::move(e2sm_kpm_iface)});

  // E2SM-RC
  auto                                  e2sm_rc_packer = std::make_unique<e2sm_rc_asn1_packer>();
  auto                                  e2sm_rc_iface  = std::make_unique<e2sm_rc_impl>(logger, *e2sm_rc_packer);
  int                                   control_service_style_id = 3;
  std::unique_ptr<e2sm_control_service> rc_control_service_style =
      std::make_unique<e2sm_rc_control_service>(control_service_style_id);
  std::unique_ptr<e2sm_control_action_executor> rc_control_action_executor =
      std::make_unique<e2sm_rc_control_action_3_1_cu_executor>(*cu_configurator_);
  rc_control_service_style->add_e2sm_rc_control_action_executor(std::move(rc_control_action_executor));
  e2sm_rc_packer->add_e2sm_control_service(rc_control_service_style.get());
  e2sm_rc_iface->add_e2sm_control_service(std::move(rc_control_service_style));
  // No Supported Control Styles.
  dependencies.e2sm_modules.emplace_back(e2sm_module{
      e2sm_rc_asn1_packer::ran_func_id, e2sm_rc_asn1_packer::oid, std::move(e2sm_rc_packer), std::move(e2sm_rc_iface)});

  auto e2_ext = std::make_unique<e2_entity>(std::move(dependencies));
  return e2_ext;
}
