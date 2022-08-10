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
byte_buffer_slice_chain make_rlc_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer             buf = {tv};
  byte_buffer_slice_chain pdu;
  pdu.push_back(std::move(buf));
  return pdu;
}

class rlc_test_frame : public rlc_rx_upper_layer_data_notifier,
                       public rlc_tx_upper_layer_control_notifier,
                       public rlc_tx_buffer_state_update_notifier
{
public:
  rlc_sdu_queue sdu_queue;
  uint32_t      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice sdu) override
  {
    rlc_sdu boxed_sdu(sdu_counter++, std::move(sdu));
    sdu_queue.write(boxed_sdu);
  }
  void on_ack_received() override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr) override {}
};

void test_full_sdus(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - Full SDUs, no Segmentation", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 5;
  const uint32_t num_pdus = 5;
  const uint32_t sdu_size = 1;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1
  byte_buffer_slice_chain pdu_bufs[num_pdus];
  const int               payload_len = 1 + sdu_size; // 1 bytes for header + payload
  for (uint32_t i = 0; i < num_pdus; i++) {
    pdu_bufs[i] = rlc1_tx_lower->pull_pdu(payload_len);
    TESTASSERT_EQ(payload_len, pdu_bufs[i].length());
    TESTASSERT_EQ(i, pdu_bufs[i][payload_len - 1]); // check if last payload item corresponds with index

    // TODO: write PCAP
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Read SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }
}

void test_segmented_sdu(rlc_um_sn_size sn_size, bool reverse_rx = false)
{
  test_delimit_logger delimiter{"RLC UM ({}) - Segmented SDU{}", sn_size, reverse_rx ? ", reverse rx" : ""};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t          payload_len  = 25;
  const uint32_t          max_num_pdus = 10;
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  buffer_state = rlc1_tx_lower->get_buffer_state();
  while (buffer_state > 0 && num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
    buffer_state = rlc1_tx_lower->get_buffer_state();
  }
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2
  if (reverse_rx) {
    // receive PDUs in reverse order
    for (uint32_t i = num_pdus; i > 0; i--) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  } else {
    // receive PDUs in order
    for (uint32_t i = 0; i < num_pdus; i++) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  }
  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Read SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }
}

void test_multiple_segmented_sdus(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - Multiple segmented SDUs", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 2;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t          payload_len  = 25;
  const uint32_t          max_num_pdus = 20;
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  buffer_state = rlc1_tx_lower->get_buffer_state();
  while (buffer_state > 0 && num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
    buffer_state = rlc1_tx_lower->get_buffer_state();
  }
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2 (except 1 and 6)
  for (uint32_t i = 0; i < num_pdus; i++) {
    if (i != 1 && i != 6) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  }

  // write remaining two PDUs in reverse-order (so SN=1 is received first)
  {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[6].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[1].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }

  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Read SDUs from RLC2's upper layer
  // Upper layer receives SDUs in order of completion (i.e. here in reverse order)
  // Therefore compare with initial testvectors in reverse order
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
    TESTASSERT(sdu_bufs[num_sdus - 1 - i] == boxed_sdu.buf);
  }
}

void test_segmented_sdu_with_pdu_duplicates(rlc_um_sn_size sn_size, const uint32_t last_sn)
{
  test_delimit_logger delimiter{"RLC UM ({}) - Segmented SDU with PDU duplicates (last_sn={})", sn_size, last_sn};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t          payload_len  = 25;
  const uint32_t          max_num_pdus = 10;
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  buffer_state = rlc1_tx_lower->get_buffer_state();
  while (buffer_state > 0 && num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
    buffer_state = rlc1_tx_lower->get_buffer_state();
  }
  TESTASSERT_EQ(0, buffer_state);

  // Write all PDUs twice into RLC2 (except for last_sn)
  for (uint32_t k = 0; k < 2; k++) {
    for (uint32_t i = 0; i < num_pdus; i++) {
      if (i != last_sn) {
        byte_buffer pdu;
        for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
          pdu.append(slice);
        }
        rlc2_rx_lower->handle_pdu(std::move(pdu));
      }
    }
  }
  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Write the skipped PDU into RLC2
  {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[last_sn].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }

  // Read SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }
}

