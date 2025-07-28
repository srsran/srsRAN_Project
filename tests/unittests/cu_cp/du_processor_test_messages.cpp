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

#include "du_processor_test_messages.h"
#include "lib/f1ap/asn1_helpers.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include "lib/f1ap/cu_cp/procedures/f1_setup_procedure.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

void srsran::srs_cu_cp::generate_valid_f1_setup_request(du_setup_request& setup_request,
                                                        gnb_du_id_t       gnb_du_id,
                                                        nr_cell_identity  nci,
                                                        pci_t             pci,
                                                        unsigned          tac)
{
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request(gnb_du_id, {{nci, pci, tac}});
  setup_request            = create_du_setup_request(f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

void srsran::srs_cu_cp::generate_f1_setup_request_base(du_setup_request& setup_request)
{
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x11), {{nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(), 0, 7}});
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();
  setup_request = create_du_setup_request(f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

f1ap_message srsran::srs_cu_cp::create_f1_setup_request_with_too_many_cells(const f1ap_message& base)
{
  f1ap_message msg = base;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = true;
  auto& cells = msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list;
  cells.resize(MAX_NOF_DU_CELLS + 1);
  for (unsigned i = 0; i != cells.size(); ++i) {
    cells[i].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
    cells[i]->gnb_du_served_cells_item() =
        test_helpers::generate_served_cells_item({nr_cell_identity::create(gnb_id_t{411, 22}, i).value(), (pci_t)i, 7});
  }

  return msg;
}

void srsran::srs_cu_cp::generate_f1_setup_request_with_too_many_cells(du_setup_request& setup_request)
{
  f1ap_message f1setup_msg  = test_helpers::generate_f1_setup_request();
  auto&        f1_setup_req = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  f1_setup_req->gnb_du_served_cells_list.clear();

  f1_setup_req->gnb_du_served_cells_list_present = true;

  for (int du_cell_idx_int = du_cell_index_to_uint(du_cell_index_t::min); du_cell_idx_int < MAX_NOF_DU_CELLS + 1;
       du_cell_idx_int++) {
    f1_setup_req->gnb_du_served_cells_list.push_back({});
    f1_setup_req->gnb_du_served_cells_list.back().load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
    f1_setup_req->gnb_du_served_cells_list.back()->gnb_du_served_cells_item() =
        test_helpers::generate_served_cells_item(
            {nr_cell_identity::create(gnb_id_t{411, 22}, du_cell_idx_int).value(), (pci_t)du_cell_idx_int, 7});
  }

  setup_request = create_du_setup_request(f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

ue_rrc_context_creation_request srsran::srs_cu_cp::generate_ue_rrc_context_creation_request(ue_index_t       ue_index,
                                                                                            rnti_t           c_rnti,
                                                                                            nr_cell_identity nrcell_id)
{
  ue_rrc_context_creation_request req = {};
  req.ue_index                        = ue_index;
  req.c_rnti                          = c_rnti;
  req.cgi.plmn_id                     = plmn_identity::test_value();
  req.cgi.nci                         = nrcell_id;
  asn1::unbounded_octstring<true> tmp;
  tmp.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");
  req.du_to_cu_rrc_container = byte_buffer::create(tmp.begin(), tmp.end()).value();

  return req;
}
