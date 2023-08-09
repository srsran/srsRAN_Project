/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_kpm_report_service_base : public e2sm_report_service
{
public:
  e2sm_kpm_report_service_base() = delete;
  e2sm_kpm_report_service_base(asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def_,
                               e2_du_metrics_interface&                      du_metrics_interface_);
  virtual ~e2sm_kpm_report_service_base() = default;

  // e2sm_report_service interface
  virtual bool        collect_measurements() override = 0;
  srsran::byte_buffer get_indication_message() override;
  srsran::byte_buffer get_indication_header() override;

protected:
  // helper functions
  bool check_measurement_name(asn1::e2sm_kpm::meas_type_c meas_type, const char* meas);

  srslog::basic_logger&                         logger;
  asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def_generic;
  uint32_t                                      granul_period          = 0;
  bool                                          cell_global_id_present = false;
  asn1::e2sm_kpm::cgi_c                         cell_global_id         = {};

  e2_du_metrics_interface&            du_metrics_interface;
  asn1::e2sm_kpm::e2_sm_kpm_ind_hdr_s ric_ind_header_generic;
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s ric_ind_message_generic;
};

class e2sm_kpm_report_service_style1 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style1(asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def_,
                                 e2_du_metrics_interface&                      du_metrics_interface_);
  virtual ~e2sm_kpm_report_service_style1() = default;

  bool collect_measurements() override;

private:
  asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s& action_def;
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_format1_s&           ric_ind_message;
};

class e2sm_kpm_report_service_style3 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style3(asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def_,
                                 e2_du_metrics_interface&                      du_metrics_interface_);
  virtual ~e2sm_kpm_report_service_style3() = default;

  bool collect_measurements() override;

private:
  void add_matching_condition_item(const char*                            name,
                                   asn1::e2sm_kpm::meas_cond_ueid_item_s& cond_ueid_item,
                                   asn1::e2sm_kpm::matching_cond_item_s&  match_cond_item);

  asn1::e2sm_kpm::e2_sm_kpm_action_definition_format3_s& action_def;
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_format2_s&           ric_ind_message;
};

} // namespace srsran
