/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/fapi/messages.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <memory>

namespace srsran {

class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class uplink_slot_pdu_repository;

namespace fapi_adaptor {

/// Configuration parameters for the PHY&ndash;FAPI adaptor factory.
struct phy_fapi_adaptor_factory_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// PRACH configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x1031.
  fapi::prach_config prach_cfg;
  /// Carrier configuration per SCF-222 v4.0 Section 3.3.2.4 TLV 0x102d.
  fapi::carrier_config carrier_cfg;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
};

/// Factory to create \c phy_fapi_adaptor objects.
class phy_fapi_adaptor_factory
{
public:
  virtual ~phy_fapi_adaptor_factory() = default;

  /// Creates a \c phy_fapi_adaptor object using the given configuration.
  virtual std::unique_ptr<phy_fapi_adaptor> create(phy_fapi_adaptor_factory_config&& config) = 0;
};

/// Creates a \c phy_fapi_adaptor_factory object.
std::unique_ptr<phy_fapi_adaptor_factory>
create_phy_fapi_adaptor_factory(downlink_processor_pool&      dl_processor_pool,
                                resource_grid_pool&           rg_pool,
                                uplink_request_processor&     ul_request_processor,
                                resource_grid_pool&           ul_rg_pool,
                                uplink_slot_pdu_repository&   ul_pdu_repository,
                                const downlink_pdu_validator& dl_pdu_validator,
                                const uplink_pdu_validator&   ul_pdu_validator);

} // namespace fapi_adaptor
} // namespace srsran
