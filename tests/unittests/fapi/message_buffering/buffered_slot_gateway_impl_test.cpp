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

#include "../../../lib/fapi/message_buffering/buffered_slot_gateway_impl.h"
#include "../message_builder_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

namespace srsran {
namespace fapi {

// The following comparison operators usually do not check PODs, as the default copy constructor should copy them
// correctly.
static bool operator==(const tx_precoding_and_beamforming_pdu::prgs_info& lhs,
                       const tx_precoding_and_beamforming_pdu::prgs_info& rhs)
{
  return lhs.pm_index == rhs.pm_index && lhs.beam_index == rhs.beam_index;
}

static bool operator==(const tx_precoding_and_beamforming_pdu& lhs, const tx_precoding_and_beamforming_pdu& rhs)
{
  return lhs.prgs == rhs.prgs && lhs.prg_size == rhs.prg_size;
}

static bool operator==(const dl_pdcch_pdu_maintenance_v3& lhs, const dl_pdcch_pdu_maintenance_v3& rhs)
{
  // As the maintenance_info struct contains PODs, check only that the size matches.
  return lhs.pdcch_pdu_index == rhs.pdcch_pdu_index && lhs.info.size() == rhs.info.size();
}

static bool operator==(const dl_dci_pdu& lhs, const dl_dci_pdu& rhs)
{
  return lhs.context == rhs.context && lhs.precoding_and_beamforming == rhs.precoding_and_beamforming;
}

static bool operator==(const dl_pdcch_pdu& lhs, const dl_pdcch_pdu& rhs)
{
  return lhs.dl_dci == rhs.dl_dci && lhs.maintenance_v3 == rhs.maintenance_v3;
}

static bool operator==(const dl_pdsch_codeword& lhs, const dl_pdsch_codeword& rhs)
{
  return lhs.mcs_index == rhs.mcs_index && lhs.tb_size == rhs.tb_size;
}

static bool operator==(const dl_pdsch_pdu& lhs, const dl_pdsch_pdu& rhs)
{
  return lhs.context == rhs.context && lhs.cws == rhs.cws &&
         lhs.precoding_and_beamforming == rhs.precoding_and_beamforming;
}

static bool operator==(const dl_csi_rs_pdu& lhs, const dl_csi_rs_pdu& rhs)
{
  return lhs.freq_domain == rhs.freq_domain && lhs.precoding_and_beamforming == rhs.precoding_and_beamforming;
}

static bool operator==(const dl_ssb_phy_mib_pdu& lhs, const dl_ssb_phy_mib_pdu& rhs)
{
  return lhs.dmrs_typeA_position == rhs.dmrs_typeA_position && lhs.pdcch_config_sib1 == rhs.pdcch_config_sib1 &&
         lhs.cell_barred == rhs.cell_barred && lhs.intrafreq_reselection == rhs.intrafreq_reselection;
}

static bool operator==(const dl_ssb_pdu& lhs, const dl_ssb_pdu& rhs)
{
  if (!(lhs.precoding_and_beamforming == rhs.precoding_and_beamforming &&
        lhs.bch_payload_flag == rhs.bch_payload_flag)) {
    return false;
  }

  if (lhs.bch_payload_flag == bch_payload_type::phy_full) {
    return lhs.bch_payload.phy_mib_pdu == rhs.bch_payload.phy_mib_pdu;
  }

  return lhs.bch_payload.bch_payload == rhs.bch_payload.bch_payload;
}

static bool operator==(const dl_tti_request_pdu& lhs, const dl_tti_request_pdu& rhs)
{
  if (!(lhs.pdu_type == rhs.pdu_type && lhs.pdu_size == rhs.pdu_size)) {
    return false;
  }

  switch (lhs.pdu_type) {
    case dl_pdu_type::PDCCH:
      return lhs.pdcch_pdu == rhs.pdcch_pdu;
      break;
    case dl_pdu_type::PDSCH:
      return lhs.pdsch_pdu == rhs.pdsch_pdu;
      break;
    case dl_pdu_type::SSB:
      return lhs.ssb_pdu == rhs.ssb_pdu;
      break;
    case dl_pdu_type::CSI_RS:
      return lhs.csi_rs_pdu == rhs.csi_rs_pdu;
      break;
    default:
      break;
  }

  return false;
}

static bool operator==(const dl_tti_request_message& lhs, const dl_tti_request_message& rhs)
{
  return lhs.sfn == rhs.sfn && lhs.slot == rhs.slot && lhs.num_pdus_of_each_type == rhs.num_pdus_of_each_type &&
         lhs.num_groups == rhs.num_groups && lhs.is_last_message_in_slot == rhs.is_last_message_in_slot &&
         lhs.pdus == rhs.pdus;
}

static bool operator==(const ul_prach_pdu& lhs, const ul_prach_pdu& rhs)
{
  return lhs.phys_cell_id == rhs.phys_cell_id && lhs.prach_format == rhs.prach_format &&
         lhs.has_msg_a_pusch_beamforming == rhs.has_msg_a_pusch_beamforming;
}

static bool operator==(const uci_part1_to_part2_correspondence_v3::part2_info& lhs,
                       const uci_part1_to_part2_correspondence_v3::part2_info& rhs)
{
  return lhs.priority == rhs.priority && lhs.param_offsets == rhs.param_offsets && lhs.param_sizes == rhs.param_sizes &&
         lhs.part2_size_map_index == rhs.part2_size_map_index && lhs.part2_size_map_scope == rhs.part2_size_map_scope;
}

static bool operator==(const uci_part1_to_part2_correspondence_v3& lhs, const uci_part1_to_part2_correspondence_v3& rhs)
{
  return lhs.part2 == rhs.part2;
}

static bool operator==(const ul_pucch_pdu& lhs, const ul_pucch_pdu& rhs)
{
  return lhs.rnti == rhs.rnti && lhs.format_type == rhs.format_type && lhs.uci_correspondence == rhs.uci_correspondence;
}

static bool operator==(const ul_pusch_data& lhs, const ul_pusch_data& rhs)
{
  return lhs.rv_index == rhs.rv_index && lhs.new_data == rhs.new_data && lhs.tb_size == rhs.tb_size &&
         lhs.cb_present_and_position == rhs.cb_present_and_position;
}

static bool operator==(const ul_pusch_uci& lhs, const ul_pusch_uci& rhs)
{
  return lhs.harq_ack_bit_length == rhs.harq_ack_bit_length && lhs.alpha_scaling == rhs.alpha_scaling;
}

static bool operator==(const ul_pusch_pdu& lhs, const ul_pusch_pdu& rhs)
{
  return lhs.rnti == rhs.rnti && lhs.pdu_bitmap == rhs.pdu_bitmap && lhs.uci_correspondence == rhs.uci_correspondence &&
         lhs.rb_bitmap == rhs.rb_bitmap && lhs.pusch_data == rhs.pusch_data && lhs.pusch_uci == rhs.pusch_uci;
}

static bool operator==(const ul_tti_request_pdu& lhs, const ul_tti_request_pdu& rhs)
{
  if (!(lhs.pdu_type == rhs.pdu_type && lhs.pdu_size == rhs.pdu_size)) {
    return false;
  }

  switch (lhs.pdu_type) {
    case ul_pdu_type::PRACH:
      return lhs.prach_pdu == rhs.prach_pdu;
      break;
    case ul_pdu_type::PUCCH:
      return lhs.pucch_pdu == rhs.pucch_pdu;
      break;
    case ul_pdu_type::PUSCH:
      return lhs.pusch_pdu == rhs.pusch_pdu;
      break;
    default:
      break;
  }

  return false;
}

static bool operator==(const ul_tti_request_message& lhs, const ul_tti_request_message& rhs)
{
  return lhs.sfn == rhs.sfn && lhs.slot == rhs.slot && lhs.num_pdus_of_each_type == rhs.num_pdus_of_each_type &&
         lhs.num_groups == rhs.num_groups && lhs.pdus == rhs.pdus;
}

static bool operator==(const ul_dci_pdu& lhs, const ul_dci_pdu& rhs)
{
  return lhs.pdu_type == rhs.pdu_type && lhs.pdu_size == rhs.pdu_size && lhs.pdu == rhs.pdu;
}

static bool operator==(const ul_dci_request_message& lhs, const ul_dci_request_message& rhs)
{
  return lhs.sfn == rhs.sfn && lhs.slot == rhs.slot && lhs.num_pdus_of_each_type == rhs.num_pdus_of_each_type &&
         lhs.pdus == rhs.pdus && lhs.is_last_message_in_slot == rhs.is_last_message_in_slot;
}

static bool operator==(const tlv_custom_payload& lhs, const tlv_custom_payload& rhs)
{
  return lhs.length == rhs.length && lhs.payload == rhs.payload;
}

static bool operator==(const tx_data_req_pdu& lhs, const tx_data_req_pdu& rhs)
{
  return lhs.pdu_length == rhs.pdu_length && lhs.pdu_index == rhs.pdu_index && lhs.cw_index == rhs.cw_index &&
         lhs.tlv_custom == rhs.tlv_custom;
}

static bool operator==(const tx_data_request_message& lhs, const tx_data_request_message& rhs)
{
  return lhs.sfn == rhs.sfn && lhs.slot == rhs.slot && lhs.control_length == rhs.control_length && lhs.pdus == rhs.pdus;
}

} // namespace fapi
} // namespace srsran

