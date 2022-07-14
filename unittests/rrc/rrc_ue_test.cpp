/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/cu_cp/srb_pdu_notifier.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_factory.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

class rrc_entity_dummy : public rrc_entity_ue_interface
{
public:
  rrc_entity_dummy() = default;

  int  get_pucch_resources() override { return 0; }
  bool is_amf_connected() override { return false; }
};

class dummy_tx_pdu_handler
{
public:
  dummy_tx_pdu_handler() = default;
  void handle_pdu(byte_buffer_slice pdu) { last_pdu = std::move(pdu); }

  byte_buffer_slice last_pdu;
};

class dummy_adapter : public srb_pdu_notifier
{
public:
  dummy_adapter(dummy_tx_pdu_handler& handler_) : handler(handler_) {}

  void on_new_pdu(byte_buffer_slice pdu) override { handler.handle_pdu(std::move(pdu)); }

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer_slice pdu({rrc_container.begin(), rrc_container.end()});
    handler.handle_pdu(std::move(pdu));
  }

private:
  dummy_tx_pdu_handler& handler;
};

// When a RRC setup request PDU is received on UL-CCCH, a RRC setup PDU is generated and sent down to the UE.
void rrc_setup_test()
{
  uint8_t rrc_setup_pdu[] = {0x1d, 0xec, 0x89, 0xd0, 0x57, 0x66};

  // create RRC entity
  rrc_cfg_t                                cfg{}; // empty config
  rrc_entity_creation_message              msg(cfg);
  std::unique_ptr<rrc_entity_du_interface> rrc = srsgnb::srs_cu_cp::create_rrc_entity(msg);

  // create single UE context and add RRC user
  ue_context ue_ctxt{};
  ue_ctxt.c_rnti = to_rnti(0x1234);
  ue_ctxt.rrc    = rrc->add_user(ue_ctxt);

  // Object to handle the generated RRC message
  dummy_tx_pdu_handler tx_pdu_handler;

  // create SRB0 with RRC to "F1" adapter
  ue_ctxt.srbs.emplace(LCID_SRB0);
  cu_srb_context& srb0 = ue_ctxt.srbs[LCID_SRB0];
  srb0.lcid            = LCID_SRB0;
  srb0.tx_notifier     = std::make_unique<dummy_adapter>(tx_pdu_handler);

  // inject RRC setup into UE object
  ue_ctxt.rrc->get_ul_ccch_pdu_handler()->handle_ul_ccch_pdu(byte_buffer{rrc_setup_pdu});

  TESTASSERT(tx_pdu_handler.last_pdu.length() > 0); // check PDU is not empty

  // Unpack received PDU
  byte_buffer                 rx_pdu{tx_pdu_handler.last_pdu.begin(), tx_pdu_handler.last_pdu.end()};
  asn1::cbit_ref              bref(rx_pdu);
  asn1::rrc_nr::dl_ccch_msg_s dl_ccch;
  TESTASSERT(dl_ccch.unpack(bref) == asn1::SRSASN_SUCCESS);
}

int main()
{
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  rrc_setup_test();

  srslog::flush();
}
