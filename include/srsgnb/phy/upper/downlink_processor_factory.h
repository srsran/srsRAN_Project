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

class downlink_processor;
class downlink_processor_pool;

/// Configuration parameters for downlink processors.
struct downlink_processor_config {
  /// Radio sector identifier.
  unsigned id;
};

/// Factory that allows to create downlink processors.
class downlink_processor_factory
{
public:
  virtual ~downlink_processor_factory() = default;

  /// \brief Creates a return a downlink processor.
  virtual std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) = 0;
};

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_config {
  /// Downlink processors for a given sector and numerology.
  struct info {
    /// Radio sector identifier.
    unsigned sector;
    /// Numerology index.
    unsigned numerology;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor>> procs;
  };

  /// Collection of all downlink processors, organized by radio sector and numerology.
  std::vector<info> dl_processors;
  /// Number of radio sectors.
  unsigned num_sectors;
};

/// \brief Creates and returns a downlink processor pool.
std::unique_ptr<downlink_processor_pool> create_dl_processor_pool(downlink_processor_pool_config config);

} // namespace srsgnb
