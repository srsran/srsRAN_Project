/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_wrapper.h"

using namespace srsran;

cu_up_wrapper::cu_up_wrapper(std::unique_ptr<srs_cu_up::ngu_gateway>          gateway_,
                             std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                             std::unique_ptr<srs_cu_up::cu_up_interface>      cu_up_) :
  gateway(std::move(gateway_)), e2_metric_connector(std::move(e2_metric_connector_)), cu_up(std::move(cu_up_))
{
  srsran_assert(gateway, "Invlid NGU gateway");
  srsran_assert(cu_up, "Invalid CU-UP");
}

void cu_up_wrapper::start()
{
  cu_up->start();
}

void cu_up_wrapper::stop()
{
  cu_up->stop();
}

std::optional<uint16_t> cu_up_wrapper::get_n3_bind_port()
{
  return cu_up->get_n3_bind_port();
}
