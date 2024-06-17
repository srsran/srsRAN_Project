/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/security/security.h"

namespace srsran {

namespace srs_cu_cp {

// Forward declarations.
class up_resource_manager;
class ue_task_scheduler;
class ngap_rrc_ue_pdu_notifier;
class ngap_rrc_ue_control_notifier;

/// Common UE interface.
class cu_cp_ue_impl_interface
{
public:
  virtual ~cu_cp_ue_impl_interface() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() = 0;

  /// \brief Get the UP resource manager of the UE.
  virtual up_resource_manager& get_up_resource_manager() = 0;

  /// \brief Get the task scheduler of the UE.
  virtual ue_task_scheduler& get_task_sched() = 0;

  /// \brief Get the security context of the UE.
  virtual security::security_context& get_security_context() = 0;

  /// \brief Get the RRC UE PDU notifier of the UE.
  virtual ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() = 0;

  /// \brief Get the RRC UE control notifier of the UE.
  virtual ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
