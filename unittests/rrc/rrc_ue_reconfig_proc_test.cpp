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

  /// @brief Constructs full RRC Reconfig message with radioBearerConfig, masterCellGroup and NAS PDU
  rrc_reconfiguration_procedure_args fill_rrc_reconfiguration_procedure_args()
  {
    rrc_reconfiguration_procedure_args args;

    // add dummy radio bearer config
    auto& rb_cfg = args.radio_bearer_cfg.emplace();

    rb_cfg.drb_to_add_mod_list.resize(1);

    auto& drb_item                                = rb_cfg.drb_to_add_mod_list[0];
    drb_item.drb_id                               = 1;
    drb_item.cn_assoc_present                     = true;
    drb_item.cn_assoc.set_eps_bearer_id()         = 5;
    drb_item.pdcp_cfg_present                     = true;
    drb_item.pdcp_cfg.ciphering_disabled_present  = true;
    drb_item.pdcp_cfg.drb_present                 = true;
    drb_item.pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
    drb_item.pdcp_cfg.drb.pdcp_sn_size_dl         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::len18bits;
    drb_item.pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
    drb_item.pdcp_cfg.drb.pdcp_sn_size_ul         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::len18bits;
    drb_item.pdcp_cfg.drb.discard_timer_present   = true;
    drb_item.pdcp_cfg.drb.discard_timer           = asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_opts::ms100;
    drb_item.pdcp_cfg.drb.hdr_compress.set_not_used();
    drb_item.pdcp_cfg.t_reordering_present = true;
    drb_item.pdcp_cfg.t_reordering         = asn1::rrc_nr::pdcp_cfg_s::t_reordering_opts::ms0;

    rb_cfg.security_cfg_present                        = true;
    rb_cfg.security_cfg.key_to_use_present             = true;
    rb_cfg.security_cfg.key_to_use                     = asn1::rrc_nr::security_cfg_s::key_to_use_opts::secondary;
    rb_cfg.security_cfg.security_algorithm_cfg_present = true;
    rb_cfg.security_cfg.security_algorithm_cfg.ciphering_algorithm = asn1::rrc_nr::ciphering_algorithm_opts::nea2;

    // add dummy NAS PDU
    args.nas_pdu.emplace().from_string("aabbcc");

    // add dummy master cell group config
    args.master_cell_group_config.emplace().from_string("deadbeef");

    return args;
  }
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_reconfig, when_reconfig_complete_received_proc_successful)
{
  // Prepare args
  rrc_reconfiguration_procedure_args args = fill_rrc_reconfiguration_procedure_args();

  // Trigger Reconfig
  async_task<bool>         t = get_rrc_ue_reconfiguration_handler()->start_rrc_reconfiguration(args);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  check_rrc_reconfig_pdu();

  // Receive Reconfig complete
  receive_reconfig_complete();

  ASSERT_TRUE(t.ready());
}
