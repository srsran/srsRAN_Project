/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
