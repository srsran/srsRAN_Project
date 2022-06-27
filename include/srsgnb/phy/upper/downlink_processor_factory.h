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

struct downlink_processor_config {
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

struct downlink_processor_pool_config {
  struct info {
    unsigned                                         sector;
    unsigned                                         numerology;
    std::vector<std::unique_ptr<downlink_processor>> procs;
  };

  std::vector<info> dl_processors;
  unsigned          num_sectors;
};

/// \brief Creates and returns a downlink processor pool.
std::unique_ptr<downlink_processor_pool> create_dl_processor_pool(downlink_processor_pool_config config);

} // namespace srsgnb
