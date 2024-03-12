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

#include "lib/e1ap/common/e1ap_asn1_converters.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_converters.h"
#include "lib/ngap/ngap_asn1_converters.h"
#include "srsran/ran/cause/e1ap_cause_converters.h"
#include "srsran/ran/cause/f1ap_cause_converters.h"
#include "srsran/ran/cause/ngap_cause_converters.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class asn1_cause_conversion_test : public ::testing::Test
{
public:
  ~asn1_cause_conversion_test() override
  {
    // flush logger after each test
    srslog::flush();
  }

protected:
  asn1_cause_conversion_test()
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    srslog::init();
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

// test conversion from ngap asn1
TEST_F(asn1_cause_conversion_test, when_ngap_cause_received_then_conversion_to_common_cause_successful)
{
  asn1::ngap::cause_c asn1_cause;
  ngap_cause_t        ngap_cause;

  asn1_cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::unspecified;
  ngap_cause                     = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_radio_network_t>(ngap_cause), ngap_cause_radio_network_t::unspecified);

  asn1_cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::cell_not_available;
  ngap_cause                     = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_radio_network_t>(ngap_cause), ngap_cause_radio_network_t::cell_not_available);

  asn1_cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::up_confidentiality_protection_not_possible;
  ngap_cause                     = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_radio_network_t>(ngap_cause),
            ngap_cause_radio_network_t::up_confidentiality_protection_not_possible);

  asn1_cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::ho_target_not_allowed;
  ngap_cause                     = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_radio_network_t>(ngap_cause), ngap_cause_radio_network_t::ho_target_not_allowed);

  asn1_cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::user_inactivity;
  ngap_cause                     = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_radio_network_t>(ngap_cause), ngap_cause_radio_network_t::user_inactivity);

  asn1_cause.set_transport() = asn1::ngap::cause_transport_opts::unspecified;
  ngap_cause                 = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_transport_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_transport_t>(ngap_cause), ngap_cause_transport_t::unspecified);

  asn1_cause.set_nas() = asn1::ngap::cause_nas_opts::deregister;
  ngap_cause           = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_nas_t>(ngap_cause));
  ASSERT_EQ(variant_get<cause_nas_t>(ngap_cause), cause_nas_t::deregister);

  asn1_cause.set_protocol() = asn1::ngap::cause_protocol_opts::msg_not_compatible_with_receiver_state;
  ngap_cause                = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_protocol_t>(ngap_cause));
  ASSERT_EQ(variant_get<cause_protocol_t>(ngap_cause), cause_protocol_t::msg_not_compatible_with_receiver_state);

  asn1_cause.set_misc() = asn1::ngap::cause_misc_opts::unspecified;
  ngap_cause            = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_misc_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_misc_t>(ngap_cause), ngap_cause_misc_t::unspecified);

  asn1_cause.set_misc() = asn1::ngap::cause_misc_opts::unknown_plmn_or_sn_pn;
  ngap_cause            = asn1_to_cause(asn1_cause);
  ASSERT_TRUE(variant_holds_alternative<ngap_cause_misc_t>(ngap_cause));
  ASSERT_EQ(variant_get<ngap_cause_misc_t>(ngap_cause), ngap_cause_misc_t::unknown_plmn_or_sn_pn);
}

