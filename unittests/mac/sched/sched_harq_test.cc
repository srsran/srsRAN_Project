
#include "../../../lib/mac/sched/sched_harq.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

static inline bool next_ndi(bool latest_ndi, bool new_tx)
{
  return new_tx ? not latest_ndi : latest_ndi;
}

void test_harq()
{
  // Test parameters
  constexpr uint32_t max_n_rtx      = 1;
  constexpr uint32_t nof_h_procs    = 4;
  constexpr uint32_t arq_slot_delay = 4;
  constexpr uint32_t ul_tx_delay    = 4;

  // create dl HARQ process
  dl_harq_proc dl_h{0, 52};
  TESTASSERT(dl_h.empty());

  // create ul HARQ process
  ul_harq_proc ul_h{0, 52};
  TESTASSERT(ul_h.empty());

  // Initialize logger
  srslog::init();
  auto& harq_logger = srslog::fetch_basic_logger("MAC-NR");
  harq_logger.set_level(srslog::basic_levels::info);

  // Pre-define variables for HARQ processes
  const int mcs_dl_0 = 17, mcs_dl_1 = 13, mcs_dl_2 = 11, mcs_dl_3 = 8;
  const int mcs_ul_0 = 12, mcs_ul_1 = 7, mcs_ul_2 = 8, mcs_ul_3 = 23;
  // TBS size does not really matter in this test, any value will do
  const uint32_t tbs_dl_0 = 3152, tbs_dl_1 = 2184, tbs_dl_2 = 1918, tbs_dl_3 = 1224;
  const uint32_t tbs_ul_0 = 3152, tbs_ul_1 = 2184, tbs_ul_2 = 1616, tbs_ul_3 = 4578;
  // Save latest per-HARQ process NDI
  bool latest_dl_ndi[nof_h_procs]{false, false, false, false};
  bool latest_ul_ndi[nof_h_procs]{false, false, false, false};
  // Temp DCI that will be reused by all processes (we use this to check the output of new_tx and new_retx function)
  struct dci_dl_t dci_dl;
  struct dci_ul_t dci_ul;

  // Generate: (i) RGB bitmap to be reused by all HARQ processes; and (ii) the prb_grant for DL and UL
  rbg_bitmap rgb_btmp(18);
  rgb_btmp.set(1);
  rgb_btmp.set(5);
  rgb_btmp.set(9);
  rgb_btmp.set(13);
  rgb_btmp.set(17);
  prb_grant grant_dl{rgb_btmp}, grant_ul{rgb_btmp};

  // Create HARQ entity
  harq_entity h_entity = harq_entity(0x04601, 52, nof_h_procs, harq_logger);

  // ----------------  SLOT 0  ------------------
  // Update slot
  uint32_t t = 0;
  h_entity.new_slot(t);

  // DL : Create HARQ Proc ID 0
  dl_harq_proc* dl_harq_pic_0 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_0 != nullptr);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_0->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_0, max_n_rtx, dci_dl);
  dl_harq_pic_0->set_tbs(tbs_dl_0);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_dl.mcs == mcs_dl_0 and dci_dl.ndi == true and dci_dl.pid == 0);
  TESTASSERT(dl_harq_pic_0->mcs() == mcs_dl_0 and dl_harq_pic_0->tbs() == tbs_dl_0 and dl_harq_pic_0->ndi() == true);
  latest_dl_ndi[0] = dl_harq_pic_0->ndi();

  // UL: Create HARQ Proc ID 0
  ul_harq_proc* ul_harq_pic_0 = h_entity.find_empty_ul_harq();
  TESTASSERT(ul_harq_pic_0 != nullptr);

  // Create new transmission and set MCS + TBS
  // NOTE: UL HARQ is scheduled to be TXed at slot t + ul_tx_delay
  ul_harq_pic_0->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_0, max_n_rtx, dci_ul);
  ul_harq_pic_0->set_tbs(tbs_ul_0);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_0 and dci_ul.ndi == true and dci_ul.pid == 0);
  TESTASSERT(ul_harq_pic_0->mcs() == mcs_ul_0 and ul_harq_pic_0->tbs() == tbs_ul_0 and ul_harq_pic_0->ndi() == true);
  TESTASSERT(ul_harq_pic_0->get_softbuffer().size() == tbs_ul_0);
  latest_ul_ndi[0] = ul_harq_pic_0->ndi();

  // ----------------  SLOT 1  ------------------
  // Update slot
  h_entity.new_slot(++t);

  // DL: Create HARQ Proc ID 1
  dl_harq_proc* dl_harq_pic_1 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_1 != nullptr);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_1->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_1, max_n_rtx, dci_dl);
  dl_harq_pic_1->set_tbs(tbs_dl_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_1 and dci_dl.ndi == true and dci_dl.pid == 1);
  TESTASSERT(dl_harq_pic_1->mcs() == mcs_dl_1 and dl_harq_pic_1->tbs() == tbs_dl_1 and dl_harq_pic_1->ndi() == true);
  latest_dl_ndi[1] = dl_harq_pic_1->ndi();

  // UL: Create HARQ Proc ID 1
  ul_harq_proc* ul_harq_pic_1 = h_entity.find_empty_ul_harq();
  TESTASSERT(ul_harq_pic_1 != nullptr);

  // Create new transmission and set MCS + TBS
  // NOTE: UL HARQ is scheduled to be TXed at slot t + ul_tx_delay
  ul_harq_pic_1->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_1, max_n_rtx, dci_ul);
  ul_harq_pic_1->set_tbs(tbs_ul_1);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_1 and dci_ul.ndi == true and dci_ul.pid == 1);
  TESTASSERT(ul_harq_pic_1->mcs() == mcs_ul_1 and ul_harq_pic_1->tbs() == tbs_ul_1 and ul_harq_pic_1->ndi() == true);
  TESTASSERT(ul_harq_pic_1->get_softbuffer().size() == tbs_ul_1);
  latest_ul_ndi[1] = ul_harq_pic_1->ndi();

  // ----------------  SLOT 2  ------------------
  // Update slot
  h_entity.new_slot(++t);

  // DL : Create HARQ Proc ID 2
  dl_harq_proc* dl_harq_pic_2 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_2 != nullptr);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_2->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_2, max_n_rtx, dci_dl);
  dl_harq_pic_2->set_tbs(tbs_dl_2);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_2 and dci_dl.ndi == true and dci_dl.pid == 2);
  TESTASSERT(dl_harq_pic_2->mcs() == mcs_dl_2 and dl_harq_pic_2->tbs() == tbs_dl_2 and dl_harq_pic_2->ndi() == true);
  latest_dl_ndi[2] = dl_harq_pic_2->ndi();

  // UL: Create HARQ Proc ID 2
  // NOTE: UL HARQ is scheduled to be TXed at slot t + ul_tx_delay
  ul_harq_proc* ul_harq_pic_2 = h_entity.find_empty_ul_harq();
  TESTASSERT(ul_harq_pic_2 != nullptr);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_2->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_2, max_n_rtx, dci_ul);
  ul_harq_pic_2->set_tbs(tbs_ul_2);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_2 and dci_ul.ndi == true and dci_ul.pid == 2);
  TESTASSERT(ul_harq_pic_2->mcs() == mcs_ul_2 and ul_harq_pic_2->tbs() == tbs_ul_2 and ul_harq_pic_2->ndi() == true);
  TESTASSERT(ul_harq_pic_2->get_softbuffer().size() == tbs_ul_2);
  latest_ul_ndi[2] = ul_harq_pic_2->ndi();

  // ----------------  SLOT 3  ------------------
  // Update slot
  h_entity.new_slot(++t);

  // DL : Create HARQ Proc ID 3
  dl_harq_proc* dl_harq_pic_3 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_3 != nullptr);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_3->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_3, max_n_rtx, dci_dl);
  dl_harq_pic_3->set_tbs(tbs_dl_3);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_3 and dci_dl.ndi == true and dci_dl.pid == 3);
  TESTASSERT(dl_harq_pic_3->mcs() == mcs_dl_3 and dl_harq_pic_3->tbs() == tbs_dl_3 and dl_harq_pic_3->ndi() == true);
  latest_dl_ndi[3] = dl_harq_pic_3->ndi();

  // UL: Create HARQ Proc ID 3
  ul_harq_proc* ul_harq_pic_3 = h_entity.find_empty_ul_harq();
  TESTASSERT(ul_harq_pic_3 != nullptr);

  // Create new transmission and set MCS + TBS
  // NOTE: UL HARQ is scheduled to be TXed at slot t + ul_tx_delay
  ul_harq_pic_3->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_3, max_n_rtx, dci_ul);
  ul_harq_pic_3->set_tbs(tbs_ul_3);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_3 and dci_ul.ndi == true and dci_ul.pid == 3);
  TESTASSERT(ul_harq_pic_3->mcs() == mcs_ul_3 and ul_harq_pic_3->tbs() == tbs_ul_3 and ul_harq_pic_3->ndi() == true);
  TESTASSERT(ul_harq_pic_3->get_softbuffer().size() == tbs_ul_3);
  latest_ul_ndi[3] = ul_harq_pic_3->ndi();

  // ----------------  SLOT 4  ------------------
  // Receive feedback (ACK) for DL HARQ PID 0 and update slot
  h_entity.dl_ack_info(0, 0, true);
  // This CRC info (OK) is for UL HARQ processed by scheduler at t = 0 (but scheduled for slot 4)
  h_entity.ul_crc_info(0, true);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 0
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_0);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_0->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_0, max_n_rtx, dci_dl);
  dl_harq_pic_0->set_tbs(tbs_dl_0);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_0 and dci_dl.ndi == next_ndi(latest_dl_ndi[0], true) and dci_dl.pid == 0);
  TESTASSERT(dl_harq_pic_0->mcs() == mcs_dl_0 and dl_harq_pic_0->tbs() == tbs_dl_0 and dl_harq_pic_0->ndi() == false);
  latest_dl_ndi[0] = dl_harq_pic_0->ndi();

  // UL : Verify HARQ process is pointing at PID 0
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_0);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_0->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_0, max_n_rtx, dci_ul);
  ul_harq_pic_0->set_tbs(tbs_ul_0);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_0 and dci_ul.ndi == next_ndi(latest_ul_ndi[0], true) and dci_ul.pid == 0);
  TESTASSERT(ul_harq_pic_0->mcs() == mcs_ul_0 and ul_harq_pic_0->tbs() == tbs_ul_0 and
             ul_harq_pic_0->ndi() == next_ndi(latest_ul_ndi[0], true));
  TESTASSERT(ul_harq_pic_0->get_softbuffer().size() == tbs_ul_0);
  latest_ul_ndi[0] = ul_harq_pic_0->ndi();

  // ----------------  SLOT 5  ------------------
  // Receive feedback (NACK) for DL HARQ PID 1 and update slot
  h_entity.dl_ack_info(1, 0, false);
  // This CRC (KO) info is for UL HARQ processed by scheduler at t = 1 (but scheduled for slot 5)
  h_entity.ul_crc_info(1, false);
  h_entity.new_slot(++t);

  // DL: Expect that all HARQ processes are active
  TESTASSERT(h_entity.find_empty_dl_harq() == nullptr);

  // NACK received for PID 1 -> Re-TX packet
  dl_harq_pic_1->new_retx(t, t + arq_slot_delay, grant_dl, dci_dl);
  dl_harq_pic_1->set_tbs(tbs_dl_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_1 and dci_dl.ndi == next_ndi(latest_dl_ndi[1], false) and dci_dl.pid == 1);
  TESTASSERT(dl_harq_pic_1->mcs() == mcs_dl_1 and dl_harq_pic_1->tbs() == tbs_dl_1 and dl_harq_pic_1->ndi() == true);
  latest_dl_ndi[1] = dl_harq_pic_1->ndi();

  // UL : Expect that all HARQ processes are active
  TESTASSERT(h_entity.find_empty_ul_harq() == nullptr);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_1->new_retx(t + ul_tx_delay, grant_ul, dci_ul);
  ul_harq_pic_1->set_tbs(tbs_ul_1);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_1 and dci_ul.ndi == next_ndi(latest_ul_ndi[1], false) and dci_ul.pid == 1);
  TESTASSERT(ul_harq_pic_1->mcs() == mcs_ul_1 and ul_harq_pic_1->tbs() == tbs_ul_1 and
             ul_harq_pic_1->ndi() == next_ndi(latest_ul_ndi[1], false));
  TESTASSERT(ul_harq_pic_1->get_softbuffer().size() == tbs_ul_1);
  latest_ul_ndi[1] = ul_harq_pic_1->ndi();

  // ----------------  SLOT 6  ------------------
  // Receive feedback (ACK) for DL HARQ PID 2 and update slot
  h_entity.dl_ack_info(2, 0, true);
  // This CRC (OK) info is for UL HARQ processed by scheduler at t = 2 (but scheduled for slot 6)
  h_entity.ul_crc_info(2, true);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 2
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_2);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_2->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_2, max_n_rtx, dci_dl);
  dl_harq_pic_2->set_tbs(tbs_dl_2);

  // Test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_2 and dci_dl.ndi == next_ndi(latest_dl_ndi[2], true) and dci_dl.pid == 2);
  TESTASSERT(dl_harq_pic_2->mcs() == mcs_dl_2 and dl_harq_pic_2->tbs() == tbs_dl_2 and
             dl_harq_pic_2->ndi() == next_ndi(latest_dl_ndi[2], true));
  latest_dl_ndi[2] = dl_harq_pic_2->ndi();

  // UL : Verify HARQ process is pointing at PID 2
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_2);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_2->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_2, max_n_rtx, dci_ul);
  ul_harq_pic_2->set_tbs(tbs_ul_2);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_2 and dci_ul.ndi == next_ndi(latest_ul_ndi[2], true) and dci_ul.pid == 2);
  TESTASSERT(ul_harq_pic_2->mcs() == mcs_ul_2 and ul_harq_pic_2->tbs() == tbs_ul_2 and
             ul_harq_pic_2->ndi() == next_ndi(latest_ul_ndi[2], true));
  TESTASSERT(ul_harq_pic_2->get_softbuffer().size() == tbs_ul_2);
  latest_ul_ndi[2] = ul_harq_pic_2->ndi();

  // ----------------  SLOT 7  ------------------
  // Receive feedback (NACK) for DL HARQ PID 3 and update slot
  h_entity.dl_ack_info(3, 0, false);
  // This CRC (KO) info is for UL HARQ processed by scheduler at t = 3 (but scheduled for slot 7)
  h_entity.ul_crc_info(3, false);
  h_entity.new_slot(++t);

  // DL : Verify that all HARQ processes are active
  TESTASSERT(h_entity.find_empty_dl_harq() == nullptr);

  // NACK received for PID 3 -> Re-TX packet
  dl_harq_pic_3->new_retx(t, t + arq_slot_delay, grant_dl, dci_dl);
  dl_harq_pic_3->set_tbs(tbs_dl_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_3 and dci_dl.ndi == next_ndi(latest_dl_ndi[3], false) and dci_dl.pid == 3);
  TESTASSERT(dl_harq_pic_3->mcs() == mcs_dl_3 and dl_harq_pic_3->tbs() == tbs_dl_3 and
             dl_harq_pic_3->ndi() == next_ndi(latest_dl_ndi[3], false));
  latest_dl_ndi[3] = dl_harq_pic_3->ndi();

  // UL : Expect that all HARQ processes are active
  TESTASSERT(h_entity.find_empty_ul_harq() == nullptr);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_3->new_retx(t + ul_tx_delay, grant_ul, dci_ul);
  ul_harq_pic_3->set_tbs(tbs_ul_3);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_3 and dci_ul.ndi == next_ndi(latest_ul_ndi[3], false) and dci_ul.pid == 3);
  TESTASSERT(ul_harq_pic_3->mcs() == mcs_ul_3 and ul_harq_pic_3->tbs() == tbs_ul_3 and
             ul_harq_pic_3->ndi() == next_ndi(latest_ul_ndi[3], false));
  TESTASSERT(ul_harq_pic_3->get_softbuffer().size() == tbs_ul_3);
  latest_ul_ndi[3] = ul_harq_pic_3->ndi();

  // ----------------  SLOT 8  ------------------
  // Receive feedback for DL HARQ PID 0 and update slot
  h_entity.dl_ack_info(0, 0, true);
  // This CRC info is for UL HARQ processed by scheduler at t = 4 (but scheduled for slot 8)
  h_entity.ul_crc_info(0, true);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 0
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_0);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_0->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_0, max_n_rtx, dci_dl);
  dl_harq_pic_0->set_tbs(tbs_dl_0);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_0 and dci_dl.ndi == next_ndi(latest_dl_ndi[0], true) and dci_dl.pid == 0);
  TESTASSERT(dl_harq_pic_0->mcs() == mcs_dl_0 and dl_harq_pic_0->tbs() == tbs_dl_0 and
             dl_harq_pic_0->ndi() == next_ndi(latest_dl_ndi[0], true));
  latest_dl_ndi[0] = dl_harq_pic_0->ndi();

  // UL : Verify HARQ process is pointing at PID 0
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_0);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_0->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_0, max_n_rtx, dci_ul);
  ul_harq_pic_0->set_tbs(tbs_ul_0);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_0 and dci_ul.ndi == next_ndi(latest_ul_ndi[0], true) and dci_ul.pid == 0);
  TESTASSERT(ul_harq_pic_0->mcs() == mcs_ul_0 and ul_harq_pic_0->tbs() == tbs_ul_0 and
             ul_harq_pic_0->ndi() == next_ndi(latest_ul_ndi[0], true));
  TESTASSERT(ul_harq_pic_0->get_softbuffer().size() == tbs_ul_0);
  latest_ul_ndi[0] = ul_harq_pic_0->ndi();

  // ----------------  SLOT 9  ------------------
  // Receive feedback (NACK) for HARQ PID 1 and update slot
  h_entity.dl_ack_info(1, 0, false);
  // This CRC info (KO) is for UL HARQ processed by scheduler at t = 5 (but scheduled for slot 9)
  h_entity.ul_crc_info(1, false);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 1
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_1);

  // NACK received and max retx attempts reached; discard packet and transmit a new one
  dl_harq_pic_1->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_1, max_n_rtx, dci_dl);
  dl_harq_pic_1->set_tbs(tbs_dl_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_1 and dci_dl.ndi == next_ndi(latest_dl_ndi[1], true) and dci_dl.pid == 1);
  TESTASSERT(dl_harq_pic_1->mcs() == mcs_dl_1 and dl_harq_pic_1->tbs() == tbs_dl_1 and
             dl_harq_pic_1->ndi() == next_ndi(latest_dl_ndi[1], true));
  latest_dl_ndi[1] = dl_harq_pic_1->ndi();

  // UL : Verify HARQ process is pointing at PID 1
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_1);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_1->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_1, max_n_rtx, dci_ul);
  ul_harq_pic_1->set_tbs(tbs_ul_1);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_1 and dci_ul.ndi == next_ndi(latest_ul_ndi[1], true) and dci_ul.pid == 1);
  TESTASSERT(ul_harq_pic_1->mcs() == mcs_ul_1 and ul_harq_pic_1->tbs() == tbs_ul_1 and
             ul_harq_pic_1->ndi() == next_ndi(latest_ul_ndi[1], true));
  TESTASSERT(ul_harq_pic_1->get_softbuffer().size() == tbs_ul_1);
  latest_ul_ndi[1] = ul_harq_pic_1->ndi();

  // ----------------  SLOT 10  ------------------
  // Receive feedback for HARQ PID 2 and update slot
  h_entity.dl_ack_info(2, 0, true);
  // This CRC info is for UL HARQ processed by scheduler at t = 6 (but scheduled for slot 10)
  h_entity.ul_crc_info(2, true);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 2
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_2);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_2->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_2, max_n_rtx, dci_dl);
  dl_harq_pic_2->set_tbs(tbs_dl_2);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_2 and dci_dl.ndi == next_ndi(latest_dl_ndi[2], true) and dci_dl.pid == 2);
  TESTASSERT(dl_harq_pic_2->mcs() == mcs_dl_2 and dl_harq_pic_2->tbs() == tbs_dl_2 and
             dl_harq_pic_2->ndi() == next_ndi(latest_dl_ndi[2], true));
  latest_dl_ndi[2] = dl_harq_pic_2->ndi();

  // UL : Verify HARQ process is pointing at PID 2
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_2);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_2->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_2, max_n_rtx, dci_ul);
  ul_harq_pic_2->set_tbs(tbs_ul_2);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_2 and dci_ul.ndi == next_ndi(latest_ul_ndi[2], true) and dci_ul.pid == 2);
  TESTASSERT(ul_harq_pic_2->mcs() == mcs_ul_2 and ul_harq_pic_2->tbs() == tbs_ul_2 and
             ul_harq_pic_2->ndi() == next_ndi(latest_ul_ndi[2], true));
  TESTASSERT(ul_harq_pic_2->get_softbuffer().size() == tbs_ul_2);
  latest_ul_ndi[2] = ul_harq_pic_2->ndi();

  // ----------------  SLOT 11  ------------------
  // Receive feedback (ACK) for HARQ PID 1 and update slot
  h_entity.dl_ack_info(3, 0, true);
  // This CRC info (OK) is for UL HARQ processed by scheduler at t = 7 (but scheduled for slot 11)
  h_entity.ul_crc_info(3, true);
  h_entity.new_slot(++t);

  // DL : Verify HARQ process is pointing at PID 3
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_3);

  // Create new transmission and set MCS + TBS
  dl_harq_pic_3->new_tx(t, t + arq_slot_delay, grant_dl, mcs_dl_3, max_n_rtx, dci_dl);
  dl_harq_pic_3->set_tbs(tbs_dl_3);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_dl.mcs == mcs_dl_3 and dci_dl.ndi == next_ndi(latest_dl_ndi[3], true) and dci_dl.pid == 3);
  TESTASSERT(dl_harq_pic_3->mcs() == mcs_dl_3 and dl_harq_pic_3->tbs() == tbs_dl_3 and
             dl_harq_pic_3->ndi() == next_ndi(latest_dl_ndi[3], true));
  latest_dl_ndi[3] = dl_harq_pic_3->ndi();

  // UL : Expect that all HARQ processes are active
  TESTASSERT(h_entity.find_empty_ul_harq() == ul_harq_pic_3);

  // Create new transmission and set MCS + TBS
  ul_harq_pic_3->new_tx(t + ul_tx_delay, grant_ul, mcs_ul_3, max_n_rtx, dci_ul);
  ul_harq_pic_3->set_tbs(tbs_ul_3);

  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT(dci_ul.mcs == mcs_ul_3 and dci_ul.ndi == next_ndi(latest_ul_ndi[3], true) and dci_ul.pid == 3);
  TESTASSERT(ul_harq_pic_3->mcs() == mcs_ul_3 and ul_harq_pic_3->tbs() == tbs_ul_3 and
             ul_harq_pic_3->ndi() == next_ndi(latest_ul_ndi[3], true));
  TESTASSERT(ul_harq_pic_3->get_softbuffer().size() == tbs_ul_3);
  latest_ul_ndi[3] = ul_harq_pic_3->ndi();
}

int main()
{
  test_harq();
}