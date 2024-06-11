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

#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

/// Sounding Reference Signal propagation channel estimator factory.
class srs_estimator_factory
{
public:
  /// Default destructor.
  virtual ~srs_estimator_factory() = default;

  /// Creates a Sounding Reference Signal based propagation channel estimator.
  virtual std::unique_ptr<srs_estimator> create() = 0;

  /// Creates a Sounding Reference Signal based propagation channel estimator with logging.
  virtual std::unique_ptr<srs_estimator> create(srslog::basic_logger& logger);

  /// Creates a Sounding Reference Signal channel estimator configuration validator.
  virtual std::unique_ptr<srs_estimator_configuration_validator> create_validator() = 0;
};

/// Create a generic SRS propagation channel estimator factory.
std::shared_ptr<srs_estimator_factory>
create_srs_estimator_generic_factory(std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory,
                                     std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory);

} // namespace srsran
