#pragma once

#include "srsgnb/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsgnb/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

namespace srsgnb {

class port_channel_estimator_factory;

std::unique_ptr<csi_rs_processor> create_csi_rs_processor();

class dmrs_pbch_processor_factory
{
public:
  virtual ~dmrs_pbch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pbch_processor> create() = 0;
};

std::shared_ptr<dmrs_pbch_processor_factory>
create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class dmrs_pdcch_processor_factory
{
public:
  virtual ~dmrs_pdcch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pdcch_processor> create() = 0;
};

std::shared_ptr<dmrs_pdcch_processor_factory>
create_dmrs_pdcch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class dmrs_pdsch_processor_factory
{
public:
  virtual ~dmrs_pdsch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pdsch_processor> create() = 0;
};

std::shared_ptr<dmrs_pdsch_processor_factory>
create_dmrs_pdsch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class dmrs_pusch_estimator_factory
{
public:
  virtual ~dmrs_pusch_estimator_factory()                = default;
  virtual std::unique_ptr<dmrs_pusch_estimator> create() = 0;
};

std::shared_ptr<dmrs_pusch_estimator_factory>
create_dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory);

class port_channel_estimator_factory
{
public:
  virtual ~port_channel_estimator_factory()                = default;
  virtual std::unique_ptr<port_channel_estimator> create() = 0;
};

std::shared_ptr<port_channel_estimator_factory> create_port_channel_estimator_factory_sw();

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

} // namespace srsgnb
