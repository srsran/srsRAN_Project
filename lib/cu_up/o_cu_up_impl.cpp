/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_up_impl.h"

using namespace srsran;
using namespace srs_cu_up;

void o_cu_up_impl::start()
{
  cu_up->start();
}

void o_cu_up_impl::stop()
{
  cu_up->stop();
}

void o_cu_up_with_e2_impl::start()
{
  o_cu_up_impl::start();
  e2agent->start();
}

void o_cu_up_with_e2_impl::stop()
{
  e2agent->stop();
  o_cu_up_impl::stop();
}