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

#include "f1c_du_bearer_impl.h"
#include "du/procedures/f1ap_du_event_manager.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/bcd_helpers.h"

using namespace srsran::srs_du;

f1c_srb0_du_bearer::f1c_srb0_du_bearer(f1ap_ue_context&           ue_ctxt_,
                                       const nr_cell_global_id_t& pcell_cgi,
                                       const byte_buffer&         du_cu_rrc_container_,
                                       f1ap_message_notifier&     f1ap_notifier_,
                                       f1c_rx_sdu_notifier&       f1c_rx_sdu_notifier_,
                                       f1ap_event_manager&        ev_manager_,
                                       task_executor&             ctrl_exec_,
                                       task_executor&             ue_exec_) :
  ue_ctxt(ue_ctxt_),
  nr_cgi(pcell_cgi),
  du_cu_rrc_container(du_cu_rrc_container_.copy()),
  f1ap_notifier(f1ap_notifier_),
  sdu_notifier(f1c_rx_sdu_notifier_),
  ev_manager(ev_manager_),
  ctrl_exec(ctrl_exec_),
  ue_exec(ue_exec_),
  logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1c_srb0_du_bearer::handle_sdu(byte_buffer_chain sdu)
{
  // Ensure SRB tasks are handled within the control executor.
  if (not ctrl_exec.execute([this, sdu = std::move(sdu)]() {
        const protocol_transaction<f1ap_transaction_response> transaction =
            ev_manager.transactions.create_transaction();

        // Pack Initial UL RRC Message Transfer as per TS38.473, Section 8.4.1.
        f1ap_message msg;
        msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
        asn1::f1ap::init_ul_rrc_msg_transfer_s& init_msg = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
        init_msg->gnb_du_ue_f1ap_id                      = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
        init_msg->nr_cgi.plmn_id.from_number(plmn_string_to_bcd(nr_cgi.plmn));
        init_msg->nr_cgi.nr_cell_id.from_number(nr_cgi.nci);
        init_msg->c_rnti = to_value(ue_ctxt.rnti);
        if (not init_msg->rrc_container.append(sdu.begin(), sdu.end())) {
          logger.error("UL {} SRB0 Tx PDU: Discarding Tx PDU. Cause: Failed to append SDU to RRC container.", ue_ctxt);
          return;
        }
        init_msg->du_to_cu_rrc_container_present           = true;
        init_msg->du_to_cu_rrc_container                   = std::move(du_cu_rrc_container);
        init_msg->sul_access_ind_present                   = false;
        init_msg->transaction_id                           = transaction.id();
        init_msg->ran_ue_id_present                        = false;
        init_msg->rrc_container_rrc_setup_complete_present = false;

        // Notify upper layers of the initial UL RRC Message Transfer.
        f1ap_notifier.on_new_message(msg);

        logger.info("UL {} SRB0 Tx PDU: Initial UL RRC Message Transfer", ue_ctxt);
      })) {
    logger.error("UL {} SRB0: Discarding Tx PDU. Cause: The task executor queue is full.", ue_ctxt);
  }
}

void f1c_srb0_du_bearer::handle_pdu(byte_buffer pdu)
{
  logger.info("DL {} SRB0 Rx PDU: DL RRC Message Transfer", ue_ctxt);

  // Change to UE execution context before forwarding the SDU to lower layers.
  if (not ue_exec.execute([this, sdu = std::move(pdu)]() mutable { sdu_notifier.on_new_sdu(std::move(sdu), {}); })) {
    logger.error("DL {} SRB0 Rx PDU: Discarding Rx PDU. Cause: The task executor queue is full.", ue_ctxt);
  }
}

