/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void phy_to_fapi_error_event_translator::on_late_uplink_message(srsran::slot_point ul_frame_slot)
{
  error_notifier.get().on_error_indication(fapi::build_error_indication(ul_frame_slot.sfn(),
                                                                        ul_frame_slot.slot_index(),
                                                                        fapi::message_type_id::ul_tti_request,
                                                                        fapi::error_code_id::out_of_sync));
}
