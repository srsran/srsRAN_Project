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

#pragma once

#include "ngap_repository.h"
#include "ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_configurator.h"

namespace srsran {

class cu_configurator_impl : public cu_configurator
{
public:
  cu_configurator_impl(srs_cu_cp::mobility_manager_cu_cp_notifier& mobility_notif_,
                       srs_cu_cp::du_processor_repository&         du_db_,
                       srs_cu_cp::ngap_repository&                 ngap_db_,
                       srs_cu_cp::ue_manager&                      ue_mng_) :
    mobility_notif(mobility_notif_), du_db(du_db_), ngap_db(ngap_db_), ue_mng(ue_mng_)
  {
  }

  /// Get UE index for a given tuple of (AMF-UE-NGAP-ID, GUAMI, GNB-CU-UE-F1AP-ID).
  srs_cu_cp::ue_index_t get_ue_index(const srs_cu_cp::amf_ue_id_t& amf_ue_id,
                                     const srs_cu_cp::guami_t&     guami,
                                     const gnb_cu_ue_f1ap_id_t&    gnb_cu_ue_f1ap_id) const override;

  /// Get index of the DU that has UE with a given GNB-CU-UE-F1AP-ID.
  srs_cu_cp::du_index_t get_du_index(const srs_cu_cp::ue_index_t& ue_index) const override;

  /// Get DU index for a given NR Cell Global ID.
  srs_cu_cp::du_index_t get_du_index(const nr_cell_global_id_t& nr_cgi) const override;

  /// Get PCI for a given NR Cell Global ID.
  pci_t get_pci(const nr_cell_global_id_t& nr_cgi) const override;

  async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>
  trigger_handover(const srs_cu_cp::du_index_t&                      source_du_index,
                   const srs_cu_cp::cu_cp_intra_cu_handover_request& handover_req) override;

private:
  async_task<srs_cu_cp::cu_cp_intra_cu_handover_response> return_handover_response(bool ack);

  srs_cu_cp::mobility_manager_cu_cp_notifier& mobility_notif;
  srs_cu_cp::du_processor_repository&         du_db;
  srs_cu_cp::ngap_repository&                 ngap_db;
  srs_cu_cp::ue_manager&                      ue_mng;
};

} // namespace srsran
