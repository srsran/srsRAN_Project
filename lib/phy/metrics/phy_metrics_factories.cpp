/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/metrics/phy_metrics_factories.h"
#include "phy_metrics_channel_equalizer_decorator.h"
#include "phy_metrics_channel_precoder_decorator.h"
#include "phy_metrics_crc_calculator_decorator.h"
#include "phy_metrics_ldpc_decoder_decorator.h"
#include "phy_metrics_ldpc_encoder_decorator.h"
#include "phy_metrics_pseudo_random_generator_decorator.h"
#include "phy_metrics_pusch_channel_estimator_decorator.h"
#include "phy_metrics_pusch_processor_decorator.h"

using namespace srsran;

namespace {

/// \brief Generic metric decorator factory.
///
/// Describes a generic factory that decorates any physical layer block with a metric decorator.
///
/// \tparam PhyBlock  Physical layer block.
/// \tparam Factory   Physical layer block factory.
/// \tparam Notifier  Metric notifier corresponding to the physical layer block.
/// \tparam Decorator Decorator class to use.
template <typename PhyBlock, typename Factory, typename Notifier, typename Decorator>
class metric_decorator_factory : public Factory
{
public:
  metric_decorator_factory(std::shared_ptr<Factory> base_factory_, Notifier& notifier_) :
    base_factory(std::move(base_factory_)), notifier(notifier_)
  {
    srsran_assert(base_factory, "Invalid factory.");
  }

  // See interface for documentation.
  std::unique_ptr<PhyBlock> create() override { return std::make_unique<Decorator>(base_factory->create(), notifier); }

private:
  std::shared_ptr<Factory> base_factory;
  Notifier&                notifier;
};

template <typename PhyBlock, typename PhyBlockValidator, typename Factory, typename Notifier, typename Decorator>
class metric_decorator_factory_with_validator : public Factory
{
public:
  metric_decorator_factory_with_validator(std::shared_ptr<Factory> base_factory_, Notifier& notifier_) :
    base_factory(std::move(base_factory_)), notifier(notifier_)
  {
    srsran_assert(base_factory, "Invalid factory.");
  }

  // See interface for documentation.
  std::unique_ptr<PhyBlock> create() override { return std::make_unique<Decorator>(base_factory->create(), notifier); }

  // See interface for documentation.
  std::unique_ptr<PhyBlockValidator> create_validator() override { return base_factory->create_validator(); }

private:
  std::shared_ptr<Factory> base_factory;
  Notifier&                notifier;
};

/// Metric decorator factory for CRC calculators.
class metric_decorator_crc_calculator_factory : public crc_calculator_factory
{
public:
  /// Creates the factory from a base factory and notifier.
  metric_decorator_crc_calculator_factory(std::shared_ptr<crc_calculator_factory> base_factory_,
                                          crc_calculator_metric_notifier&         notifier_) :
    base_factory(std::move(base_factory_)), notifier(notifier_)
  {
    srsran_assert(base_factory, "Invalid base factory.");
  }

  // See interface for documentation.
  std::unique_ptr<crc_calculator> create(crc_generator_poly poly) override
  {
    return std::make_unique<phy_metrics_crc_calculator_decorator>(base_factory->create(poly), notifier);
  }

private:
  std::shared_ptr<crc_calculator_factory> base_factory;
  crc_calculator_metric_notifier&         notifier;
};

} // namespace

std::shared_ptr<crc_calculator_factory>
srsran::create_crc_calculator_metric_decorator_factory(std::shared_ptr<crc_calculator_factory> base_factory,
                                                       srsran::crc_calculator_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_crc_calculator_factory>(base_factory, notifier);
}

std::shared_ptr<ldpc_encoder_factory>
srsran::create_ldpc_encoder_metric_decorator_factory(std::shared_ptr<ldpc_encoder_factory> base_factory,
                                                     ldpc_encoder_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<ldpc_encoder,
                                                   ldpc_encoder_factory,
                                                   ldpc_encoder_metric_notifier,
                                                   phy_metrics_ldpc_encoder_decorator>>(std::move(base_factory),
                                                                                        notifier);
}

std::shared_ptr<ldpc_decoder_factory>
srsran::create_ldpc_decoder_metric_decorator_factory(std::shared_ptr<ldpc_decoder_factory> base_factory,
                                                     ldpc_decoder_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<ldpc_decoder,
                                                   ldpc_decoder_factory,
                                                   ldpc_decoder_metric_notifier,
                                                   phy_metrics_ldpc_decoder_decorator>>(std::move(base_factory),
                                                                                        notifier);
}

std::shared_ptr<dmrs_pusch_estimator_factory> srsran::create_pusch_channel_estimator_metric_decorator_factory(
    std::shared_ptr<dmrs_pusch_estimator_factory> base_factory,
    pusch_channel_estimator_metric_notifier&      notifier)
{
  return std::make_shared<metric_decorator_factory<dmrs_pusch_estimator,
                                                   dmrs_pusch_estimator_factory,
                                                   pusch_channel_estimator_metric_notifier,
                                                   phy_metrics_pusch_channel_estimator_decorator>>(
      std::move(base_factory), notifier);
}

std::shared_ptr<pusch_processor_factory>
srsran::create_pusch_processor_metric_decorator_factory(std::shared_ptr<pusch_processor_factory> base_factory,
                                                        pusch_processor_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory_with_validator<pusch_processor,
                                                                  pusch_pdu_validator,
                                                                  pusch_processor_factory,
                                                                  pusch_processor_metric_notifier,
                                                                  phy_metrics_pusch_processor_decorator>>(
      std::move(base_factory), notifier);
}

std::shared_ptr<pseudo_random_generator_factory> srsran::create_pseudo_random_generator_metric_decorator_factory(
    std::shared_ptr<pseudo_random_generator_factory>  base_factory,
    pseudo_random_sequence_generator_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_factory<pseudo_random_generator,
                                                   pseudo_random_generator_factory,
                                                   pseudo_random_sequence_generator_metric_notifier,
                                                   phy_metrics_pseudo_random_generator_decorator>>(
      std::move(base_factory), notifier);
}

std::shared_ptr<channel_precoder_factory>
srsran::create_channel_precoder_metric_decorator_factory(std::shared_ptr<channel_precoder_factory> base_factory,
                                                         srsran::channel_precoder_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_factory<channel_precoder,
                                                   channel_precoder_factory,
                                                   channel_precoder_metric_notifier,
                                                   phy_metrics_channel_precoder_decorator>>(std::move(base_factory),
                                                                                            notifier);
}

std::shared_ptr<channel_equalizer_factory>
srsran::create_channel_equalizer_metric_decorator_factory(std::shared_ptr<channel_equalizer_factory> base_factory,
                                                          srsran::channel_equalizer_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_factory<channel_equalizer,
                                                   channel_equalizer_factory,
                                                   channel_equalizer_metric_notifier,
                                                   phy_metrics_channel_equalizer_decorator>>(std::move(base_factory),
                                                                                             notifier);
}
