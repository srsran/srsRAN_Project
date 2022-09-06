/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_to_mac_data_msg_translator.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/mac/mac_cell_rach_handler.h"

using namespace srsgnb;
using namespace fapi_adaptor;

namespace {

class mac_cell_rach_handler_dummy : public mac_cell_rach_handler
{
public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC RACH handler, which will be later set up through the \ref set_cell_rach_handler() method.
static mac_cell_rach_handler_dummy dummy_mac_rach_handler;

fapi_to_mac_data_msg_translator::fapi_to_mac_data_msg_translator(subcarrier_spacing scs) :
  rach_handler(dummy_mac_rach_handler), scs(scs)
{
}

void fapi_to_mac_data_msg_translator::on_dl_tti_response(const fapi::dl_tti_response_message& msg) {}

void fapi_to_mac_data_msg_translator::on_rx_data_indication(const fapi::rx_data_indication_message& msg) {}

void fapi_to_mac_data_msg_translator::on_crc_indication(const fapi::crc_indication_message& msg) {}

void fapi_to_mac_data_msg_translator::on_uci_indication(const fapi::uci_indication_message& msg) {}

void fapi_to_mac_data_msg_translator::on_srs_indication(const fapi::srs_indication_message& msg) {}

/// Converts the given FAPI RACH occasion RSSI to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_rssi_dB(int fapi_rssi)
{
  return (fapi_rssi - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble power to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_power_dB(int fapi_power)
{
  return (fapi_power - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble SNR to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_snr_dB(int fapi_snr)
{
  return (fapi_snr - 128) * 0.5F;
}

void fapi_to_mac_data_msg_translator::on_rach_indication(const fapi::rach_indication_message& msg)
{
  mac_rach_indication indication;
  indication.slot_rx = slot_point(to_numerology_value(scs), msg.sfn, msg.slot);
  for (const auto& pdu : msg.pdus) {
    srsgnb_assert(pdu.avg_rssi != std::numeric_limits<uint16_t>::max(), "Average RSSI field not set");

    indication.occasions.emplace_back();
    mac_rach_indication::rach_occasion& occas = indication.occasions.back();
    occas.frequency_index                     = pdu.ra_index;
    occas.slot_index                          = pdu.slot_index;
    occas.start_symbol                        = pdu.symbol_index;
    occas.rssi_dB                             = to_rssi_dB(pdu.avg_rssi);
    for (const auto& preamble : pdu.preambles) {
      srsgnb_assert(preamble.preamble_pwr != std::numeric_limits<uint32_t>::max(), "Preamble power field not set");
      srsgnb_assert(preamble.preamble_snr != std::numeric_limits<uint8_t>::max(), "Preamble SNR field not set");

      occas.preambles.emplace_back();
      mac_rach_indication::rach_preamble& mac_pream = occas.preambles.back();
      mac_pream.index                               = preamble.preamble_index;
      mac_pream.time_advance = phy_time_unit::from_seconds(preamble.timing_advance_offset_ns * 1e-9);
      mac_pream.power_dB     = to_power_dB(preamble.preamble_pwr);
      mac_pream.snr_dB       = to_snr_dB(preamble.preamble_snr);
    }
  }

  rach_handler.get().handle_rach_indication(indication);
}

void fapi_to_mac_data_msg_translator::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  rach_handler = std::ref(mac_rach_handler);
}
