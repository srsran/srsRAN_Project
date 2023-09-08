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

#include "du_ue.h"
#include "srsran/gtpu/gtpu_teid_pool.h"

namespace srsran {
namespace srs_du {

/// \brief Interface used by DU Manager procedures to add/find/remove UEs from the DU UE Manager.
class du_ue_manager_repository
{
public:
  virtual ~du_ue_manager_repository() = default;

  /// \brief Adds a new UE context in the DU UE manager repository.
  virtual du_ue* add_ue(std::unique_ptr<du_ue> u) = 0;

  /// \brief removes an existing ue context from the du ue manager repository.
  virtual void remove_ue(du_ue_index_t ue_index) = 0;

  /// \brief Update the CRNTI of an existing UE context.
  virtual void update_crnti(du_ue_index_t ue_index, rnti_t crnti) = 0;

  /// \brief Find UE context based on DU UE index.
  virtual du_ue* find_ue(du_ue_index_t ue_index) = 0;

  /// \brief Find UE context based on UE RNTI.
  virtual du_ue* find_rnti(rnti_t rnti) = 0;

  /// \brief Handle detected Radio Link Failures.
  virtual void handle_radio_link_failure(du_ue_index_t ue_index, rlf_cause cause) = 0;

  /// \brief Access to the TEID pool that can be used to allocate/deallocate unique TEIDs for F1-U bearers.
  virtual gtpu_teid_pool& get_f1u_teid_pool() = 0;

  /// \brief Schedule task for a given UE.
  virtual void schedule_async_task(du_ue_index_t ue_index, async_task<void> task) = 0;
};

} // namespace srs_du
} // namespace srsran
