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

#include "ngap_test_helpers.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_error_indication_test : public ngap_test
{
protected:
  ue_index_t start_procedure()
  {
    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request
    run_initial_context_setup(ue_index);

    return ue_index;
  }

  bool was_error_indication_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

// Note that this is currently a manual test without asserts, as we currently don't handle the error indication

/// Test handling of error indication message for inexisting ue
TEST_F(ngap_error_indication_test,
       when_error_indication_message_for_inexisting_ue_received_message_is_dropped_and_error_indication_is_sent)
{
  // Inject error indication message
  ngap_message error_indication_msg = generate_error_indication_message(uint_to_amf_ue_id(10), uint_to_ran_ue_id(0));
  ngap->handle_message(error_indication_msg);

  // Check that Error Indication has been sent to AMF
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test handling of error indication message for existing ue
TEST_F(ngap_error_indication_test, when_error_indication_message_for_existing_ue_received_message_is_logged)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject error indication message
  ngap_message error_indication_msg = generate_error_indication_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(error_indication_msg);
}