// test conversion to ngap asn1
TEST_F(asn1_cause_conversion_test, when_common_cause_received_then_conversion_to_ngap_cause_successful)
{
  ngap_cause_t        cause;
  asn1::ngap::cause_c ngap_cause;

  cause      = ngap_cause_radio_network_t::unspecified;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(ngap_cause.radio_network().value, asn1::ngap::cause_radio_network_opts::unspecified);

  cause      = ngap_cause_radio_network_t::unknown_local_ue_ngap_id;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(ngap_cause.radio_network().value, asn1::ngap::cause_radio_network_opts::unknown_local_ue_ngap_id);

  cause      = ngap_cause_radio_network_t::ue_in_rrc_inactive_state_not_reachable;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(ngap_cause.radio_network().value,
            asn1::ngap::cause_radio_network_opts::ue_in_rrc_inactive_state_not_reachable);

  cause = srsran::e1ap_to_ngap_cause(e1ap_cause_t{e1ap_cause_radio_network_t::encryption_algorithms_not_supported});
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(ngap_cause.radio_network().value,
            asn1::ngap::cause_radio_network_opts::encryption_and_or_integrity_protection_algorithms_not_supported);

  cause      = f1ap_to_ngap_cause(f1ap_cause_radio_network_t::rl_fail_rlc);
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(ngap_cause.radio_network().value, asn1::ngap::cause_radio_network_opts::radio_conn_with_ue_lost);

  cause      = ngap_cause_transport_t::unspecified;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::transport);
  ASSERT_EQ(ngap_cause.transport().value, asn1::ngap::cause_transport_opts::unspecified);

  cause      = cause_nas_t::deregister;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::nas);
  ASSERT_EQ(ngap_cause.nas().value, asn1::ngap::cause_nas_opts::deregister);

  cause      = cause_protocol_t::msg_not_compatible_with_receiver_state;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::protocol);
  ASSERT_EQ(ngap_cause.protocol().value, asn1::ngap::cause_protocol_opts::msg_not_compatible_with_receiver_state);

  cause      = ngap_cause_misc_t::unspecified;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::misc);
  ASSERT_EQ(ngap_cause.misc().value, asn1::ngap::cause_misc_opts::unspecified);

  cause      = ngap_cause_misc_t::unknown_plmn_or_sn_pn;
  ngap_cause = cause_to_asn1(cause);
  ASSERT_EQ(ngap_cause.type(), asn1::ngap::cause_c::types::misc);
  ASSERT_EQ(ngap_cause.misc().value, asn1::ngap::cause_misc_opts::unknown_plmn_or_sn_pn);
}

// test conversion from f1ap asn1
TEST_F(asn1_cause_conversion_test, when_f1ap_cause_received_then_conversion_to_common_cause_successful)
{
  asn1::f1ap::cause_c f1ap_cause;
  f1ap_cause_t        cause;

  f1ap_cause.set_radio_network() = asn1::f1ap::cause_radio_network_opts::unspecified;
  cause                          = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_radio_network_t>(cause), f1ap_cause_radio_network_t::unspecified);

  f1ap_cause.set_radio_network() = asn1::f1ap::cause_radio_network_opts::cell_not_available;
  cause                          = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_radio_network_t>(cause), f1ap_cause_radio_network_t::cell_not_available);

  f1ap_cause.set_radio_network() = asn1::f1ap::cause_radio_network_opts::normal_release;
  cause                          = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_radio_network_t>(cause), f1ap_cause_radio_network_t::normal_release);

  f1ap_cause.set_radio_network() = asn1::f1ap::cause_radio_network_opts::gnb_cu_cell_capacity_exceeded;
  cause                          = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_radio_network_t>(cause), f1ap_cause_radio_network_t::gnb_cu_cell_capacity_exceeded);

  f1ap_cause.set_radio_network() = asn1::f1ap::cause_radio_network_opts::rl_fail_others;
  cause                          = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_radio_network_t>(cause), f1ap_cause_radio_network_t::rl_fail_others);

  f1ap_cause.set_transport() = asn1::f1ap::cause_transport_opts::unspecified;
  cause                      = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_transport_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_transport_t>(cause), f1ap_cause_transport_t::unspecified);

  f1ap_cause.set_transport() = asn1::f1ap::cause_transport_opts::unknown_tnl_address_for_iab;
  cause                      = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_transport_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_transport_t>(cause), f1ap_cause_transport_t::unknown_tnl_address_for_iab);

  f1ap_cause.set_transport() = asn1::f1ap::cause_transport_opts::unknown_up_tnl_info_for_iab;
  cause                      = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<f1ap_cause_transport_t>(cause));
  ASSERT_EQ(variant_get<f1ap_cause_transport_t>(cause), f1ap_cause_transport_t::unknown_up_tnl_info_for_iab);

  f1ap_cause.set_protocol() = asn1::f1ap::cause_protocol_opts::semantic_error;
  cause                     = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_protocol_t>(cause));
  ASSERT_EQ(variant_get<cause_protocol_t>(cause), cause_protocol_t::semantic_error);

  f1ap_cause.set_misc() = asn1::f1ap::cause_misc_opts::hardware_fail;
  cause                 = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_misc_t>(cause));
  ASSERT_EQ(variant_get<cause_misc_t>(cause), cause_misc_t::hardware_fail);

  f1ap_cause.set_misc() = asn1::f1ap::cause_misc_opts::unspecified;
  cause                 = asn1_to_cause(f1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_misc_t>(cause));
  ASSERT_EQ(variant_get<cause_misc_t>(cause), cause_misc_t::unspecified);
}

