/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_am_entity.h"
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
  void on_max_retx() override{};

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr) override {}
};

void test_instantiation(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter{"RLC AM ({} bit) - Instantiation", to_number(sn_size)};
  rlc_am_config       config;
  config.rx = std::make_unique<rlc_rx_am_config>(rlc_rx_am_config{sn_size, 35, 8});
  config.tx = std::make_unique<rlc_tx_am_config>(rlc_tx_am_config{sn_size, 45, 4, 4, 25});
  timer_manager  timers;
  rlc_test_frame tester1, tester2;
  rlc_am_entity  rlc1(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
  rlc_am_entity  rlc2(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

  rlc_rx_pdu_handler*     rlc1_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_sdu_handler*     rlc1_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc1_tx_lower = rlc1.get_tx_pdu_transmitter();
  rlc_rx_pdu_handler*     rlc2_rx_lower = rlc2.get_rx_pdu_handler();
  rlc_tx_sdu_handler*     rlc2_tx_upper = rlc1.get_tx_sdu_handler();
  rlc_tx_pdu_transmitter* rlc2_tx_lower = rlc2.get_tx_pdu_transmitter();

  (void)rlc1_rx_lower;
  (void)rlc1_tx_upper;
  (void)rlc2_rx_lower;
  (void)rlc2_tx_upper;

  uint32_t buffer_state = 0;

  buffer_state = rlc1_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);
  buffer_state = rlc2_tx_lower->get_buffer_state();
  TESTASSERT_EQ(0, buffer_state);
}
} // namespace srsgnb

int main()
{
  srslog::init();
  srslog::fetch_basic_logger("TEST", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(100);
  fprintf(stdout, "Testing RLC AM\n");
  std::initializer_list<srsgnb::rlc_am_sn_size> sn_sizes = {srsgnb::rlc_am_sn_size::size12bits,
                                                            srsgnb::rlc_am_sn_size::size18bits};
  for (srsgnb::rlc_am_sn_size sn_size : sn_sizes) {
    srsgnb::test_instantiation(sn_size);
  }
}