namespace {

class slot_message_gateway_spy : public slot_message_gateway
{
  bool                    dl_tti_request_received  = false;
  bool                    ul_tti_request_received  = false;
  bool                    ul_dci_request_received  = false;
  bool                    tx_data_request_received = false;
  dl_tti_request_message  dl_tti_msg;
  ul_tti_request_message  ul_tti_msg;
  ul_dci_request_message  ul_dci_msg;
  tx_data_request_message tx_data_msg;

public:
  void dl_tti_request(const dl_tti_request_message& msg) override
  {
    dl_tti_request_received = true;
    dl_tti_msg              = msg;
  }

  void ul_tti_request(const ul_tti_request_message& msg) override
  {
    ul_tti_request_received = true;
    ul_tti_msg              = msg;
  }

  void ul_dci_request(const ul_dci_request_message& msg) override
  {
    ul_dci_request_received = true;
    ul_dci_msg              = msg;
  }

  void tx_data_request(const tx_data_request_message& msg) override
  {
    tx_data_request_received = true;
    tx_data_msg              = msg;
  }

  bool                           has_dl_tti_request_message_been_received() const { return dl_tti_request_received; }
  bool                           has_ul_tti_request_message_been_received() const { return ul_tti_request_received; }
  bool                           has_ul_dci_request_message_been_received() const { return ul_dci_request_received; }
  bool                           has_tx_data_request_message_been_received() const { return tx_data_request_received; }
  const dl_tti_request_message&  get_dl_tti_request() const { return dl_tti_msg; }
  const ul_tti_request_message&  get_ul_tti_request() const { return ul_tti_msg; }
  const ul_dci_request_message&  get_ul_dci_request() const { return ul_dci_msg; }
  const tx_data_request_message& get_tx_data_request() const { return tx_data_msg; }