f1c_other_srb_du_bearer::f1c_other_srb_du_bearer(f1ap_ue_context&       ue_ctxt_,
                                                 srb_id_t               srb_id_,
                                                 f1ap_message_notifier& f1ap_notifier_,
                                                 f1c_rx_sdu_notifier&   f1c_sdu_notifier_,
                                                 task_executor&         ctrl_exec_,
                                                 task_executor&         ue_exec_) :
  ue_ctxt(ue_ctxt_),
  srb_id(srb_id_),
  f1ap_notifier(f1ap_notifier_),
  sdu_notifier(f1c_sdu_notifier_),
  ctrl_exec(ctrl_exec_),
  ue_exec(ue_exec_),
  logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1c_other_srb_du_bearer::handle_sdu(byte_buffer_chain sdu)
{
  // Ensure SRB tasks are handled within the control executor as they involve access to the UE context.
  if (not ctrl_exec.execute([this, sdu = std::move(sdu)]() {
        gnb_cu_ue_f1ap_id_t cu_ue_id = ue_ctxt.gnb_cu_ue_f1ap_id;
        if (cu_ue_id >= gnb_cu_ue_f1ap_id_t::max) {
          logger.warning(
              "ue={} rnti={} GNB-DU-UE-F1AP-ID={} SRB={}: Discarding F1AP RX SDU. Cause: GNB-CU-UE-F1AP-ID is invalid.",
              ue_ctxt.ue_index,
              ue_ctxt.rnti,
              ue_ctxt.gnb_du_ue_f1ap_id,
              srb_id_to_uint(srb_id));
          return;
        }

        f1ap_message msg;

        // Fill F1AP UL RRC Message Transfer.
        msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
        asn1::f1ap::ul_rrc_msg_transfer_s& ul_msg = msg.pdu.init_msg().value.ul_rrc_msg_transfer();
        ul_msg->gnb_du_ue_f1ap_id                 = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
        ul_msg->gnb_cu_ue_f1ap_id                 = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.gnb_cu_ue_f1ap_id);
        ul_msg->srb_id                            = srb_id_to_uint(srb_id);
        if (not ul_msg->rrc_container.append(sdu.begin(), sdu.end())) {
          logger.error("UL {} SRB{} Tx PDU: Discarding Tx PDU. Cause: Failed to append SDU to RRC container.",
                       ue_ctxt,
                       srb_id_to_uint(srb_id));
          return;
        }
        ul_msg->sel_plmn_id_present           = false;
        ul_msg->new_gnb_du_ue_f1ap_id_present = false;

        f1ap_notifier.on_new_message(msg);

        logger.info("UL {} SRB{} Tx PDU: UL RRC Message Transfer", ue_ctxt, srb_id_to_uint(srb_id));
      })) {
    logger.error("UL {} SRB{} Tx PDU: Discarding Tx PDU. Cause: The task executor queue is full.",
                 ue_ctxt,
                 srb_id_to_uint(srb_id));
  }
}

void f1c_other_srb_du_bearer::handle_pdu(srsran::byte_buffer pdu)
{
  if (pdu.length() < 3) {
    logger.warning("DL {} SRB{} Rx PDU: Invalid PDU length. Dropping PDU.", ue_ctxt, srb_id_to_uint(srb_id));
    return;
  }

  uint32_t pdcp_sn = get_srb_pdcp_sn(pdu);

  // Change to UE execution context before forwarding the SDU to lower layers.
  if (not ue_exec.execute(
          [this, sdu = std::move(pdu), pdcp_sn]() mutable { sdu_notifier.on_new_sdu(std::move(sdu), pdcp_sn); })) {
    logger.error(
        "{} SRB{} Rx PDU: Discarding Rx PDU. Cause: The task executor queue is full.", ue_ctxt, srb_id_to_uint(srb_id));
  } else {
    logger.info("DL {} SRB{} Rx PDU: DL RRC Message Transfer", ue_ctxt, srb_id_to_uint(srb_id));
  }
}

uint32_t f1c_other_srb_du_bearer::get_srb_pdcp_sn(const byte_buffer& buf)
{
  byte_buffer::const_iterator buf_it = buf.begin();

  // Extract PDCP SN (SRBs are always 12bits)
  uint32_t sn = {};
  sn          = (*buf_it & 0x0fU) << 8U; // first 4 bits SN (R bits are dropped)
  ++buf_it;
  sn |= (*buf_it & 0xffU); // last 8 bits SN
  ++buf_it;

  return sn;
}
