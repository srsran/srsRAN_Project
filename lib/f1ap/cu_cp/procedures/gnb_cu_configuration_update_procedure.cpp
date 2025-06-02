/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "gnb_cu_configuration_update_procedure.h"
#include "asn1_helpers.h"
#include "f1ap_asn1_utils.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

/// \brief Convert the GNB CU Configuration Update from common type to ASN.1.
/// \param[out] asn1_cfg_update The ASN.1 struct to store the result.
/// \param[in] cfg_update The common type GNB CU Configuration Update.
static void fill_asn1_gnb_cu_configuration_update(asn1::f1ap::gnb_cu_cfg_upd_s&           asn1_cfg_update,
                                                  const f1ap_gnb_cu_configuration_update& cfg_update);

/// \brief Convert the GNB CU Configuration Update Failure from ASN.1 to common type.
/// \param[out] response  The common type struct to store the result.
/// \param[in] asn1_failure The ASN.1 GNB CU Configuration Update Failure.
static void fill_f1ap_gnb_cu_configuration_update_response(f1ap_gnb_cu_configuration_update_response& response,
                                                           const asn1::f1ap::gnb_cu_cfg_upd_fail_s&   asn1_failure);

/// \brief Convert the GNB CU Configuration Update Acknowledge from ASN.1 to common type.
/// \param[in] response The common type GNB CU Configuration Update Acknowledge.
/// \param[in] asn1_ack The ASN.1 struct to convert.
static void fill_f1ap_gnb_cu_configuration_update_response(f1ap_gnb_cu_configuration_update_response& response,
                                                           const asn1::f1ap::gnb_cu_cfg_upd_ack_s&    asn1_ack);

// ---- GNB CU Configuration Update Procedure ----

gnb_cu_configuration_update_procedure::gnb_cu_configuration_update_procedure(
    const f1ap_configuration&               f1ap_cfg_,
    const f1ap_gnb_cu_configuration_update& request_,
    f1ap_message_notifier&                  f1ap_notif_,
    f1ap_event_manager&                     ev_mng_,
    srslog::basic_logger&                   logger_) :
  f1ap_cfg(f1ap_cfg_), request(request_), f1ap_notifier(f1ap_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void gnb_cu_configuration_update_procedure::operator()(
    coro_context<async_task<f1ap_gnb_cu_configuration_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\": started...", name());

  // Create transaction.
  transaction = ev_mng.transactions.create_transaction(f1ap_cfg.proc_timeout);

  // Send command to DU.
  send_gnb_cu_configuration_update();

  // Await DU response.
  CORO_AWAIT(transaction);

  // Handle result of the transaction.
  CORO_RETURN(handle_procedure_result());
}

void gnb_cu_configuration_update_procedure::send_gnb_cu_configuration_update()
{
  // Pack message into PDU.
  f1ap_message f1ap_gnb_cu_config_update_msg;
  f1ap_gnb_cu_config_update_msg.pdu.set_init_msg();
  f1ap_gnb_cu_config_update_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
  gnb_cu_cfg_upd_s& req = f1ap_gnb_cu_config_update_msg.pdu.init_msg().value.gnb_cu_cfg_upd();

  req->transaction_id = transaction.id();

  // Convert common type to ASN1.
  fill_asn1_gnb_cu_configuration_update(req, request);

  // Send GNB CU Configuration Update.
  f1ap_notifier.on_new_message(f1ap_gnb_cu_config_update_msg);
}

f1ap_gnb_cu_configuration_update_response gnb_cu_configuration_update_procedure::handle_procedure_result()
{
  response.success = false;

  if (not transaction.valid()) {
    // Transaction could not be allocated.
    logger.debug("\"{}\" cancelled. Cause: Failed to allocate transaction", name());
    return response;
  }

  if (transaction.aborted()) {
    // Timeout or cancellation case.
    logger.debug("\"{}\" cancelled. Cause: Timeout reached", name());
    return response;
  }

  if (transaction.has_response()) {
    if (transaction.response().has_value()) {
      // F1 GNB CU Configuration Update Acknowledge case.
      const auto& asn1_resp = transaction.response().value().value.gnb_cu_cfg_upd_ack();
      fill_f1ap_gnb_cu_configuration_update_response(response, asn1_resp);

      logger.debug("\"{}\": finished successfully", name());
    } else {
      // F1 GNB CU Configuration Update Failure case.
      const auto& asn1_fail = transaction.response().error().value.gnb_cu_cfg_upd_fail();
      fill_f1ap_gnb_cu_configuration_update_response(response, asn1_fail);

      logger.debug("\"{}\": failed. Cause: {}", name(), get_cause_str(asn1_fail->cause));
    }
  }

  return response;
}

