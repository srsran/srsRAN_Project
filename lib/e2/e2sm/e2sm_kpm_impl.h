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
using namespace asn1::e2ap;

using namespace asn1::e2sm_kpm;
using namespace srsran;

class e2sm_kpm_impl : public e2sm_interface
{
public:
  // constructor takes logger as argument
  e2sm_kpm_impl(srslog::basic_logger&    logger_,
                e2sm_handler&            e2sm_packer_,
                e2_du_metrics_interface& du_metrics_interface_);

  srsran::byte_buffer handle_action(const srsran::byte_buffer& action_definition) override;

  srsran::byte_buffer get_indication_header(uint32_t action_id) override;

private:
  void process_action_definition(asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def);

  bool check_measurement_name(meas_type_c meas_type, const char* meas);

  void add_matching_condition_item(const char*            name,
                                   meas_cond_ueid_item_s& cond_ueid_item,
                                   matching_cond_item_s&  match_cond_item);

  // process action definition format 3 & uses fields to populate RIC indication message
  void handle_action_definition_format3(asn1::e2sm_kpm::e2_sm_kpm_action_definition_format3_s action_def);

  srslog::basic_logger&    logger;
  e2_sm_kpm_ind_hdr_s      ric_ind_header;
  e2_sm_kpm_ind_msg_s      ric_ind_message;
  e2sm_handler&            e2sm_packer;
  e2_du_metrics_interface& du_metrics_interface;
};
