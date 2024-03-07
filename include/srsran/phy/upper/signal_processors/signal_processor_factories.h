#pragma once

#include "port_channel_estimator_parameters.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/pss_processor.h"
#include "srsran/phy/upper/signal_processors/sss_processor.h"

namespace srsran {

class port_channel_estimator_factory;

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

class dmrs_pucch_estimator_factory
{
public:
  virtual ~dmrs_pucch_estimator_factory()                        = default;
  virtual std::unique_ptr<dmrs_pucch_processor> create_format1() = 0;
  virtual std::unique_ptr<dmrs_pucch_processor> create_format2() = 0;
};

std::shared_ptr<dmrs_pucch_estimator_factory>
create_dmrs_pucch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>      prg_factory,
                                       std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory,
                                       std::shared_ptr<port_channel_estimator_factory>       ch_estimator_factory);

class dmrs_pusch_estimator_factory
{
public:
  virtual ~dmrs_pusch_estimator_factory()                = default;
  virtual std::unique_ptr<dmrs_pusch_estimator> create() = 0;
};

std::shared_ptr<dmrs_pusch_estimator_factory>
create_dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory);

class nzp_csi_rs_generator_factory
{
public:
  virtual ~nzp_csi_rs_generator_factory()                                        = default;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create()           = 0;
  virtual std::unique_ptr<nzp_csi_rs_configuration_validator> create_validator() = 0;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create(srslog::basic_logger& logger);
};

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_pool_factory(std::shared_ptr<nzp_csi_rs_generator_factory> generator_factory,
                                         unsigned                                      nof_concurrent_threads);

class port_channel_estimator_factory
{
public:
  virtual ~port_channel_estimator_factory() = default;
  virtual std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy =
             port_channel_estimator_fd_smoothing_strategy::filter) = 0;
};

std::shared_ptr<port_channel_estimator_factory>
create_port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory);

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
