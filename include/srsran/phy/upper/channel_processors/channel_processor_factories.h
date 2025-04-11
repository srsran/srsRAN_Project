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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/prach_generator.h"
#include "srsran/srslog/srslog.h"
#include <memory>

namespace srsran {

class prach_generator_factory;
class task_executor;

class prach_detector_factory
{
public:
  virtual ~prach_detector_factory()                                    = default;
  virtual std::unique_ptr<prach_detector>           create()           = 0;
  virtual std::unique_ptr<prach_detector_validator> create_validator() = 0;
  virtual std::unique_ptr<prach_detector>           create(srslog::basic_logger& logger, bool log_all_opportunities);
};

struct prach_detector_factory_sw_configuration {
  unsigned idft_long_size  = 1024;
  unsigned idft_short_size = 256;
  bool     combine_symbols = true;
};

std::shared_ptr<prach_detector_factory>
create_prach_detector_factory_sw(std::shared_ptr<dft_processor_factory>         dft_factory,
                                 std::shared_ptr<prach_generator_factory>       prach_gen_factory,
                                 const prach_detector_factory_sw_configuration& config = {});

std::shared_ptr<prach_detector_factory>
create_prach_detector_pool_factory(std::shared_ptr<prach_detector_factory> factory, unsigned nof_concurrent_threads);

class prach_generator_factory
{
public:
  virtual ~prach_generator_factory()                = default;
  virtual std::unique_ptr<prach_generator> create() = 0;
};

std::shared_ptr<prach_generator_factory> create_prach_generator_factory_sw();

} // namespace srsran