  void reset()
  {
    dl_tti_request_received  = false;
    ul_tti_request_received  = false;
    ul_dci_request_received  = false;
    tx_data_request_received = false;
  }
};

} // namespace

TEST(bufferer_slot_gateway_impl_test, dl_tti_request_from_same_slot_is_transmitted)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  const dl_tti_request_message& msg  = unittest::build_valid_dl_tti_request();
  slot_point                    slot = slot_point(scs, msg.sfn, msg.slot);
  bufferer->update_current_slot(slot);
  ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());

  bufferer->handle_dl_tti_request(msg);
  bufferer->forward_cached_messages(slot);

  ASSERT_TRUE(spy.has_dl_tti_request_message_been_received());
  ASSERT_EQ(msg, spy.get_dl_tti_request());
}

TEST(bufferer_slot_gateway_impl_test, cached_slot_is_sended_when_the_current_slot_matches)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point              current_slot(scs, 10, 0);
  dl_tti_request_message  dl_tti_msg  = unittest::build_valid_dl_tti_request();
  ul_tti_request_message  ul_tti_msg  = unittest::build_valid_ul_tti_request();
  ul_dci_request_message  ul_dci_msg  = unittest::build_valid_ul_dci_request();
  tx_data_request_message tx_data_msg = unittest::build_valid_tx_data_request();

  slot_point first_message_slot = current_slot + 1;

  dl_tti_msg.sfn   = first_message_slot.sfn();
  dl_tti_msg.slot  = first_message_slot.slot_index();
  ul_tti_msg.sfn   = first_message_slot.sfn();
  ul_tti_msg.slot  = first_message_slot.slot_index();
  ul_dci_msg.sfn   = first_message_slot.sfn();
  ul_dci_msg.slot  = first_message_slot.slot_index();
  tx_data_msg.sfn  = first_message_slot.sfn();
  tx_data_msg.slot = first_message_slot.slot_index();

  bufferer->update_current_slot(current_slot);
  bufferer->forward_cached_messages(current_slot);
  bufferer->handle_dl_tti_request(dl_tti_msg);
  bufferer->handle_ul_tti_request(ul_tti_msg);
  bufferer->handle_ul_dci_request(ul_dci_msg);
  bufferer->handle_tx_data_request(tx_data_msg);

  ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());
  ASSERT_FALSE(spy.has_ul_tti_request_message_been_received());
  ASSERT_FALSE(spy.has_ul_dci_request_message_been_received());
  ASSERT_FALSE(spy.has_tx_data_request_message_been_received());

  // Send the first slot.
  bufferer->update_current_slot(first_message_slot);
  bufferer->forward_cached_messages(first_message_slot);

  ASSERT_TRUE(spy.has_dl_tti_request_message_been_received());
  ASSERT_TRUE(spy.has_ul_tti_request_message_been_received());
  ASSERT_TRUE(spy.has_ul_dci_request_message_been_received());
  ASSERT_TRUE(spy.has_tx_data_request_message_been_received());
  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());
  ASSERT_EQ(ul_tti_msg, spy.get_ul_tti_request());
  ASSERT_EQ(ul_dci_msg, spy.get_ul_dci_request());
  ASSERT_EQ(tx_data_msg, spy.get_tx_data_request());

  // Second slot.
  slot_point second_message_slot = current_slot + 2;
  dl_tti_msg.sfn                 = second_message_slot.sfn();
  dl_tti_msg.slot                = second_message_slot.slot_index();
  ul_tti_msg.sfn                 = second_message_slot.sfn();
  ul_tti_msg.slot                = second_message_slot.slot_index();
  ul_dci_msg.sfn                 = second_message_slot.sfn();
  ul_dci_msg.slot                = second_message_slot.slot_index();
  tx_data_msg.sfn                = second_message_slot.sfn();
  tx_data_msg.slot               = second_message_slot.slot_index();

  bufferer->handle_dl_tti_request(dl_tti_msg);
  bufferer->handle_ul_tti_request(ul_tti_msg);
  bufferer->handle_ul_dci_request(ul_dci_msg);
  bufferer->handle_tx_data_request(tx_data_msg);

  // Send the second slot.
  bufferer->update_current_slot(second_message_slot);
  bufferer->forward_cached_messages(second_message_slot);

  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());
  ASSERT_EQ(ul_tti_msg, spy.get_ul_tti_request());
  ASSERT_EQ(ul_dci_msg, spy.get_ul_dci_request());
  ASSERT_EQ(tx_data_msg, spy.get_tx_data_request());
}

