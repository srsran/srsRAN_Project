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

#include "f1ap_cu_test_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class f1ap_paging_test : public f1ap_cu_test
{
protected:
  bool was_conversion_successful() const
  {
    auto& paging_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.paging();

    // check ue id idx value
    if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (279089024671 % 1024)) {
      test_logger.error("UE ID idx value mismatch {} != {}",
                        paging_msg->ue_id_idx_value.idx_len10().to_number(),
                        (279089024671 % 1024));
      return false;
    }

    // check paging id
    if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != 279089024671) {
      test_logger.error("Paging ID mismatch {} != {}",
                        paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                        279089024671);
      return false;
    }

    // check paging drx
    if (!paging_msg->paging_drx_present) {
      return false;
    }
    if (paging_msg->paging_drx.to_number() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
      return false;
    }

    // check paging prio
    if (!paging_msg->paging_prio_present) {
      return false;
    }
    if (paging_msg->paging_prio.to_number() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
      return false;
    }

    // check paging cell list
    if (paging_msg->paging_cell_list.size() != 1) {
      return false;
    }
    auto& paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
    if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != 6576) {
      test_logger.error("NR CGI NCI mismatch {} != {}}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), 6576);
      return false;
    }
    if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
      return false;
    }

    // check paging origin
    if (!paging_msg->paging_origin_present) {
      return false;
    }
    if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
      test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
      return false;
    }

    return true;
  }

  bool was_paging_forwarded() const { return was_conversion_successful(); }
};

/// Test paging message handling
TEST_F(f1ap_paging_test, when_paging_message_received_message_is_forwarded)
{
  // Inject paging message
  cu_cp_paging_message paging_msg = generate_paging_message();
  f1ap->handle_paging(paging_msg);

  // check that paging message has been forwarded
  ASSERT_TRUE(was_paging_forwarded());
}
