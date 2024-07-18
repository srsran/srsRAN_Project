/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"

using namespace srsran;

static void configure_cli11_buffer_pool_args(CLI::App& app, buffer_pool_appconfig& config)
{
  app.add_option("--nof_segments", config.nof_segments, "Number of segments allocated by the buffer pool")
      ->capture_default_str();
  app.add_option("--segment_size", config.segment_size, "Size of each buffer pool segment in bytes")
      ->capture_default_str();
}

void srsran::configure_cli11_with_buffer_pool_appconfig_schema(CLI::App& app, buffer_pool_appconfig& config)
{
  // Buffer pool section.
  CLI::App* buffer_pool_subcmd = app.add_subcommand("buffer_pool", "Buffer pool configuration")->configurable();
  configure_cli11_buffer_pool_args(*buffer_pool_subcmd, config);
}
