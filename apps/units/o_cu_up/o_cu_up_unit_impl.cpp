/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_up_unit_impl.h"

using namespace srsran;

o_cu_up_unit_impl::o_cu_up_unit_impl(std::unique_ptr<srs_cu_up::ngu_gateway>          gateway_,
                                     std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                                     std::unique_ptr<srs_cu_up::o_cu_up>              cu_up_) :
  gateway(std::move(gateway_)), e2_metric_connector(std::move(e2_metric_connector_)), cu_up(std::move(cu_up_))
{
  srsran_assert(gateway, "Invlid NGU gateway");
  srsran_assert(cu_up, "Invalid CU-UP");
}

srs_cu_up::cu_up_interface& o_cu_up_unit_impl::get_cu_up()
{
  return cu_up->get_cu_up();
}

srs_cu_up::cu_up_power_controller& o_cu_up_unit_impl::get_power_controller()
{
  return cu_up->get_power_controller();
}
