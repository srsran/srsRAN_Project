/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_test_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class RRC Reconfiguration tests preparation (to bring UE in RRC connected state)
class rrc_ue_reconfig : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    init();

    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    connect_amf();
    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Constructs full RRC Reconfig message with radioBearerConfig, masterCellGroup and NAS PDU
  cu_cp_rrc_reconfiguration_procedure_message fill_rrc_reconfiguration_procedure_message()
  {
    cu_cp_rrc_reconfiguration_procedure_message args;

    // add dummy radio bearer config
    cu_cp_radio_bearer_config rb_cfg;

    cu_cp_drb_to_add_mod drb_item;

    drb_item.drb_id = uint_to_drb_id(1);

    cu_cp_cn_assoc cn_assoc;
    cn_assoc.eps_bearer_id = 5;
    drb_item.cn_assoc      = cn_assoc;

    cu_cp_pdcp_config pdcp_config;
    pdcp_config.ciphering_disabled_present = true;

    cu_cp_drb drb;
    drb.pdcp_sn_size_dl = 18;
    drb.pdcp_sn_size_ul = 18;
    drb.discard_timer   = 100;
    pdcp_config.drb     = drb;

    pdcp_config.t_reordering = 0;
    drb_item.pdcp_cfg        = pdcp_config;

    cu_cp_security_config security_config;

    security_config.key_to_use = "secondary";

    cu_cp_security_algorithm_config security_algorithm_config;
    security_algorithm_config.ciphering_algorithm = "nea2";
    security_config.security_algorithm_cfg        = security_algorithm_config;

    rb_cfg.security_cfg = security_config;

    rb_cfg.drb_to_add_mod_list.push_back(drb_item);

    args.radio_bearer_cfg = rb_cfg;

    cu_cp_rrc_recfg_v1530_ies non_crit_ext;
    // add dummy NAS PDU
    non_crit_ext.ded_nas_msg_list.push_back(make_byte_buffer("aabbcc"));

    // add dummy master cell group config
    non_crit_ext.master_cell_group = make_byte_buffer("deadbeef");

    args.non_crit_ext = non_crit_ext;

    return args;
  }
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_reconfig, when_reconfig_complete_received_proc_successful)
{
  // Prepare args
  cu_cp_rrc_reconfiguration_procedure_message args = fill_rrc_reconfiguration_procedure_message();

  // Trigger Reconfig
  async_task<bool>         t = get_rrc_ue_reconfiguration_handler()->start_rrc_reconfiguration(args);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  check_rrc_reconfig_pdu();

  // Receive Reconfig complete
  receive_reconfig_complete();

  ASSERT_TRUE(t.ready());
}
