/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/nrppa/nrppa.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

class nrppa_dummy_impl : public nrppa_interface, public nrppa_message_handler, public nrppa_measurement_handler
{
public:
  nrppa_dummy_impl();
  ~nrppa_dummy_impl();

  // See nrppa_message_handler for documentation.
  void handle_new_nrppa_pdu(const byte_buffer& nrppa_pdu, std::optional<ue_index_t> ue_index) override;

  // See nrppa_measurement_handler for documentation.
  void handle_ue_measurement(ue_index_t ue_index, const cell_measurement_positioning_info& meas_result) override;

  nrppa_message_handler&     get_nrppa_message_handler() override { return *this; }
  nrppa_measurement_handler& get_nrppa_measurement_handler() override { return *this; }

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
