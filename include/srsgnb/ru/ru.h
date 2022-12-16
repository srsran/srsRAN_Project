/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/restricted_set_config.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

class prach_buffer;
class resource_grid_reader;

/// RU PRACH control PDU.
struct ru_prach_control_pdu {
  /// Identifies this PRACH control PDU.
  unsigned id;
  /// Sector identifier.
  unsigned sector;
  /// Port identifier within the sector.
  unsigned port;
  /// Slot context within the system frame.
  slot_point slot;
};

/// RU downlink slot control PDU.
struct ru_downlink_slot_control_pdu {
  /// Identifies this RU downlink control PDU.
  unsigned id;
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
};

/// RU uplink slot control PDU.
struct ru_uplink_slot_control_pdu {
  /// Identifies this RU downlink control PDU.
  unsigned id;
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
};

/// Radio Unit Control Plane notifier.
class ru_cp_notifier
{
public:
  virtual ~ru_cp_notifier() = default;

  /// Notifies a new slot.
  virtual void on_new_slot(slot_point slot) = 0;
};

/// Radio Unit Control Plane handler.
class ru_cp_handler
{
public:
  virtual ~ru_cp_handler() = default;

  /// Handles a PRACH occassion with the given control information.
  ///
  /// \param[in] data Contains the control PDU data.
  virtual void handle_prach_occasion(const ru_prach_control_pdu& data) = 0;

  /// Handles a new downlink slot with the given control information.
  ///
  /// \param[in] data Contains the control PDU data.
  virtual void handle_new_downlink_slot(const ru_downlink_slot_control_pdu& data) = 0;

  /// Handles a new uplink slot with the given control information.
  ///
  /// \param[in] data Contains the control PDU data.
  virtual void handle_new_uplink_slot(const ru_uplink_slot_control_pdu& data) = 0;
};

/// \brief Radio Unit Control Plane interface.
///
/// Control plane interface of a radio unit. The interface works with slots as the signaling granularity.
class ru_cp
{
public:
  virtual ~ru_cp() = default;

  /// Returns the RU-CP handler of this RU-CP.
  virtual ru_cp_handler& get_handler() = 0;

  /// \brief Sets the RU-CP notifier of this RU-CP.
  ///
  /// \param[in] notifier RU-CP notifier to set.
  virtual void set_notifier(ru_cp_notifier& notifier) = 0;
};

/// Radio Unit User Plane notifier.
class ru_up_notifier
{
public:
  virtual ~ru_up_notifier() = default;

  /// \brief Notifies that new uplink data is available.
  ///
  /// This callback will only be executed if a previous control plane PDU was sent.
  ///
  /// \param[in] ru_ul_control_pdu_id RU uplink control PDU identifier to which this data belongs.
  /// \param[in] grid Uplink data grid.
  virtual void on_new_uplink_data(unsigned ru_ul_control_pdu_id, const resource_grid_reader& grid) = 0;

  /// \brief Notifies the completion of PRACH window.
  ///
  /// The RU uses this method to notify that a PRACH window identified by \c ru_prach_control_pdu_id has been written in
  /// \c buffer.
  ///
  /// \param[in] ru_prach_control_pdu_id PRACH control PDU identifier.
  /// \param[in] buffer  Read-only PRACH buffer.
  virtual void on_new_prach_window_data(unsigned ru_prach_control_pdu_id, const prach_buffer& buffer) = 0;
};

/// Radio Unit User Plane gateway.
class ru_up_gateway
{
public:
  virtual ~ru_up_gateway() = default;

  /// \brief Sends the given data associated to the RU downlink control PDU index.
  ///
  /// If the identifier cannot be found (no previous associated control plane message) the data is ignored and will not
  /// be sent.
  ///
  /// \param[in] ru_dl_control_pdu_id Identifier of the RU downlink control pdu.
  /// \param[in] grid Downlink data to send.
  virtual void send_dl_data(unsigned ru_dl_control_pdu_id, const resource_grid_reader& grid) = 0;
};

/// \brief Radio Unit User Plane interface.
///
/// User plane interface of a radio unit. The interface works with slots as the signaling granularity.
class ru_up
{
public:
  virtual ~ru_up() = default;

  /// Returns the RU-UP gateway of this RU-UP.
  virtual ru_up_gateway& get_gateway() = 0;

  /// Sets the RU-UP uplink data notifier of this RU-UP.
  virtual void set_notifier(ru_up_notifier& notifier) = 0;
};

/// Radio Unit interface.
class ru
{
public:
  virtual ~ru() = default;

  /// Returns the Control Plane interface of this RU.
  virtual ru_cp& get_control_plane() = 0;

  /// Returns the User Plane interface of this RU.
  virtual ru_up& get_user_plane() = 0;
};

} // namespace srsgnb