void test_reassembly_window_wrap_around(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - reassembly window wrap-around", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = cardinality(to_number(sn_size)); // 2 * UM_Window_Size
  const uint32_t sdu_size = 10;

  const uint32_t          payload_len  = 8;
  const uint32_t          max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  // Push SDUs into RLC1 and read PDUs from RLC1 with grant of 8 Bytes each
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));

    // check buffer state
    buffer_state = rlc1_tx_lower->get_buffer_state();
    TESTASSERT_EQ(sdu_size + 1, buffer_state);

    // read PDUs from lower end
    while (buffer_state > 0 && num_pdus < max_num_pdus) {
      pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

      if (pdu_bufs[num_pdus].empty()) {
        break;
      }
      // TODO: write PCAP
      num_pdus++;
      buffer_state = rlc1_tx_lower->get_buffer_state();
    }
  }

  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2 and read SDUs from RLC2's upper layer
  uint32_t rx_sdu_idx = 0;
  for (uint32_t i = 0; i < num_pdus; i++) {
    // Prepare and write PDU
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));

    // Read SDU and check content
    while (!tester2.sdu_queue.is_empty() && rx_sdu_idx < num_sdus) {
      rlc_sdu boxed_sdu = {};
      TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
      TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
      TESTASSERT(sdu_bufs[rx_sdu_idx] == boxed_sdu.buf);
      rx_sdu_idx++;
    }
  }

  // Check number of received SDUs
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);

  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);
}

void test_lost_pdu_outside_reassembly_window(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - lost PDU outside reassembly window", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = cardinality(to_number(sn_size)); // 2 * UM_Window_Size
  const uint32_t sdu_size = 10;

  const uint32_t          payload_len  = 8;
  const uint32_t          max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  // Push SDUs into RLC1 and read PDUs from RLC1 with grant of 8 Bytes each
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));

    // check buffer state
    buffer_state = rlc1_tx_lower->get_buffer_state();
    TESTASSERT_EQ(sdu_size + 1, buffer_state);

    // read PDUs from lower end
    while (buffer_state > 0 && num_pdus < max_num_pdus) {
      pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

      if (pdu_bufs[num_pdus].empty()) {
        break;
      }
      // TODO: write PCAP
      num_pdus++;
      buffer_state = rlc1_tx_lower->get_buffer_state();
    }
  }

  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2 (except 11th) and read SDUs from RLC2's upper layer
  uint32_t rx_sdu_idx = 0;
  for (uint32_t i = 0; i < num_pdus; i++) {
    if (i != 10) {
      // Prepare and write PDU
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    } else {
      // increment rx_sdu_idx to skip content check of the SDU that will be lost
      rx_sdu_idx++;
    }

    // Read SDU and check content
    while (!tester2.sdu_queue.is_empty() && rx_sdu_idx < num_sdus) {
      rlc_sdu boxed_sdu = {};
      TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
      TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
      TESTASSERT(sdu_bufs[rx_sdu_idx] == boxed_sdu.buf);
      rx_sdu_idx++;
    }
  }

  // Check number of received SDUs
  TESTASSERT_EQ(num_sdus - 1, tester2.sdu_counter);

  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // let t-reassembly expire
  while (timers.nof_running_timers() != 0) {
    timers.tick_all();
  }

  rlc_bearer_metrics_container rlc2_metrics = rlc2.get_metrics();
  TESTASSERT(rlc2_metrics.rx.num_lost_pdus == 1);
}

