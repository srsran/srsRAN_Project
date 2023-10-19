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

#include "../../ran/gnb_format.h"
#include "rrc_ue_helpers.h"
#include "rrc_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_impl::send_dl_ccch(const dl_ccch_msg_s& dl_ccch_msg)
{
  // pack DL CCCH msg
  byte_buffer pdu = pack_into_pdu(dl_ccch_msg);

  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={}", context.ue_index);
  fmt::format_to(fmtbuf2, "CCCH DL {}", dl_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Tx, pdu, dl_ccch_msg, to_c_str(fmtbuf2));

  // send down the stack
  logger.debug(pdu.begin(), pdu.end(), "ue={} C-RNTI={} TX {} PDU", context.ue_index, context.c_rnti, srb_id_t::srb0);
  f1ap_pdu_notifier.on_new_rrc_pdu(srb_id_t::srb0, std::move(pdu));
}

void rrc_ue_impl::send_dl_dcch(srb_id_t srb_id, const dl_dcch_msg_s& dl_dcch_msg)
{
  if (context.srbs.find(srb_id) == context.srbs.end()) {
    logger.error(
        "ue={} C-RNTI={} TX {} is not set up - dropping DL DCCH message.", context.ue_index, context.c_rnti, srb_id);
    return;
  }

  // pack DL CCCH msg
  byte_buffer pdu = pack_into_pdu(dl_dcch_msg);

  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={}", context.ue_index);
  fmt::format_to(fmtbuf2, "DCCH DL {}", dl_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Tx, pdu, dl_dcch_msg, to_c_str(fmtbuf2));

  // pack PDCP PDU and send down the stack
  byte_buffer pdcp_pdu = context.srbs.at(srb_id).pack_rrc_pdu(std::move(pdu));
  logger.debug(pdcp_pdu.begin(), pdcp_pdu.end(), "ue={} C-RNTI={} TX {} PDU", context.ue_index, context.c_rnti, srb_id);
  f1ap_pdu_notifier.on_new_rrc_pdu(srb_id, std::move(pdcp_pdu));
}

void rrc_ue_impl::send_rrc_reject(uint8_t reject_wait_time_secs)
{
  dl_ccch_msg_s     dl_ccch_msg;
  rrc_reject_ies_s& reject = dl_ccch_msg.msg.set_c1().set_rrc_reject().crit_exts.set_rrc_reject();

  // See TS 38.331, RejectWaitTime
  if (reject_wait_time_secs > 0) {
    reject.wait_time_present = true;
    reject.wait_time         = reject_wait_time_secs;
  }

  send_dl_ccch(dl_ccch_msg);
}
