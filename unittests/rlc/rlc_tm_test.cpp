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

namespace srsgnb {

template <std::size_t N>
byte_buffer make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer pdu;
  pdu.append(tv);
  // write_pdu_to_pcap(4, tv.data(), tv.size()); TODO
  return pdu;
}

class rlc_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_tx_upper_layer_control_notifier
{
public:
  rlc_sdu_queue sdu_queue;
  uint32_t      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer pdu) override
  {
    rlc_sdu boxed_sdu(sdu_counter++, std::move(pdu));
    sdu_queue.write(boxed_sdu);
  }
  void on_ack_received() override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override{};
};

void test_rx()
{
  test_delimit_logger delimiter{"RLC TM - Rx SDUs"};
  rlc_test_frame      tester;
  rlc_tm_entity       rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, tester, tester);

  rlc_rx_pdu_handler* rlc1_rx_lower = rlc1.get_rx_pdu_handler();

  const int                              payload_len = 4;
  const std::array<uint8_t, payload_len> tv_sdu      = {0xc0, 0xca, 0xc0, 0x1a};
  const std::array<uint8_t, payload_len> tv_pdu      = {0xc0, 0xca, 0xc0, 0x1a};
  byte_buffer                            sdu         = make_pdu_and_log(tv_sdu);
  byte_buffer                            pdu         = make_pdu_and_log(tv_pdu);

  // write PDU into lower end
  rlc1_rx_lower->handle_pdu(std::move(pdu));

  // read cached SDU from tester
  TESTASSERT(tester.sdu_queue.is_empty() == false);
  rlc_sdu boxed_sdu = {};
  TESTASSERT(tester.sdu_queue.read(boxed_sdu) == true);

  TESTASSERT(boxed_sdu.buf == sdu);
  TESTASSERT(boxed_sdu.pdcp_sn == 0);
}

void test_tx()
{
  test_delimit_logger delimiter{"RLC TM - Tx PDUs"};
  rlc_test_frame      tester;
  rlc_tm_entity       rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, tester, tester);

  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();

  const int                              payload_len  = 4;
  const std::array<uint8_t, payload_len> tv_sdu       = {0xc0, 0xca, 0xc0, 0x1a};
  const std::array<uint8_t, payload_len> tv_pdu       = {0xc0, 0xca, 0xc0, 0x1a};
  const std::array<uint8_t, payload_len> tv_pdu_relic = {0xde, 0xad, 0xde, 0xad};

  {
    // write SDU into upper end
    byte_buffer sdu       = make_pdu_and_log(tv_sdu);
    rlc_sdu     boxed_sdu = {1000, std::move(sdu)};
    rlc1_tx_upper->handle_sdu(std::move(boxed_sdu));
  }

  {
    // read PDU from lower end
    byte_buffer pdu = make_pdu_and_log(tv_pdu_relic);
    TESTASSERT(rlc1_tx_lower->pull_pdu(pdu, payload_len) == true);
    TESTASSERT(pdu == tv_pdu);
  }

  {
    // read another PDU from lower end but there is nothing to read
    byte_buffer pdu = make_pdu_and_log(tv_pdu_relic);
    TESTASSERT(rlc1_tx_lower->pull_pdu(pdu, payload_len) == false);
    TESTASSERT(pdu == tv_pdu_relic);
  }

  {
    // write another SDU into upper end
    byte_buffer sdu       = make_pdu_and_log(tv_sdu);
    rlc_sdu     boxed_sdu = {1001, std::move(sdu)};
    rlc1_tx_upper->handle_sdu(std::move(boxed_sdu));
  }

  {
    // read PDU from lower end with insufficient space for the whole SDU
    byte_buffer pdu      = make_pdu_and_log(tv_pdu_relic);
    const int   shortage = 1;
    TESTASSERT(rlc1_tx_lower->pull_pdu(pdu, payload_len - shortage) == false);
    TESTASSERT(pdu == tv_pdu_relic);
  }

  {
    // write another SDU into upper end
    byte_buffer sdu       = make_pdu_and_log(tv_sdu);
    rlc_sdu     boxed_sdu = {1002, std::move(sdu)};
    rlc1_tx_upper->handle_sdu(std::move(boxed_sdu));
  }

  {
    // read PDU from lower end with oversized space
    byte_buffer pdu      = make_pdu_and_log(tv_pdu_relic);
    const int   oversize = 10;
    TESTASSERT(rlc1_tx_lower->pull_pdu(pdu, payload_len + oversize) == true);
    TESTASSERT(pdu.length() == payload_len);
    TESTASSERT(pdu == tv_pdu);
  }

  {
    // read another PDU from lower end but there is nothing to read
    byte_buffer pdu = make_pdu_and_log(tv_pdu_relic);
    TESTASSERT(rlc1_tx_lower->pull_pdu(pdu, payload_len) == false);
    TESTASSERT(pdu == tv_pdu_relic);
  }
}

} // namespace srsgnb

int main()
{
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  logger.info("Testing RLC TM");
  srslog::flush();

  srsgnb::test_rx();
  srsgnb::test_tx();
}