static void fill_asn1_gnb_cu_configuration_update(asn1::f1ap::gnb_cu_cfg_upd_s&           asn1_cfg_update,
                                                  const f1ap_gnb_cu_configuration_update& cfg_update)
{
  // Fill cells to be activated list.
  if (!cfg_update.cells_to_be_activated_list.empty()) {
    asn1_cfg_update->cells_to_be_activ_list_present = true;

    for (const auto& cell : cfg_update.cells_to_be_activated_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> asn1_cell_container;
      cells_to_be_activ_list_item_s& asn1_cell = asn1_cell_container->cells_to_be_activ_list_item();

      asn1_cell.nr_cgi = cgi_to_asn1(cell.cgi);
      if (cell.pci.has_value()) {
        asn1_cell.nr_pci_present = true;
        asn1_cell.nr_pci         = cell.pci.value();
      }

      if (!cell.available_plmn_list.empty()) {
        asn1_cell.ie_exts_present                     = true;
        asn1_cell.ie_exts.available_plmn_list_present = true;

        for (const auto& plmn : cell.available_plmn_list) {
          available_plmn_list_item_s asn1_plmn;
          asn1_plmn.plmn_id = plmn.to_bytes();
          asn1_cell.ie_exts.available_plmn_list.push_back(asn1_plmn);
        }
      }

      asn1_cfg_update->cells_to_be_activ_list.push_back(asn1_cell_container);
    }
  }

  // Fill cells to be deactivated list.
  if (!cfg_update.cells_to_be_deactivated_list.empty()) {
    asn1_cfg_update->cells_to_be_deactiv_list_present = true;

    for (const auto& cell : cfg_update.cells_to_be_deactivated_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_deactiv_list_item_ies_o> asn1_cell_container;
      cells_to_be_deactiv_list_item_s& asn1_cell = asn1_cell_container->cells_to_be_deactiv_list_item();

      asn1_cell.nr_cgi = cgi_to_asn1(cell.cgi);

      asn1_cfg_update->cells_to_be_deactiv_list.push_back(asn1_cell_container);
    }
  }

  // Fill GNB CU name.
  if (!cfg_update.gnb_cu_name.empty()) {
    asn1_cfg_update->gnb_cu_name_present = true;
    asn1_cfg_update->gnb_cu_name.from_string(cfg_update.gnb_cu_name);
  }
}

static void fill_f1ap_gnb_cu_configuration_update_response(f1ap_gnb_cu_configuration_update_response& response,
                                                           const asn1::f1ap::gnb_cu_cfg_upd_fail_s&   asn1_failure)
{
  response.success = false;
  // Fill cause.
  response.cause = asn1_to_cause(asn1_failure->cause);
}

static void fill_f1ap_gnb_cu_configuration_update_response(f1ap_gnb_cu_configuration_update_response& response,
                                                           const asn1::f1ap::gnb_cu_cfg_upd_ack_s&    asn1_ack)
{
  response.success = true;
  // Fill cells failed to be activated list.
  if (asn1_ack->cells_failed_to_be_activ_list_present) {
    for (const auto& asn1_cell_failed : asn1_ack->cells_failed_to_be_activ_list) {
      f1ap_cell_failed_to_activate cell_failed;
      cell_failed.cgi   = cgi_from_asn1(asn1_cell_failed->cells_failed_to_be_activ_list_item().nr_cgi).value();
      cell_failed.cause = asn1_to_cause(asn1_cell_failed->cells_failed_to_be_activ_list_item().cause);

      response.cells_failed_to_be_activated_list.push_back(cell_failed);
    }
  }
}
