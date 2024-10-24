/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_wrapper_impl.h"
#include "srsran/du/du_high/du_high_wrapper.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

du_wrapper_impl::du_wrapper_impl(du_wrapper_impl_dependencies&& dependencies) :
  du_hi(std::move(dependencies.du_hi)), du_lo(std::move(dependencies.du_lo))
{
  srsran_assert(du_lo, "Invalid DU low");
  srsran_assert(du_hi, "Invalid DU high");
}

void du_wrapper_impl::start()
{
  du_hi->get_power_controller().start();
}

void du_wrapper_impl::stop()
{
  du_lo->get_du_low().get_power_controller().stop();
  du_hi->get_power_controller().stop();
}

du_high_wrapper& du_wrapper_impl::get_du_high_wrapper()
{
  return *du_hi;
}

du_low_wrapper& du_wrapper_impl::get_du_low_wrapper()
{
  return *du_lo;
}
