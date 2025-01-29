/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_validator.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

/// PRS generator factory.
class prs_generator_factory
{
public:
  /// Default destructor.
  virtual ~prs_generator_factory() = default;

  /// Creates a PRS generator.
  virtual std::unique_ptr<prs_generator> create() = 0;

  /// Creates a PRS generator configuration validator.
  virtual std::unique_ptr<prs_generator_validator> create_validator() = 0;

  /// Creates a PRS generator with a logging decorator.
  virtual std::unique_ptr<prs_generator> create(srslog::basic_logger& logger);
};

/// \brief Creates a generic PRS generator factory.
/// \param[in] prg_factory      Pseudo-random sequence generator factory.
/// \param[in] precoder_factory Channel precoder factory.
/// \return A valid PRS generator factory if successful.
std::shared_ptr<prs_generator_factory>
create_prs_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                     std::shared_ptr<channel_precoder_factory>        precoder_factory);

/// \brief Creates a PRS generator pool.
/// \param[in] generator_factory      Base PRS generator factory.
/// \param[in] nof_concurrent_threads Maximum number of threads that might use the pool.
/// \return A PRS generator that can generate PRS transmission concurrently.
std::shared_ptr<prs_generator_factory>
create_prs_generator_pool_factory(std::shared_ptr<prs_generator_factory> generator_factory,
                                  unsigned                               nof_concurrent_threads);

} // namespace srsran