void test_lost_segment_outside_reassembly_window(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - lost segment outside reassembly window", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 10;
  const uint32_t sdu_size = 10;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1 with grant of 8 Bytes each
  const uint32_t          payload_len  = 8;
  const uint32_t          max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  buffer_state = rlc1_tx_lower->get_buffer_state();
  while (buffer_state > 0 && num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
    buffer_state = rlc1_tx_lower->get_buffer_state();
  }
  TESTASSERT_EQ(0, buffer_state);

  // Write PDUs into RLC2 (except 2nd)
  for (uint32_t i = 0; i < num_pdus; i++) {
    if (i != 2) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  }

  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // let t-reassembly expire
  while (timers.nof_running_timers() != 0) {
    timers.tick_all();
  }

  // Read SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus - 1, tester2.sdu_counter);
  for (uint32_t i = 0; i < tester2.sdu_counter; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
  }

  rlc_bearer_metrics_container rlc2_metrics = rlc2.get_metrics();
  TESTASSERT(rlc2_metrics.rx.num_lost_pdus == 1);
}

void test_out_of_order_segments_across_sdus(rlc_um_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC UM ({}) - out-of-order segments across different SDUs", sn_size};
  rlc_um_config       config;
  config.rx = std::make_unique<rlc_rx_um_config>(rlc_rx_um_config{sn_size, 5});
  config.tx = std::make_unique<rlc_tx_um_config>(rlc_tx_um_config{sn_size});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_um_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_um_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  uint32_t buffer_state = 0;

  const uint32_t num_sdus = 2;
  const uint32_t sdu_size = 20;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i].append(i + k);
    }

    // write SDU into upper end
    rlc_sdu sdu = {0, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(num_sdus * (sdu_size + 1), buffer_state);

  // Read PDUs from RLC1 with grant smaller than SDU size
  const uint32_t          payload_len  = 10;
  const uint32_t          max_num_pdus = 10;
  uint32_t                num_pdus     = 0;
  byte_buffer_slice_chain pdu_bufs[max_num_pdus];

  buffer_state = rlc1_tx_lower->get_buffer_state();
  while (buffer_state > 0 && num_pdus < max_num_pdus) {
    pdu_bufs[num_pdus] = rlc1_tx_lower->pull_pdu(payload_len);

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
    buffer_state = rlc1_tx_lower->get_buffer_state();
  }
  TESTASSERT_EQ(6, num_pdus);
  TESTASSERT_EQ(0, buffer_state);

  // Write all PDUs such that the middle section of SN=0 is received after the start section of SN=1
  //                    +------------------- skip 2nd PDU which is the middle section of SN=0
  //                    |      +------------ now feed the middle part of SN=0
  //                    |      |  +--------- and the rest of SN=1
  //                    V      V  V
  uint32_t order[] = {0, 2, 3, 1, 4, 5};

  // Write PDUs into RLC2 (except 2nd)
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[order[i]].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  // Read SDUs from RLC2's upper layer
  TESTASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    rlc_sdu boxed_sdu = {};
    TESTASSERT_EQ(true, tester2.sdu_queue.read(boxed_sdu));
    TESTASSERT_EQ(sdu_size, boxed_sdu.buf.length());
    TESTASSERT(sdu_bufs[i] == boxed_sdu.buf);
  }

  rlc_bearer_metrics_container rlc2_metrics = rlc2.get_metrics();
  TESTASSERT(rlc2_metrics.rx.num_lost_pdus == 0);

  TESTASSERT_EQ(0, timers.nof_running_timers());
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
    srsgnb::test_full_sdus(sn_size);
    srsgnb::test_segmented_sdu(sn_size, /* reverse_rx = */ false);
    srsgnb::test_multiple_segmented_sdus(sn_size);
    for (uint32_t last_sn = 0; last_sn < 5; last_sn++) {
      srsgnb::test_segmented_sdu_with_pdu_duplicates(sn_size, last_sn);
    }
    srsgnb::test_reassembly_window_wrap_around(sn_size);
    srsgnb::test_lost_pdu_outside_reassembly_window(sn_size);
    srsgnb::test_lost_segment_outside_reassembly_window(sn_size);
    srsgnb::test_out_of_order_segments_across_sdus(sn_size);
  }
}