// test conversion to f1ap asn1
TEST_F(asn1_cause_conversion_test, when_common_cause_received_then_conversion_to_f1ap_cause_successful)
{
  f1ap_cause_t        cause;
  asn1::f1ap::cause_c f1ap_cause;

  cause      = f1ap_cause_radio_network_t::unspecified;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::unspecified);

  cause      = f1ap_cause_radio_network_t::cell_not_available;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::cell_not_available);

  cause      = f1ap_cause_radio_network_t::normal_release;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::normal_release);

  cause      = f1ap_cause_radio_network_t::gnb_cu_cell_capacity_exceeded;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::gnb_cu_cell_capacity_exceeded);

  cause      = f1ap_cause_radio_network_t::rl_fail_others;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::rl_fail_others);

  cause      = f1ap_cause_transport_t::unspecified;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::transport);
  ASSERT_EQ(f1ap_cause.transport().value, asn1::f1ap::cause_transport_opts::unspecified);

  cause      = f1ap_cause_transport_t::unknown_tnl_address_for_iab;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::transport);
  ASSERT_EQ(f1ap_cause.transport().value, asn1::f1ap::cause_transport_opts::unknown_tnl_address_for_iab);

  cause      = f1ap_cause_transport_t::unknown_up_tnl_info_for_iab;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::transport);
  ASSERT_EQ(f1ap_cause.transport().value, asn1::f1ap::cause_transport_opts::unknown_up_tnl_info_for_iab);

  cause      = srsran::ngap_to_f1ap_cause(ngap_cause_t{cause_nas_t::normal_release});
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(f1ap_cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::normal_release);

  cause      = cause_protocol_t::semantic_error;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::protocol);
  ASSERT_EQ(f1ap_cause.protocol().value, asn1::f1ap::cause_protocol_opts::semantic_error);

  cause      = cause_misc_t::hardware_fail;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::misc);
  ASSERT_EQ(f1ap_cause.misc().value, asn1::f1ap::cause_misc_opts::hardware_fail);

  cause      = cause_misc_t::unspecified;
  f1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(f1ap_cause.type(), asn1::f1ap::cause_c::types::misc);
  ASSERT_EQ(f1ap_cause.misc().value, asn1::f1ap::cause_misc_opts::unspecified);
}

