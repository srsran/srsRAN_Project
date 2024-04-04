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

#include "srsran/asn1/e2sm/e2sm_rc_ies.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace srsran;

class asn1_e2sm_test : public ::testing::Test
{
public:
  asn1_e2sm_test()
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    srslog::init();
  }
  ~asn1_e2sm_test() { srslog::flush(); }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(asn1_e2sm_test, unpack_ric_control_header)
{
  uint8_t ctrl_header[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05};
  // 000000000000f1100000000102000005
  byte_buffer buffer = byte_buffer::create(ctrl_header).value();

  asn1::cbit_ref                 bref{buffer};
  asn1::e2sm::e2sm_rc_ctrl_hdr_s pdu;
  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);
}

TEST_F(asn1_e2sm_test, unpack_ric_control_message)
{
  uint8_t ctrl_header[] = {0x00, 0x00, 0x01, 0x00, 0x00, 0x60, 0x00, 0x00, 0x40, 0x00, 0x03, 0x00, 0x02, 0x44,
                           0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x06, 0x2a, 0x00,
                           0x05, 0x30, 0x30, 0x31, 0x30, 0x31, 0x00, 0x07, 0x44, 0x00, 0x01, 0x00, 0x08, 0x2a,
                           0x00, 0x01, 0x31, 0x00, 0x09, 0x2a, 0x00, 0x01, 0x30, 0x00, 0x0a, 0x28, 0x80, 0x01,
                           0x05, 0x00, 0x0b, 0x28, 0x80, 0x01, 0x19, 0x00, 0x0c, 0x28, 0x80, 0x01, 0x64};
  // 00000100006000004000030002440000000460000040000100062a00053030313031000744000100082a00013100092a000130000a28800105000b28800119000c28800164
  byte_buffer buffer = byte_buffer::create(ctrl_header).value();

  asn1::cbit_ref                 bref{buffer};
  asn1::e2sm::e2sm_rc_ctrl_msg_s pdu;
  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);

  ASSERT_EQ(pdu.ric_ctrl_msg_formats.type().value,
            asn1::e2sm::e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format1);
  ASSERT_EQ(pdu.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list.size(), 1);
  auto& item = pdu.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list[0];
  ASSERT_EQ(item.ran_param_id, 1);
  ASSERT_EQ(item.ran_param_value_type.type().value, asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_list);
  ASSERT_EQ(item.ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param.size(), 1);
  auto& seq_params =
      item.ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param[0].seq_of_ran_params;
  ASSERT_EQ(seq_params.size(), 4);

  // First item.
  ASSERT_EQ(seq_params[0].ran_param_id, 3);
  ASSERT_EQ(seq_params[0].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_structure);
  auto& seq_params0 = seq_params[0].ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params;
  ASSERT_EQ(seq_params0.size(), 1);
  ASSERT_EQ(seq_params0[0].ran_param_id, 5);
  ASSERT_EQ(seq_params0[0].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_list);
  ASSERT_EQ(seq_params0[0].ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param.size(), 1);
  auto& ran_param0 = seq_params0[0].ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param[0];
  ASSERT_EQ(ran_param0.seq_of_ran_params.size(), 2);
  auto& seq_param00 = ran_param0.seq_of_ran_params[0];
  ASSERT_EQ(seq_param00.ran_param_id, 7);
  ASSERT_EQ(seq_param00.ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  ASSERT_TRUE(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present);
  ASSERT_EQ(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.type().value,
            asn1::e2sm::ran_param_value_c::types_opts::value_oct_s);
  ASSERT_EQ(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_string(),
            "3030313031");
  auto& seq_param01 = ran_param0.seq_of_ran_params[1];
  ASSERT_EQ(seq_param01.ran_param_id, 8);
  ASSERT_EQ(seq_param01.ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_structure);

  // Second item.
  ASSERT_EQ(seq_params[1].ran_param_id, 11);
  ASSERT_EQ(seq_params[1].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  // Third item.
  ASSERT_EQ(seq_params[2].ran_param_id, 12);
  ASSERT_EQ(seq_params[2].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  // Fourth item.
  ASSERT_EQ(seq_params[3].ran_param_id, 13);
  ASSERT_EQ(seq_params[3].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);
}
