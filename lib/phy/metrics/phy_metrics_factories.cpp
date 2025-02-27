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

#include "srsran/phy/metrics/phy_metrics_factories.h"
#include "phy_metrics_channel_equalizer_decorator.h"
#include "phy_metrics_channel_precoder_decorator.h"
#include "phy_metrics_crc_calculator_decorator.h"
#include "phy_metrics_demodulation_mapper_decorator.h"
#include "phy_metrics_downlink_processor_decorator.h"
#include "phy_metrics_evm_calculator_decorator.h"
#include "phy_metrics_ldpc_decoder_decorator.h"
#include "phy_metrics_ldpc_encoder_decorator.h"
#include "phy_metrics_ldpc_rate_dematcher_decorator.h"
#include "phy_metrics_ldpc_rate_matcher_decorator.h"
#include "phy_metrics_modulation_mapper_decorator.h"
#include "phy_metrics_pdsch_dmrs_generator_decorator.h"
#include "phy_metrics_pdsch_processor_decorator.h"
#include "phy_metrics_port_channel_estimator_decorator.h"
#include "phy_metrics_pseudo_random_generator_decorator.h"
#include "phy_metrics_pusch_channel_estimator_decorator.h"
#include "phy_metrics_pusch_demodulator_decorator.h"
#include "phy_metrics_pusch_processor_decorator.h"
#include "phy_metrics_time_alignment_estimator_decorator.h"
#include "phy_metrics_transform_precoder_decorator.h"
#include "phy_metrics_ulsch_demultiplex_decorator.h"

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

/// Metric decorator factory for port channel estimator.
class metric_decorator_port_channel_estimator_factory : public port_channel_estimator_factory
{
public:
  /// Creates the factory from a base factory and notifier.
  metric_decorator_port_channel_estimator_factory(std::shared_ptr<port_channel_estimator_factory> base_factory_,
                                                  port_channel_estimator_metric_notifier&         notifier_) :
    base_factory(std::move(base_factory_)), notifier(notifier_)
  {
    srsran_assert(base_factory, "Invalid base factory.");
  }

  // See interface for documentation.
  std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
         port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
         bool                                             complensate_cfo) override
  {
    return std::make_unique<phy_metrics_port_channel_estimator_decorator>(
        base_factory->create(fd_smoothing_strategy, td_interpolation_strategy, complensate_cfo), notifier);
  }

private:
  std::shared_ptr<port_channel_estimator_factory> base_factory;
  port_channel_estimator_metric_notifier&         notifier;
};

