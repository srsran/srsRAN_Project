/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_um_entity.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

template <std::size_t N>
byte_buffer make_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer sdu = {tv};
  return sdu;
}

template <std::size_t N>
rlc_byte_buffer make_rlc_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer     buf = {tv};
  rlc_byte_buffer pdu;
  pdu.set_payload(buf);
  return pdu;
}

class rlc_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_tx_upper_layer_control_notifier
{
public:
  rlc_sdu_queue sdu_queue;
  uint32_t      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(shared_byte_buffer_view sdu) override
  {
    rlc_sdu boxed_sdu(sdu_counter++, std::move(sdu));
    sdu_queue.write(boxed_sdu);
  }
  void on_ack_received() override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override{};
};

void test_no_segmentation(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({} bit) - No Segmentation", to_number(sn_size)};
  rlc_um_config       config = {sn_size, 5, 0};
  timer_manager       timers;
  rlc_test_frame      tester1, tester2;
  rlc_um_entity       rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, timers);
  rlc_um_entity       rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper  = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower  = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc_2_rx_lower = rlc2.get_rx_pdu_handler();

  const uint32_t num_sdus = 5;
  const uint32_t num_pdus = 5;
  const uint32_t sdu_size = 1;

  // Push 5 SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // sdu_bufs[i].append(i); // write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  // TESTASSERT(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1
  rlc_byte_buffer pdu_bufs[num_pdus];
  const int       payload_len = 1 + sdu_size; // 1 bytes for header + payload
  for (uint32_t i = 0; i < num_pdus; i++) {
    pdu_bufs[i] = rlc1_tx_lower->pull_pdu(payload_len);
    TESTASSERT_EQ(payload_len, pdu_bufs[i].length());
    TESTASSERT_EQ(i, pdu_bufs[i][payload_len - 1]); // check if last payload item corresponds with index

    // TODO: write PCAP
  }
  // TESTASSERT(0 == rlc1.get_buffer_state());

  // Write 5 PDUs into RLC2
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    pdu.append(pdu_bufs[i].header_view());
    pdu.append(pdu_bufs[i].payload_view());
    rlc_2_rx_lower->handle_pdu(std::move(pdu));
  }
  // TESTASSERT(0 == rlc2.get_buffer_state());

  // Read 5 SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }
}

void test_with_segmentation(rlc_um_sn_size sn_size, bool reverse_rx = false)
{
  test_delimit_logger delimiter{
      "RLC UM ({} bit) - With Segmentation{}", to_number(sn_size), reverse_rx ? ", reverse rx" : ""};
  rlc_um_config  config = {sn_size, 5, 0};
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper  = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower  = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc_2_rx_lower = rlc2.get_rx_pdu_handler();

  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1

  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  // TESTASSERT(103 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t  payload_len  = 25;
  const uint32_t  max_num_pdus = 10;
  uint32_t        num_pdus     = 0;
  rlc_byte_buffer pdu_bufs[max_num_pdus];

  // TODO: quit loop when 0 == rlc1.get_buffer_state()
  while (num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  // TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2
  if (reverse_rx) {
    // receive PDUs in reverse order
    for (uint32_t i = num_pdus; i > 0; i--) {
      byte_buffer pdu;
      pdu.append(pdu_bufs[i - 1].header_view());
      pdu.append(pdu_bufs[i - 1].payload_view());
      rlc_2_rx_lower->handle_pdu(std::move(pdu));
    }
  } else {
    // receive PDUs in order
    for (uint32_t i = 0; i < num_pdus; i++) {
      byte_buffer pdu;
      pdu.append(pdu_bufs[i].header_view());
      pdu.append(pdu_bufs[i].payload_view());
      rlc_2_rx_lower->handle_pdu(std::move(pdu));
    }
  }
  // TESTASSERT(0 == rlc2.get_buffer_state());

  // Read 5 SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }
}

} // namespace srsgnb

int main()
{
  srslog::init();
  srslog::fetch_basic_logger("TEST", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);
  fprintf(stdout, "Testing RLC UM\n");
  std::initializer_list<srsgnb::rlc_um_sn_size> sn_sizes = {srsgnb::rlc_um_sn_size::size6bits,
                                                            srsgnb::rlc_um_sn_size::size12bits};
  for (auto sn_size : sn_sizes) {
    srsgnb::test_no_segmentation(sn_size);
    srsgnb::test_with_segmentation(sn_size, /* reverse_rx = */ false);
    srsgnb::test_with_segmentation(sn_size, /* reverse_rx = */ true);
  }
}
