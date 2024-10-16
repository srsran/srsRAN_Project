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
#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// Implements a resource grid mapper.
class resource_grid_mapper_impl : public resource_grid_mapper
{
public:
  resource_grid_mapper_impl(std::unique_ptr<channel_precoder> precoder_);

  ~resource_grid_mapper_impl() = default;

  // See interface for documentation.
  void map(resource_grid_writer&          writer,
           const re_buffer_reader<>&      input,
           const re_pattern&              pattern,
           const precoding_configuration& precoding) override;

  // See interface for documentation.
  void map(resource_grid_writer&          writer,
           symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding,
           unsigned                       re_skip) override;

private:
  /// Maximum number of subcarriers that can be accomodated in an OFDM symbol.
  static constexpr unsigned max_nof_subcarriers = MAX_RB * NRE;
  /// Maximum number of ports to map in a mapping call.
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;

  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;
};

} // namespace srsran