TEST(bufferer_slot_gateway_impl_test, older_slots_than_supported_delay_are_sended)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point             current_slot(scs, 5, 0);
  dl_tti_request_message msg      = unittest::build_valid_dl_tti_request();
  slot_point             msg_slot = current_slot - 1;

  msg.sfn  = msg_slot.sfn();
  msg.slot = msg_slot.slot_index();

  bufferer->update_current_slot(current_slot);
  bufferer->handle_dl_tti_request(msg);
  bufferer->forward_cached_messages(current_slot);

  ASSERT_TRUE(spy.has_dl_tti_request_message_been_received());
  ASSERT_EQ(msg, spy.get_dl_tti_request());
}

#ifdef ASSERTS_ENABLED
TEST(bufferer_slot_gateway_impl_test, message_with_slot_bigger_than_delay_deads)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point             current_slot(scs, 5, 0);
  dl_tti_request_message msg      = unittest::build_valid_dl_tti_request();
  slot_point             msg_slot = current_slot + l2_nof_slots_ahead + 1;

  msg.sfn  = msg_slot.sfn();
  msg.slot = msg_slot.slot_index();

  bufferer->update_current_slot(current_slot);
  bufferer->forward_cached_messages(current_slot);
  bufferer->handle_dl_tti_request(msg);
}
#endif

