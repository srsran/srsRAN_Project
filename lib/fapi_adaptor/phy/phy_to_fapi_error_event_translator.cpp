/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "phy_to_fapi_error_event_translator.h"
#include "srsran/fapi/message_builders.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {
class slot_error_notifier_dummy : public fapi::slot_error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

} // namespace

static slot_error_notifier_dummy dummy_notifier;

phy_to_fapi_error_event_translator::phy_to_fapi_error_event_translator() : error_notifier(dummy_notifier) {}

void phy_to_fapi_error_event_translator::on_late_downlink_message(srsran::slot_point dl_frame_slot)
{
  error_notifier.get().on_error_indication(fapi::build_error_indication(dl_frame_slot.sfn(),
                                                                        dl_frame_slot.slot_index(),
                                                                        fapi::message_type_id::dl_tti_request,
                                                                        fapi::error_code_id::out_of_sync));
}
