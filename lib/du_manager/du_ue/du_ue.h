/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ran_resource_management/cell_group_config.h"
#include "../ran_resource_management/du_ran_resource_manager.h"
#include "du_ue_bearer_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_du {

class du_ue_rlf_handler : public mac_ue_radio_link_notifier, public rlc_tx_upper_layer_control_notifier
{
public:
  ~du_ue_rlf_handler() = default;

  /// \brief Notify that the UE has now an SRB2 and DRB configured, so we need to account for its potential RRC
  /// reestablishment.
  virtual void on_drb_and_srb2_configured() = 0;

  /// \brief Notify the RLF handler to stop listening to new RLF triggers.
  virtual void disconnect() = 0;
};

/// \brief This class holds the context of an UE in the DU.
struct du_ue {
  explicit du_ue(du_ue_index_t                      ue_index_,
                 du_cell_index_t                    pcell_index_,
                 rnti_t                             rnti_,
                 std::unique_ptr<du_ue_rlf_handler> rlf_notifier_,
                 ue_ran_resource_configurator       resources_) :
    ue_index(ue_index_),
    rnti(rnti_),
    pcell_index(pcell_index_),
    f1ap_ue_id(int_to_gnb_du_ue_f1ap_id(ue_index_)),
    rlf_notifier(std::move(rlf_notifier_)),
    resources(std::move(resources_))
  {
  }

  const du_ue_index_t ue_index;
  rnti_t              rnti;
  du_cell_index_t     pcell_index;
  gnb_du_ue_f1ap_id_t f1ap_ue_id;

  std::unique_ptr<du_ue_rlf_handler> rlf_notifier;

  du_ue_bearer_manager         bearers;
  ue_ran_resource_configurator resources;
  bool                         reestablishment_pending = false;
};

} // namespace srs_du
} // namespace srsran
