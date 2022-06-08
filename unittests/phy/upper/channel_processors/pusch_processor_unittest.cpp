/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

int main()
{
  pusch_processor_factory_sw_configuration pusch_processor_factory_sw_config = {};
  std::shared_ptr<pusch_processor_factory> pusch_processor_factory =
      create_pusch_processor_factory_sw(pusch_processor_factory_sw_config);
  TESTASSERT(pusch_processor_factory);

  std::unique_ptr<pusch_processor> processor = pusch_processor_factory->create();
  TESTASSERT(processor);

  return 0;
}
