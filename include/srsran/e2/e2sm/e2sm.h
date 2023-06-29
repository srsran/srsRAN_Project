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

#include "srsran/asn1/e2ap/e2sm_kpm.h"

class e2sm_handler
{
public:
  virtual ~e2sm_handler() = default;
  /// \brief Handle the packed E2SM-KPM Action Definition.
  /// \param[in] buf
  /// \return Returns the unpacked E2SM-KPM Action Definition.
  virtual asn1::e2sm_kpm::e2_sm_kpm_action_definition_s
  handle_packed_e2sm_kpm_action_definition(const srsran::byte_buffer& buf) = 0;
  /// \brief Handle the packed E2SM-KPM Event Trigger Definition.
  /// \param[in] buf
  /// \return Returns the unpacked E2SM-KPM Event Trigger Definition.
  virtual asn1::e2sm_kpm::e2_sm_kpm_event_trigger_definition_s
  handle_packed_event_trigger_definition(const srsran::byte_buffer& buf) = 0;
};

class e2sm_interface
{
public:
  virtual ~e2sm_interface() = default;
  /// \brief Handle the E2SM-KPM Action Definition.
  /// \param[in] action_id
  /// \param[in] action_definition
  /// \return Returns the packed resultant Indication Message.
  virtual srsran::byte_buffer handle_action(uint32_t action_id, const srsran::byte_buffer& action_definition) = 0;
  /// \brief get the indication header associated with the action id.
  /// \param[in] action_id
  /// \return Returns the indication header.
  virtual srsran::byte_buffer get_indication_header(uint32_t action_id) = 0;
  /// \brief gets a reference to the packer for this service model.
  /// \return Returns a reference to the packer.
  virtual e2sm_handler& get_e2sm_packer() = 0;
};
