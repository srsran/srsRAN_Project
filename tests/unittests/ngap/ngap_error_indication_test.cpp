/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ngap_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_error_indication_test : public ngap_test
{
protected:
  void start_procedure(const ue_index_t ue_index)
  {
    ASSERT_EQ(ngap->get_nof_ues(), 0);
    create_ue(ue_index);

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request
    run_inital_context_setup(ue_index);
  }
};

// Note that this is currently a manual test without asserts, as we currently don't handle the error indication

/// Test handling of error indication message for inexisting ue
TEST_F(ngap_error_indication_test, when_error_indication_message_for_inexisting_ue_received_message_is_dropped)
{
  // Inject error indication message
  ngap_message error_indication_msg = generate_error_indication_message(uint_to_amf_ue_id(10), uint_to_ran_ue_id(0));
  ngap->handle_message(error_indication_msg);
}

/// Test handling of error indication message for existing ue
TEST_F(ngap_error_indication_test, when_error_indication_message_for_existing_ue_received_message_is_logged)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject error indication message
  ngap_message error_indication_msg = generate_error_indication_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(error_indication_msg);
}
