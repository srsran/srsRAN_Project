/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_impl.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

o_du_impl::o_du_impl(o_du_impl_dependencies&& dependencies) :
  du_hi(std::move(dependencies.du_hi)), du_lo(std::move(dependencies.du_lo))
{
  srsran_assert(du_lo, "Invalid DU low");
  srsran_assert(du_hi, "Invalid DU high");

  // Connect O-DU low with O-DU high.
  for (unsigned i = 0, e = du_lo->get_du_low().get_all_upper_phys().size(); i != e; ++i) {
    du_lo->set_slot_time_message_notifier(i, du_hi->get_slot_time_message_notifier(i));
    du_lo->set_slot_error_message_notifier(i, du_hi->get_slot_error_message_notifier(i));
    du_lo->set_slot_data_message_notifier(i, du_hi->get_slot_data_message_notifier(i));
  }
}

void o_du_impl::start()
{
  du_hi->get_power_controller().start();
}

void o_du_impl::stop()
{
  du_lo->get_du_low().get_power_controller().stop();
  du_hi->get_power_controller().stop();
}

o_du_high& o_du_impl::get_o_du_high()
{
  return *du_hi;
}

o_du_low& o_du_impl::get_o_du_low()
{
  return *du_lo;
}
