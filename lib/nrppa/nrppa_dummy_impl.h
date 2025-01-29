/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/nrppa/nrppa.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

class nrppa_dummy_impl : public nrppa_interface, public nrppa_message_handler, public nrppa_ue_context_removal_handler
{
public:
  nrppa_dummy_impl();
  ~nrppa_dummy_impl();

  // See nrppa_message_handler for documentation.
  void handle_new_nrppa_pdu(const byte_buffer&                    nrppa_pdu,
                            std::variant<ue_index_t, amf_index_t> ue_or_amf_index) override;

  // See nrppa_ue_context_removal_handle for documentation.
  void remove_ue_context(ue_index_t ue_index) override;

  nrppa_message_handler&            get_nrppa_message_handler() override { return *this; }
  nrppa_ue_context_removal_handler& get_nrppa_ue_context_removal_handler() override { return *this; }

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
