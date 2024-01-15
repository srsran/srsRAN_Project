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

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/f1ap/du/f1c_bearer.h"
#include "srsran/f1ap/du/f1c_rx_sdu_notifier.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include <vector>

namespace srsran {
namespace srs_du {

/// \brief F1-C bearer to Add or Modify in UE F1 context.
struct f1c_bearer_to_addmod {
  srb_id_t             srb_id;
  f1c_rx_sdu_notifier* rx_sdu_notifier;
};

/// \brief F1c bearer Added or Modified in UE F1 context.
struct f1c_bearer_addmodded {
  srb_id_t    srb_id;
  f1c_bearer* bearer;
};

/// \brief Request sent to the DU F1AP to create a new UE F1AP context.
struct f1ap_ue_creation_request {
  du_ue_index_t                     ue_index;
  du_cell_index_t                   pcell_index;
  rnti_t                            c_rnti;
  byte_buffer                       du_cu_rrc_container;
  std::vector<f1c_bearer_to_addmod> f1c_bearers_to_add;
};

/// \brief Response from the DU F1AP to the request to create a new UE.
struct f1ap_ue_creation_response {
  bool                     result     = false;
  gnb_du_ue_f1ap_id_t      f1ap_ue_id = gnb_du_ue_f1ap_id_t::invalid;
  std::vector<f1c_bearer*> f1c_bearers_added;
};

/// \brief Request sent to the DU F1AP to update the configuration of an existing UE F1AP context.
struct f1ap_ue_configuration_request {
  du_ue_index_t                     ue_index;
  std::vector<f1c_bearer_to_addmod> f1c_bearers_to_add;
};

/// \brief Response from the DU F1AP to the request to update the configuration of an existing UE.
struct f1ap_ue_configuration_response {
  std::vector<f1c_bearer_addmodded> f1c_bearers_added;
};

} // namespace srs_du
} // namespace srsran
