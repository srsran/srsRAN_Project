/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_impl.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/phy/p7/phy_fapi_p7_sector_adaptor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_o_du_impl::split6_o_du_impl(unsigned                                        nof_cells_,
                                   std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> adaptor_,
                                   std::unique_ptr<srs_du::o_du_high>              odu_hi_) :
  nof_cells(nof_cells_), odu_hi(std::move(odu_hi_)), adaptor(std::move(adaptor_))
{
  srsran_assert(odu_hi, "Invalid O-DU high");
  srsran_assert(adaptor, "Invalid PHY-FAPI adaptor");

  for (unsigned i = 0; i != nof_cells; ++i) {
    auto& p7_mac_sector_adaptor = odu_hi->get_mac_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p7_sector_adaptor();
    auto& p7_phy_sector_adaptor = adaptor->get_sector_adaptor(i).get_p7_sector_adaptor();

    p7_phy_sector_adaptor.set_slot_data_message_notifier(p7_mac_sector_adaptor.get_slot_data_message_notifier());
    p7_phy_sector_adaptor.set_error_message_notifier(p7_mac_sector_adaptor.get_error_message_notifier());
    p7_phy_sector_adaptor.set_slot_time_message_notifier(p7_mac_sector_adaptor.get_slot_time_message_notifier());

    // TODO: connect P5
  }
}

void split6_o_du_impl::start()
{
  odu_hi->get_operation_controller().start();

  for (unsigned i = 0; i != nof_cells; ++i) {
    adaptor->get_sector_adaptor(i).start();
  }
}

void split6_o_du_impl::stop()
{
  odu_hi->get_operation_controller().stop();

  for (unsigned i = 0; i != nof_cells; ++i) {
    adaptor->get_sector_adaptor(i).stop();
  }
}
