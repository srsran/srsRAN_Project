/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_test_messages.h"

using namespace srsran;
using namespace srs_cu_cp;

security::sec_as_key srsran::srs_cu_cp::make_sec_as_key(std::string hex_str)
{
  byte_buffer          key_buf = make_byte_buffer(hex_str);
  security::sec_as_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

security::sec_128_as_key srsran::srs_cu_cp::make_sec_128_as_key(std::string hex_str)
{
  byte_buffer              key_buf = make_byte_buffer(hex_str);
  security::sec_128_as_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

cu_cp_rrc_reconfiguration_procedure_request srsran::srs_cu_cp::generate_rrc_reconfiguration_procedure_request()
{
  cu_cp_rrc_reconfiguration_procedure_request args;

  // add dummy radio bearer config
  cu_cp_radio_bearer_config rb_cfg;

  cu_cp_drb_to_add_mod drb_item;

  drb_item.drb_id = uint_to_drb_id(1);

  cu_cp_cn_assoc cn_assoc;
  cn_assoc.eps_bearer_id = 5;
  drb_item.cn_assoc      = cn_assoc;

  pdcp_config_t pdcp_config;
  pdcp_config.ciphering_disabled_present = true;

  drb_t drb;
  drb.pdcp_sn_size_dl = pdcp_sn_size::size18bits;
  drb.pdcp_sn_size_ul = pdcp_sn_size::size18bits;
  drb.discard_timer   = pdcp_discard_timer::ms100;
  pdcp_config.drb     = drb;

  pdcp_config.t_reordering = pdcp_t_reordering::ms0;
  drb_item.pdcp_cfg        = pdcp_config;

  cu_cp_security_config security_config;

  security_config.key_to_use = "secondary";

  cu_cp_security_algorithm_config security_algorithm_config;
  security_algorithm_config.ciphering_algorithm = "nea2";
  security_config.security_algorithm_cfg        = security_algorithm_config;

  rb_cfg.security_cfg = security_config;

  rb_cfg.drb_to_add_mod_list.emplace(drb_item.drb_id, drb_item);

  args.radio_bearer_cfg = rb_cfg;

  cu_cp_rrc_recfg_v1530_ies non_crit_ext;
  // add dummy NAS PDU
  non_crit_ext.ded_nas_msg_list.push_back(make_byte_buffer("aabbcc"));

  // add dummy master cell group config
  non_crit_ext.master_cell_group = make_byte_buffer("deadbeef");

  args.non_crit_ext = non_crit_ext;

  return args;
}