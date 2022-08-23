/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "dmrs_pbch_processor_impl.h"
#include "dmrs_pdcch_processor_impl.h"
#include "dmrs_pdsch_processor_impl.h"
#include "dmrs_pusch_estimator_impl.h"
#include "port_channel_estimator_average_impl.h"
#include "pss_processor_impl.h"
#include "sss_processor_impl.h"

using namespace srsgnb;

namespace {

class dmrs_pbch_processor_sw_factory : public dmrs_pbch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  explicit dmrs_pbch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<dmrs_pbch_processor> create() override
  {
    return std::make_unique<dmrs_pbch_processor_impl>(prg_factory->create());
  }
};

class dmrs_pdcch_processor_sw_factory : public dmrs_pdcch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  explicit dmrs_pdcch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<dmrs_pdcch_processor> create() override
  {
    return std::make_unique<dmrs_pdcch_processor_impl>(prg_factory->create());
  }
};

class dmrs_pdsch_processor_sw_factory : public dmrs_pdsch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  explicit dmrs_pdsch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<dmrs_pdsch_processor> create() override
  {
    return std::make_unique<dmrs_pdsch_processor_impl>(prg_factory->create());
  }
};

class dmrs_pusch_estimator_factory_sw : public dmrs_pusch_estimator_factory
{
public:
  dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                  std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory_) :
    prg_factory(std::move(prg_factory_)), ch_estimator_factory(std::move(ch_estimator_factory_))
  {
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
    srsgnb_assert(ch_estimator_factory, "Invalid channel estimator factory.");
  }

  std::unique_ptr<dmrs_pusch_estimator> create() override
  {
    return std::make_unique<dmrs_pusch_estimator_impl>(prg_factory->create(), ch_estimator_factory->create());
  }

private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory;
};

class port_channel_estimator_factory_sw : public port_channel_estimator_factory
{
public:
  std::unique_ptr<port_channel_estimator> create() override
  {
    return std::make_unique<port_channel_estimator_average_impl>();
  }
};

class pss_processor_factory_sw : public pss_processor_factory
{
public:
  std::unique_ptr<pss_processor> create() override { return std::make_unique<pss_processor_impl>(); }
};

class sss_processor_factory_sw : public sss_processor_factory
{
public:
  std::unique_ptr<sss_processor> create() override { return std::make_unique<sss_processor_impl>(); }
};

} // namespace

std::shared_ptr<dmrs_pbch_processor_factory>
srsgnb::create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<dmrs_pbch_processor_sw_factory>(std::move(prg_factory));
}

std::shared_ptr<dmrs_pdcch_processor_factory>
srsgnb::create_dmrs_pdcch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<dmrs_pdcch_processor_sw_factory>(std::move(prg_factory));
}

std::shared_ptr<dmrs_pdsch_processor_factory>
srsgnb::create_dmrs_pdsch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<dmrs_pdsch_processor_sw_factory>(std::move(prg_factory));
}

std::shared_ptr<dmrs_pusch_estimator_factory>
srsgnb::create_dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                               std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory)
{
  return std::make_shared<dmrs_pusch_estimator_factory_sw>(std::move(prg_factory), std::move(ch_estimator_factory));
}

std::shared_ptr<port_channel_estimator_factory> srsgnb::create_port_channel_estimator_factory_sw()
{
  return std::make_shared<port_channel_estimator_factory_sw>();
}

std::shared_ptr<pss_processor_factory> srsgnb::create_pss_processor_factory_sw()
{
  return std::make_shared<pss_processor_factory_sw>();
}

std::shared_ptr<sss_processor_factory> srsgnb::create_sss_processor_factory_sw()
{
  return std::make_shared<sss_processor_factory_sw>();
}
