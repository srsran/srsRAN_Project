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

#include <memory>
#include <vector>

namespace srsgnb {

class uplink_processor;
class uplink_processor_pool;

/// Configuration parameters for uplink processors.
struct uplink_processor_config {
  /// Radio sector identifier.
  unsigned sector_id;
};

/// Uplink processor factory.
class uplink_processor_factory
{
public:
  /// Default destructor.
  virtual ~uplink_processor_factory() = default;

  /// \brief Creates an uplink processor with the given configuration.
  ///
  /// \param[in] config Contains the configuration to create the uplink processor.
  /// \return A unique pointer to the new uplink processor if successful or \c nullptr otherwise.
  virtual std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) = 0;
};

/// Describes all uplink processors in a pool.
struct uplink_processor_pool_config {
  /// Uplink processors for a given sector and numerology.
  struct info {
    /// Radio sector identifier.
    unsigned sector;
    /// Numerology index.
    unsigned numerology;
    /// Pointers to the actual uplink processors.
    std::vector<std::unique_ptr<uplink_processor>> procs;
  };

  /// Collection of all uplink processors, organized by radio sector and numerology.
  std::vector<info> ul_processors;
  /// Number of radio sectors.
  unsigned num_sectors;
};

/// \brief Creates and returns an uplink processor pool.
std::unique_ptr<uplink_processor_pool> create_uplink_processor_pool(uplink_processor_pool_config config);

} // namespace srsgnb
