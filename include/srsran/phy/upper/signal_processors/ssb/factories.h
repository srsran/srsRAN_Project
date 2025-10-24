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

#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/ssb/dmrs_pbch_processor.h"
#include "srsran/phy/upper/signal_processors/ssb/pss_processor.h"
#include "srsran/phy/upper/signal_processors/ssb/sss_processor.h"

namespace srsran {

class dmrs_pbch_processor_factory
{
public:
  virtual ~dmrs_pbch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pbch_processor> create() = 0;
};

std::shared_ptr<dmrs_pbch_processor_factory>
create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class pss_processor_factory
{
public:
  virtual ~pss_processor_factory()                = default;
  virtual std::unique_ptr<pss_processor> create() = 0;
};

std::shared_ptr<pss_processor_factory> create_pss_processor_factory_sw();

class sss_processor_factory
{
public:
  virtual ~sss_processor_factory()                = default;
  virtual std::unique_ptr<sss_processor> create() = 0;
};

std::shared_ptr<sss_processor_factory> create_sss_processor_factory_sw();

} // namespace srsran
