/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_tm_entity.h"
#include "srsgnb/support/test_utils.h"
#include <queue>

namespace srsgnb {

template <std::size_t N>
byte_buffer make_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  return byte_buffer{tv};
}

template <std::size_t N>
byte_buffer_slice_chain make_rlc_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer             buf = {tv};
  byte_buffer_slice_chain pdu;
  pdu.push_back(std::move(buf));
  return pdu;
}

class rlc_test_frame : public rlc_rx_upper_data_notifier,
                       public rlc_tx_upper_layer_data_notifier,
                       public rlc_tx_upper_layer_control_notifier,
                       public rlc_tx_lower_notifier
{
public:
  std::queue<byte_buffer_slice> sdu_queue;
  uint32_t                      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  // rlc_tx_upper_layer_data_notifier interface
  void on_delivered_sdu(uint32_t pdcp_count) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr) override {}
};

void test_rx()
{
  test_delimit_logger delimiter{"RLC TM - Rx SDUs"};
  rlc_test_frame      tester;
  rlc_tm_entity       rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, tester, tester, tester, tester);

  rlc_rx_lower_interface* rlc1_rx_lower = rlc1.get_rx_lower_interface();

  const int                              payload_len = 4;
  const std::array<uint8_t, payload_len> tv_sdu      = {0xc0, 0xca, 0xc0, 0x1a};
  const std::array<uint8_t, payload_len> tv_pdu      = {0xc0, 0xca, 0xc0, 0x1a};
  byte_buffer                            sdu         = make_byte_buffer_and_log(tv_sdu);
  byte_buffer                            pdu         = make_byte_buffer_and_log(tv_pdu);

  // write PDU into lower end
  rlc1_rx_lower->handle_pdu(byte_buffer_slice{std::move(pdu)});

  // read cached SDU from tester
  TESTASSERT(tester.sdu_queue.empty() == false);
  byte_buffer_slice rx_sdu = tester.sdu_queue.front();

  TESTASSERT(rx_sdu == sdu);
  TESTASSERT(tester.sdu_counter == 1);
}

void test_tx()
{
  test_delimit_logger delimiter{"RLC TM - Tx PDUs"};
  rlc_test_frame      tester;
  rlc_tm_entity       rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, tester, tester, tester, tester);

  rlc_tx_upper_data_interface* rlc1_tx_upper = rlc1.get_tx_upper_data_interface();
  rlc_tx_lower_interface*      rlc1_tx_lower = rlc1.get_tx_lower_interface();

  uint32_t buffer_state = 0;

  const int                              payload_len = 4;
  const std::array<uint8_t, payload_len> tv_sdu      = {0xc0, 0xca, 0xc0, 0x1a};
  const std::array<uint8_t, payload_len> tv_pdu      = {0xc0, 0xca, 0xc0, 0x1a};

  {
    // write SDU into upper end
    rlc_sdu sdu = {0, make_byte_buffer_and_log(tv_sdu)};
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(payload_len, buffer_state);

  {
    // read PDU from lower end
    byte_buffer_slice_chain pdu = rlc1_tx_lower->pull_pdu(payload_len);
    TESTASSERT(not pdu.empty());
    TESTASSERT(pdu == tv_pdu);
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  {
    // read another PDU from lower end but there is nothing to read
    byte_buffer_slice_chain pdu = rlc1_tx_lower->pull_pdu(payload_len);
    TESTASSERT(pdu.empty());
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);

  {
    // write another SDU into upper end
    rlc_sdu sdu = {1, make_byte_buffer_and_log(tv_sdu)};
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(payload_len, buffer_state);

  {
    // read PDU from lower end with insufficient space for the whole SDU
    const int               shortage = 1;
    byte_buffer_slice_chain pdu      = rlc1_tx_lower->pull_pdu(payload_len - shortage);
    TESTASSERT(pdu.empty());
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(payload_len, buffer_state);

  {
    // write another SDU into upper end
    // there should now be two SDUs in the queue
    rlc_sdu sdu = {2, make_byte_buffer_and_log(tv_sdu)};
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(2 * payload_len, buffer_state);

  {
    // read PDU from lower end with oversized space
    const int               oversize = 10;
    byte_buffer_slice_chain pdu      = rlc1_tx_lower->pull_pdu(payload_len + oversize);
    TESTASSERT_EQ(pdu.length(), payload_len);
    TESTASSERT(pdu == tv_pdu);
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(payload_len, buffer_state);

  {
    // read another PDU from lower end. There should still be one SDU in the queue
    byte_buffer_slice_chain pdu = rlc1_tx_lower->pull_pdu(payload_len);
    TESTASSERT_EQ(pdu.length(), payload_len);
  }
  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);
}

} // namespace srsgnb

int main()
{
  srslog::init();
  srslog::fetch_basic_logger("TEST", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);
  fprintf(stdout, "Testing RLC TM\n");

  srsgnb::test_rx();
  srsgnb::test_tx();
}
