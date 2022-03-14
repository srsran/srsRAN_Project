
#include "../../lib/mac/mac_ul/mac_ul_sch_pdu.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_mac_ul_subpdu()
{
  test_delimit_logger delim{"subPDU test"};
  mac_ul_sch_subpdu subpdu;

  std::vector<uint8_t> msg({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6});
  TESTASSERT(subpdu.unpack(msg) >= 0);
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_pdu()
{
  test_delimit_logger delim{"UL PDU test"};
  mac_ul_sch_pdu pdu;

  std::vector<uint8_t> msg({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
  TESTASSERT(pdu.unpack(msg) >= 0);
  fmt::print("PDU:\n  Hex={:X}\n  subPDUs: {}\n", fmt::join(msg.begin(), msg.end(), ""), pdu);
}

int main()
{
  test_mac_ul_subpdu();
  test_mac_ul_pdu();
}