// test conversion from e1ap asn1
TEST_F(asn1_cause_conversion_test, when_e1ap_cause_received_then_conversion_to_common_cause_successful)
{
  asn1::e1ap::cause_c e1ap_cause;
  e1ap_cause_t        cause;

  e1ap_cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::unspecified;
  cause                          = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_radio_network_t>(cause), e1ap_cause_radio_network_t::unspecified);

  e1ap_cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::unknown_or_inconsistent_pair_of_ue_e1ap_id;
  cause                          = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_radio_network_t>(cause),
            e1ap_cause_radio_network_t::unknown_or_inconsistent_pair_of_ue_e1ap_id);

  e1ap_cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::normal_release;
  cause                          = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_radio_network_t>(cause), e1ap_cause_radio_network_t::normal_release);

  e1ap_cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::pdcp_cfg_not_supported;
  cause                          = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_radio_network_t>(cause), e1ap_cause_radio_network_t::pdcp_cfg_not_supported);

  e1ap_cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::report_characteristic_empty;
  cause                          = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_radio_network_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_radio_network_t>(cause), e1ap_cause_radio_network_t::report_characteristic_empty);

  e1ap_cause.set_transport() = asn1::e1ap::cause_transport_opts::unspecified;
  cause                      = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_transport_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_transport_t>(cause), e1ap_cause_transport_t::unspecified);

  e1ap_cause.set_transport() = asn1::e1ap::cause_transport_opts::unknown_tnl_address_for_iab;
  cause                      = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<e1ap_cause_transport_t>(cause));
  ASSERT_EQ(variant_get<e1ap_cause_transport_t>(cause), e1ap_cause_transport_t::unknown_tnl_address_for_iab);

  e1ap_cause.set_protocol() = asn1::e1ap::cause_protocol_opts::semantic_error;
  cause                     = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_protocol_t>(cause));
  ASSERT_EQ(variant_get<cause_protocol_t>(cause), cause_protocol_t::semantic_error);

  e1ap_cause.set_misc() = asn1::e1ap::cause_misc_opts::hardware_fail;
  cause                 = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_misc_t>(cause));
  ASSERT_EQ(variant_get<cause_misc_t>(cause), cause_misc_t::hardware_fail);

  e1ap_cause.set_misc() = asn1::e1ap::cause_misc_opts::unspecified;
  cause                 = asn1_to_cause(e1ap_cause);
  ASSERT_TRUE(variant_holds_alternative<cause_misc_t>(cause));
  ASSERT_EQ(variant_get<cause_misc_t>(cause), cause_misc_t::unspecified);
}

// test conversion to e1ap asn1
TEST_F(asn1_cause_conversion_test, when_common_cause_received_then_conversion_to_e1ap_cause_successful)
{
  e1ap_cause_t        cause;
  asn1::e1ap::cause_c e1ap_cause;

  cause      = e1ap_cause_radio_network_t::unspecified;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value, asn1::e1ap::cause_radio_network_opts::unspecified);

  cause      = e1ap_cause_radio_network_t::unknown_or_inconsistent_pair_of_ue_e1ap_id;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value,
            asn1::e1ap::cause_radio_network_opts::unknown_or_inconsistent_pair_of_ue_e1ap_id);

  cause      = e1ap_cause_radio_network_t::normal_release;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value, asn1::e1ap::cause_radio_network_opts::normal_release);

  cause      = e1ap_cause_radio_network_t::pdcp_cfg_not_supported;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value, asn1::e1ap::cause_radio_network_opts::pdcp_cfg_not_supported);

  cause      = e1ap_cause_radio_network_t::report_characteristic_empty;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value, asn1::e1ap::cause_radio_network_opts::report_characteristic_empty);

  cause      = e1ap_cause_transport_t::unspecified;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::transport);
  ASSERT_EQ(e1ap_cause.transport().value, asn1::e1ap::cause_transport_opts::unspecified);

  cause      = e1ap_cause_transport_t::unknown_tnl_address_for_iab;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::transport);
  ASSERT_EQ(e1ap_cause.transport().value, asn1::e1ap::cause_transport_opts::unknown_tnl_address_for_iab);

  cause      = srsran::ngap_to_e1ap_cause(ngap_cause_t{cause_nas_t::normal_release});
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::ngap::cause_c::types::radio_network);
  ASSERT_EQ(e1ap_cause.radio_network().value, asn1::e1ap::cause_radio_network_opts::normal_release);

  cause      = cause_protocol_t::semantic_error;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::protocol);
  ASSERT_EQ(e1ap_cause.protocol().value, asn1::e1ap::cause_protocol_opts::semantic_error);

  cause      = cause_misc_t::hardware_fail;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::misc);
  ASSERT_EQ(e1ap_cause.misc().value, asn1::e1ap::cause_misc_opts::hardware_fail);

  cause      = cause_misc_t::unspecified;
  e1ap_cause = cause_to_asn1(cause);
  ASSERT_EQ(e1ap_cause.type(), asn1::e1ap::cause_c::types::misc);
  ASSERT_EQ(e1ap_cause.misc().value, asn1::e1ap::cause_misc_opts::unspecified);
}