TEST(bufferer_slot_gateway_impl_test, all_cached_slot_is_sended_when_the_current_slot_matches)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point             current_slot(scs, 10, 0);
  dl_tti_request_message dl_tti_msg = unittest::build_valid_dl_tti_request();

  slot_point first_message_slot = current_slot + 1;
  dl_tti_msg.sfn                = first_message_slot.sfn();
  dl_tti_msg.slot               = first_message_slot.slot_index();

  bufferer->update_current_slot(current_slot);
  bufferer->forward_cached_messages(current_slot);
  bufferer->handle_dl_tti_request(dl_tti_msg);

  // Second slot.
  slot_point second_message_slot = current_slot + 2;
  dl_tti_msg.sfn                 = second_message_slot.sfn();
  dl_tti_msg.slot                = second_message_slot.slot_index();

  bufferer->handle_dl_tti_request(dl_tti_msg);

  // At this point, both slots should be stored.

  ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());

  // Send the first slot.
  bufferer->update_current_slot(first_message_slot);
  bufferer->forward_cached_messages(first_message_slot);

  dl_tti_msg.sfn  = first_message_slot.sfn();
  dl_tti_msg.slot = first_message_slot.slot_index();
  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());

  // Send the second slot.
  bufferer->update_current_slot(second_message_slot);
  bufferer->forward_cached_messages(second_message_slot);

  dl_tti_msg.sfn  = second_message_slot.sfn();
  dl_tti_msg.slot = second_message_slot.slot_index();
  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());
}

TEST(bufferer_slot_gateway_impl_test, no_cached_messages_send_nothing)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point current_slot(scs, 10, 0);

  for (unsigned i = 0, e = l2_nof_slots_ahead * 2; i != e; ++i) {
    ++current_slot;
    bufferer->update_current_slot(current_slot);
    bufferer->forward_cached_messages(current_slot);

    ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());
  }
}

TEST(bufferer_slot_gateway_impl_test, sended_messages_are_removed_from_the_pool)
{
  unsigned                 l2_nof_slots_ahead = 2U;
  subcarrier_spacing       scs                = subcarrier_spacing::kHz30;
  slot_message_gateway_spy spy;

  auto bufferer = std::make_unique<buffered_slot_gateway_impl>(l2_nof_slots_ahead, scs, spy);

  slot_point             current_slot(scs, 10, 0);
  dl_tti_request_message dl_tti_msg = unittest::build_valid_dl_tti_request();

  slot_point first_message_slot = current_slot + 1;
  dl_tti_msg.sfn                = first_message_slot.sfn();
  dl_tti_msg.slot               = first_message_slot.slot_index();

  bufferer->update_current_slot(current_slot);
  bufferer->forward_cached_messages(current_slot);
  bufferer->handle_dl_tti_request(dl_tti_msg);

  // Second slot.
  slot_point second_message_slot = current_slot + 2;
  dl_tti_msg.sfn                 = second_message_slot.sfn();
  dl_tti_msg.slot                = second_message_slot.slot_index();

  bufferer->handle_dl_tti_request(dl_tti_msg);

  ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());

  // Send the first slot.
  bufferer->update_current_slot(first_message_slot);
  bufferer->forward_cached_messages(first_message_slot);

  dl_tti_msg.sfn  = first_message_slot.sfn();
  dl_tti_msg.slot = first_message_slot.slot_index();
  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());

  // Send the second slot.
  bufferer->update_current_slot(second_message_slot);
  bufferer->forward_cached_messages(second_message_slot);

  dl_tti_msg.sfn  = second_message_slot.sfn();
  dl_tti_msg.slot = second_message_slot.slot_index();
  ASSERT_EQ(dl_tti_msg, spy.get_dl_tti_request());

  // Reset the bools.
  spy.reset();

  for (unsigned i = 0, e = l2_nof_slots_ahead * 2; i != e; ++i) {
    current_slot = (i % 2) ? first_message_slot : second_message_slot;
    bufferer->update_current_slot(current_slot);
    bufferer->forward_cached_messages(current_slot);

    ASSERT_FALSE(spy.has_dl_tti_request_message_been_received());
  }
}