/// Metric decorator factory for downlink processor.
class metric_decorator_downlink_processor_factory : public downlink_processor_factory
{
public:
  metric_decorator_downlink_processor_factory(std::shared_ptr<downlink_processor_factory> base_factory_,
                                              downlink_processor_metric_notifier&         notifier_) :
    base_factory(std::move(base_factory_)), notifier(notifier_)
  {
    srsran_assert(base_factory, "Invalid base factory.");
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor_controller> create(const downlink_processor_config& config) override
  {
    // Create RG gateway adaptor.
    auto gateway_adaptor =
        std::make_unique<phy_metrics_downlink_processor_decorator::rg_gateway_adaptor>(*config.gateway, notifier);

    // Modify base configuration.
    downlink_processor_config base_config = config;
    base_config.gateway                   = &gateway_adaptor->get_gateway();

    return std::make_unique<phy_metrics_downlink_processor_decorator>(base_factory->create(base_config),
                                                                      std::move(gateway_adaptor));
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor_controller>
  create(const downlink_processor_config& config, srslog::basic_logger& logger, bool enable_broadcast) override
  {
    // Create RG gateway adaptor.
    auto gateway_adaptor =
        std::make_unique<phy_metrics_downlink_processor_decorator::rg_gateway_adaptor>(*config.gateway, notifier);

    // Modify base configuration.
    downlink_processor_config base_config = config;
    base_config.gateway                   = &gateway_adaptor->get_gateway();

    return std::make_unique<phy_metrics_downlink_processor_decorator>(
        base_factory->create(base_config, logger, enable_broadcast), std::move(gateway_adaptor));
  }
  std::unique_ptr<downlink_pdu_validator> create_pdu_validator() override
  {
    return base_factory->create_pdu_validator();
  }

private:
  std::shared_ptr<downlink_processor_factory> base_factory;
  downlink_processor_metric_notifier&         notifier;
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

std::shared_ptr<ldpc_rate_matcher_factory>
srsran::create_ldpc_rate_matcher_metric_decorator_factory(std::shared_ptr<ldpc_rate_matcher_factory> base_factory,
                                                          ldpc_rate_matcher_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<ldpc_rate_matcher,
                                                   ldpc_rate_matcher_factory,
                                                   ldpc_rate_matcher_metric_notifier,
                                                   phy_metrics_ldpc_rate_matcher_decorator>>(std::move(base_factory),
                                                                                             notifier);
}

std::shared_ptr<ldpc_rate_dematcher_factory>
srsran::create_ldpc_rate_dematcher_metric_decorator_factory(std::shared_ptr<ldpc_rate_dematcher_factory> base_factory,
                                                            ldpc_rate_dematcher_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<ldpc_rate_dematcher,
                                                   ldpc_rate_dematcher_factory,
                                                   ldpc_rate_dematcher_metric_notifier,
                                                   phy_metrics_ldpc_rate_dematcher_decorator>>(std::move(base_factory),
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
                                                         channel_precoder_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<channel_precoder,
                                                   channel_precoder_factory,
                                                   channel_precoder_metric_notifier,
                                                   phy_metrics_channel_precoder_decorator>>(std::move(base_factory),
                                                                                            notifier);
}

std::shared_ptr<channel_equalizer_factory>
srsran::create_channel_equalizer_metric_decorator_factory(std::shared_ptr<channel_equalizer_factory> base_factory,
                                                          channel_equalizer_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<channel_equalizer,
                                                   channel_equalizer_factory,
                                                   channel_equalizer_metric_notifier,
                                                   phy_metrics_channel_equalizer_decorator>>(std::move(base_factory),
                                                                                             notifier);
}

std::shared_ptr<modulation_mapper_factory>
srsran::create_modulation_mapper_metric_decorator_factory(std::shared_ptr<modulation_mapper_factory> base_factory,
                                                          common_channel_modulation_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_factory<modulation_mapper,
                                                   modulation_mapper_factory,
                                                   common_channel_modulation_metric_notifier,
                                                   phy_metrics_modulation_mapper_decorator>>(std::move(base_factory),
                                                                                             notifier);
}

std::shared_ptr<demodulation_mapper_factory>
srsran::create_demodulation_mapper_metric_decorator_factory(std::shared_ptr<demodulation_mapper_factory> base_factory,
                                                            common_channel_modulation_metric_notifier&   notifier)
{
  return std::make_shared<metric_decorator_factory<demodulation_mapper,
                                                   demodulation_mapper_factory,
                                                   common_channel_modulation_metric_notifier,
                                                   phy_metrics_demodulation_mapper_decorator>>(std::move(base_factory),
                                                                                               notifier);
}

std::shared_ptr<evm_calculator_factory>
srsran::create_evm_calculator_metric_decorator_factory(std::shared_ptr<evm_calculator_factory>    base_factory,
                                                       common_channel_modulation_metric_notifier& notifier)
{
  return std::make_shared<metric_decorator_factory<evm_calculator,
                                                   evm_calculator_factory,
                                                   common_channel_modulation_metric_notifier,
                                                   phy_metrics_evm_calculator_decorator>>(std::move(base_factory),
                                                                                          notifier);
}

std::shared_ptr<ulsch_demultiplex_factory>
srsran::create_ulsch_demultiplex_metric_decorator_factory(std::shared_ptr<ulsch_demultiplex_factory> base_factory,
                                                          ulsch_demultiplex_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<ulsch_demultiplex,
                                                   ulsch_demultiplex_factory,
                                                   ulsch_demultiplex_metric_notifier,
                                                   phy_metrics_ulsch_demultiplex_decorator>>(std::move(base_factory),
                                                                                             notifier);
}

std::shared_ptr<pusch_demodulator_factory>
srsran::create_pusch_demodulator_metric_decorator_factory(std::shared_ptr<pusch_demodulator_factory> base_factory,
                                                          pusch_demodulator_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<pusch_demodulator,
                                                   pusch_demodulator_factory,
                                                   pusch_demodulator_metric_notifier,
                                                   phy_metrics_pusch_demodulator_decorator>>(std::move(base_factory),
                                                                                             notifier);
}

std::shared_ptr<time_alignment_estimator_factory> srsran::create_time_alignment_estimator_metric_decorator_factory(
    std::shared_ptr<time_alignment_estimator_factory> base_factory,
    time_alignment_estimator_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<time_alignment_estimator,
                                                   time_alignment_estimator_factory,
                                                   time_alignment_estimator_metric_notifier,
                                                   phy_metrics_time_alignment_estimator_decorator>>(
      std::move(base_factory), notifier);
}

std::shared_ptr<port_channel_estimator_factory> srsran::create_port_channel_estimator_metric_decorator_factory(
    std::shared_ptr<port_channel_estimator_factory> base_factory,
    port_channel_estimator_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_port_channel_estimator_factory>(std::move(base_factory), notifier);
}

std::shared_ptr<transform_precoder_factory>
srsran::create_transform_precoder_metric_decorator_factory(std::shared_ptr<transform_precoder_factory> base_factory,
                                                           transform_precoder_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<transform_precoder,
                                                   transform_precoder_factory,
                                                   transform_precoder_metric_notifier,
                                                   phy_metrics_transform_precoder_decorator>>(std::move(base_factory),
                                                                                              notifier);
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_metric_decorator_factory(std::shared_ptr<pdsch_processor_factory> base_factory,
                                                        pdsch_processor_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory_with_validator<pdsch_processor,
                                                                  pdsch_pdu_validator,
                                                                  pdsch_processor_factory,
                                                                  pdsch_processor_metric_notifier,
                                                                  phy_metrics_pdsch_processor_decorator>>(
      std::move(base_factory), notifier);
}

std::shared_ptr<dmrs_pdsch_processor_factory>
srsran::create_dmrs_pdsch_generator_metric_decorator_factory(std::shared_ptr<dmrs_pdsch_processor_factory> base_factory,
                                                             pdsch_dmrs_generator_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_factory<dmrs_pdsch_processor,
                                                   dmrs_pdsch_processor_factory,
                                                   pdsch_dmrs_generator_metric_notifier,
                                                   phy_metrics_dmrs_pdsch_processor_decorator>>(std::move(base_factory),
                                                                                                notifier);
}

std::shared_ptr<downlink_processor_factory> srsran::create_downlink_processor_generator_metric_decorator_factory(
    std::shared_ptr<downlink_processor_factory> base_factory,
    downlink_processor_metric_notifier&         notifier)
{
  return std::make_shared<metric_decorator_downlink_processor_factory>(std::move(base_factory), notifier);
}
