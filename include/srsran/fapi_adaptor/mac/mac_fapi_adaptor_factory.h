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

#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

/// Configuration parameters for the MAC&ndash;FAPI adaptor factory.
struct mac_fapi_adaptor_factory_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Cell number of resource blocks.
  unsigned cell_nof_prbs;
  /// Subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs;
};

/// Dependencies for the MAC&ndash;FAPI adaptor factory.
struct mac_fapi_adaptor_factory_dependencies {
  /// Slot-specific FAPI message gateway.
  fapi::slot_message_gateway* gateway;
  /// Slot-specific last message notifier.
  fapi::slot_last_message_notifier* last_msg_notifier;
  /// Precoding matrix mapper.
  std::unique_ptr<precoding_matrix_mapper> pm_mapper;
  /// UCI Part2 mapper.
  std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper;
};

/// Factory to create \c mac_fapi_adaptor objects.
class mac_fapi_adaptor_factory
{
public:
  virtual ~mac_fapi_adaptor_factory() = default;

  /// Creates a \c mac_fapi_adaptor object using the given configuration and dependencies.
  virtual std::unique_ptr<mac_fapi_adaptor> create(const mac_fapi_adaptor_factory_config&  config,
                                                   mac_fapi_adaptor_factory_dependencies&& dependencies) = 0;
};

/// Creates a \c mac_fapi_adaptor_factory object.
std::unique_ptr<mac_fapi_adaptor_factory> create_mac_fapi_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsran
