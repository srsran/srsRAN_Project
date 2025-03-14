
#pragma once

#include "srsran/mac/cell_configuration.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network slice configuration
struct mac_slice_configuration {
  // TODO: Fill remaining fields
};

/// MAC cell positioning measurement request.
struct mac_cell_positioning_measurement_request {
  /// UE identifier in case the SRSConfig belongs to a UE connected to this DU. Otherwise, it is empty.
  std::optional<du_ue_index_t> ue_index;
  std::optional<rnti_t>        rnti;
  /// SRS resources to measure.
  srs_config srs_to_meas;
};

/// MAC cell positioning measurement response.
struct mac_cell_positioning_measurement_response {
  std::vector<phy_time_unit> ul_rtoas;
};

/// Reconfiguration of a MAC cell during its operation.
struct mac_cell_reconfig_request {
  /// If not empty, passes a new SIB1 payload to broadcast.
  byte_buffer new_sib1_buffer;
  /// If not empty, the MAC is requested to collect a new positioning measurement.
  std::optional<mac_cell_positioning_measurement_request> positioning;
};

struct mac_cell_reconfig_response {
  /// Whether a pending SIB1 reconfiguration was successful.
  bool                                                     sib1_updated = false;
  std::optional<mac_cell_positioning_measurement_response> positioning;
};

/// Interface used to handle a MAC cell activation/reconfiguration/deactivation.
class mac_cell_controller
{
public:
  virtual ~mac_cell_controller() = default;

  /// Start the cell.
  virtual async_task<void> start() = 0;

  /// Stop the cell.
  virtual async_task<void> stop() = 0;

  /// Reconfigure operation cell.
  virtual async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) = 0;
};

/// Class used to setup the MAC cells and slices.
class mac_cell_manager
{
public:
  virtual ~mac_cell_manager() = default;

  /// Add new cell configuration.
  virtual void add_cell(const mac_cell_creation_request& cell_cfg) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell state controller.
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
