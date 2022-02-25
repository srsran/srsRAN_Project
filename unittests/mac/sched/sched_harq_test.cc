
#include "../../../lib/mac/sched/sched_harq.h"
#include "srsgnb/support/test_utils.h"

#define TEST_HARQ_ASSERT_MSG(SLOT, PID) fmt::format("Failed at slot: '{}', HARQ PID: '{}'", SLOT, PID).c_str()

using namespace srsgnb;

struct harq_proc_params {
  uint32_t pid;
  uint32_t arq_slot_delay;
  uint32_t max_n_rtx;
  uint32_t mcs;
  uint32_t tbs;
};

struct harq_proc_vars {
  bool          ack        = false;
  bool          latest_ndi = false;
  dl_harq_proc* dl_proc    = nullptr;
  ul_harq_proc* ul_proc    = nullptr;
};

struct harq_entity_params {
  uint32_t max_n_rtx      = 1;
  uint32_t nof_h_procs    = 4;
  uint32_t arq_slot_delay = 4;
  uint32_t ul_tx_delay    = 4;
};

static void h_entity_report_ack(const struct harq_entity_params& common,
                                const struct harq_proc_params&   dl_param,
                                const struct harq_proc_params&   ul_param,
                                uint32_t                         slot,
                                struct harq_proc_vars&           dl_var,
                                struct harq_proc_vars&           ul_var,
                                harq_entity&                     h_entity)
{
  // Receive feedback for HARQ
  // We add 1 ot the slot, as  this function is called before the slot gets updated
  // No need to get feedback for the first common.nof_h_procs slots
  if ((slot + 1) < common.nof_h_procs) {
    return;
  }

  // DL: Report ACK
  h_entity.dl_ack_info(dl_param.pid, 0, dl_var.ack);
  TESTASSERT_MSG(&(h_entity.dl_harq(dl_param.pid)) == dl_var.dl_proc, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));

  // If positive ACK or if pck is going to be discarded, reset HARQ pointer (the h_entity might assign a new one)
  if (dl_var.ack or
      (not dl_var.ack and h_entity.dl_harq(dl_param.pid).nof_retx() >= h_entity.dl_harq(dl_param.pid).max_nof_retx())) {
    dl_var.dl_proc = nullptr;
  }

  // UL: Report ACK (or read CRC from received pck)
  h_entity.ul_crc_info(ul_param.pid, ul_var.ack);
  TESTASSERT_MSG(&(h_entity.ul_harq(ul_param.pid)) == ul_var.ul_proc, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));

  // If positive ACK (CRC) or if pck is going to be discarded, reset HARQ pointer (the h_entity might assign a new one)
  if (ul_var.ack or
      (not ul_var.ack and h_entity.ul_harq(ul_param.pid).nof_retx() >= h_entity.ul_harq(ul_param.pid).max_nof_retx())) {
    ul_var.ul_proc = nullptr;
  }
}

static void test_dl_harq_entity_slot(const struct harq_entity_params& common,
                                     const struct harq_proc_params&   dl_param,
                                     uint32_t                         slot,
                                     struct harq_proc_vars&           dl_var,
                                     harq_entity&                     h_entity)

{
  dl_harq_proc*   dl_proc;
  struct dci_dl_t dci_dl;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_dl(rbg_btmp);

  // Case of NEW TX
  if (dl_var.dl_proc == nullptr) {
    // If the HARQ process hasn't been a pointer, get one from the h_entity
    dl_proc        = h_entity.find_empty_dl_harq();
    dl_var.dl_proc = dl_proc;

    // Create new transmission and set MCS + TBS
    dl_proc->new_tx(slot, slot + dl_param.arq_slot_delay, grant_dl, dl_param.mcs, dl_param.max_n_rtx, dci_dl);
    dl_proc->set_tbs(dl_param.tbs);

    // For a new TX, verify the NDI gets toggled
    TESTASSERT_EQ_MSG(dci_dl.ndi, not dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
    TESTASSERT_EQ_MSG(dl_proc->ndi(), not dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
  }
  // Case of NEW RE-TX
  else {
    // Verify the saved pointer coincides with the one in the h_entity
    TESTASSERT_MSG(&(h_entity.dl_harq(dl_param.pid)) == dl_var.dl_proc, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
    dl_proc = dl_var.dl_proc;

    // Create new re-transmission and set TBS
    dl_proc->new_retx(slot, slot + dl_param.arq_slot_delay, grant_dl, dci_dl);
    dl_proc->set_tbs(dl_param.tbs);

    // For a new RE-TX, verify the NDI is the same as for the original TX
    TESTASSERT_EQ_MSG(dci_dl.ndi, dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
    TESTASSERT_EQ_MSG(dl_proc->ndi(), dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
  }

  dl_var.latest_ndi = dl_proc->ndi();
  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT_EQ_MSG(dci_dl.mcs, dl_param.mcs, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
  TESTASSERT_EQ_MSG(dci_dl.pid, dl_param.pid, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
  TESTASSERT_EQ_MSG(dl_proc->mcs(), dl_param.mcs, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
  TESTASSERT_EQ_MSG(dl_proc->tbs(), dl_param.tbs, TEST_HARQ_ASSERT_MSG(slot, dl_param.pid));
}

static void test_ul_harq_entity_slot(const struct harq_entity_params& common,
                                     const struct harq_proc_params&   ul_param,
                                     uint32_t                         slot,
                                     struct harq_proc_vars&           ul_var,
                                     harq_entity&                     h_entity)
{
  ul_harq_proc*   ul_proc;
  struct dci_ul_t dci_ul;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_ul = rbg_btmp;

  // Case of NEW TX
  if (ul_var.ul_proc == nullptr) {
    // If the HARQ process hasn't been a pointer, get one from the h_entityeen a pointer, get one from the h_entity
    ul_proc        = h_entity.find_empty_ul_harq();
    ul_var.ul_proc = ul_proc;

    // Create new transmission and set MCS + TBS
    ul_proc->new_tx(slot + ul_param.arq_slot_delay, grant_ul, ul_param.mcs, ul_param.max_n_rtx, dci_ul);
    ul_proc->set_tbs(ul_param.tbs);

    // For a new TX, verify the NDI gets toggled
    TESTASSERT_EQ_MSG(dci_ul.ndi, not ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
    TESTASSERT_EQ_MSG(ul_proc->ndi(), not ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  }
  // Case of NEW RE-TX: verify the saved pointer coincides with the one in the h_entity
  else {
    // Verify the saved pointer coincides with the one in the h_entity
    TESTASSERT(&(h_entity.ul_harq(ul_param.pid)) == ul_var.ul_proc);
    ul_proc = ul_var.ul_proc;

    // NACK received for PID 1 -> Re-TX packet
    ul_proc->new_retx(slot + ul_param.arq_slot_delay, grant_ul, dci_ul);
    ul_proc->set_tbs(ul_param.tbs);

    // For a new RE-TX, verify the NDI is the same as for the original TX
    TESTASSERT_EQ_MSG(dci_ul.ndi, ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
    TESTASSERT_EQ_MSG(ul_proc->ndi(), ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  }

  ul_var.latest_ndi = ul_proc->ndi();
  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT_EQ_MSG(dci_ul.mcs, ul_param.mcs, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  TESTASSERT_EQ_MSG(dci_ul.pid, ul_param.pid, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  TESTASSERT_EQ_MSG(ul_proc->mcs(), ul_param.mcs, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  TESTASSERT_EQ_MSG(ul_proc->tbs(), ul_param.tbs, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
  TESTASSERT_EQ_MSG(ul_proc->get_softbuffer().size(), ul_param.tbs, TEST_HARQ_ASSERT_MSG(slot, ul_param.pid));
}

void test_harq(srslog::basic_logger& harq_logger)
{
  /// This tests aims at verifying the class harq_entity. It simulates 4 DL and 4 UL HARQ processes, it assumes that, at
  /// each slot only 1 DL and 1 UL HARQ processes get allocated a transmission or a retransmission. The test runs for 12
  /// slots, and each Processes will be scheduled once in a 4-slot interval.
  /// It assumes: DL and UL HARQ processes 0 and 2 are always successful (ACK reported)
  /// DL and UL HARQ processes 1 get 2 consecutive NACKs and their packet will be discarded
  /// DL and UL HARQ processes 3 get 1 NACK and 1 ACK

  using harq_proc_params = struct harq_proc_params;
  using harq_proc_vars   = struct harq_proc_vars;

  // Test parameters
  constexpr uint32_t        max_n_rtx      = 1;
  constexpr uint32_t        nof_prbs       = 452;
  constexpr uint32_t        nof_h_procs    = 4;
  constexpr uint32_t        arq_slot_delay = 4;
  constexpr uint32_t        ul_tx_delay    = 4;
  struct harq_entity_params common_param;

  // Create HARQ entity
  harq_entity h_entity = harq_entity(0x04601, nof_prbs, nof_h_procs, harq_logger);

  // ----------------  SLOT 0  ------------------
  // Update slot
  // Let it do the job of finding the MAX UINT 32
  uint32_t t = -1;

  // Define DL HARQ PID 0
  const harq_proc_params dl_h_par_0{
      .pid = 0, .arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};
  harq_proc_vars dl_h_var_0{};
  // Define UL HARQ PID 0
  harq_proc_params ul_h_par_0{.pid = 0, .arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};
  harq_proc_vars   ul_h_var_0{};

  // Update ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, t, dl_h_var_0, ul_h_var_0, h_entity);
  // Increment slot and, if necessary, clear HARQ processes
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, t, dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, t, ul_h_var_0, h_entity);

  // ----------------  SLOT 1  ------------------
  // Update slot

  // Define DL HARQ PID 1
  const harq_proc_params dl_h_par_1{
      .pid = 1, .arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};
  harq_proc_vars dl_h_var_1{};
  // Define UL HARQ PID 1
  harq_proc_params ul_h_par_1{.pid = 1, .arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};
  harq_proc_vars   ul_h_var_1{};

  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, t, dl_h_var_1, ul_h_var_1, h_entity);
  h_entity.new_slot(++t);
  test_dl_harq_entity_slot(common_param, dl_h_par_1, t, dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, t, ul_h_var_1, h_entity);

  // ----------------  SLOT 2  ------------------
  // Update slot

  // Define DL HARQ PID 2
  const harq_proc_params dl_h_par_2{
      .pid = 2, .arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 11, .tbs = 1918};
  harq_proc_vars dl_h_var_2{};
  // Define UL HARQ PID 2
  harq_proc_params ul_h_par_2{.pid = 2, .arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 8, .tbs = 1616};
  harq_proc_vars   ul_h_var_2{};

  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, t, dl_h_var_2, ul_h_var_2, h_entity);
  h_entity.new_slot(++t);
  test_dl_harq_entity_slot(common_param, dl_h_par_2, t, dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, t, ul_h_var_2, h_entity);

  // ----------------  SLOT 3  ------------------
  // Update slot

  // Define DL HARQ PID 2
  const harq_proc_params dl_h_par_3{
      .pid = 3, .arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 8, .tbs = 1224};
  harq_proc_vars dl_h_var_3{};
  // Define UL HARQ PID 2
  harq_proc_params ul_h_par_3{.pid = 3, .arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 23, .tbs = 4578};
  harq_proc_vars   ul_h_var_3{};

  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, t, dl_h_var_3, ul_h_var_3, h_entity);
  h_entity.new_slot(++t);
  test_dl_harq_entity_slot(common_param, dl_h_par_3, t, dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, t, ul_h_var_3, h_entity);

  // ----------------  SLOT 4  ------------------
  // From this slot onwards, the ACK will be actually processed
  // Update ACKs
  dl_h_var_0.ack = true;
  ul_h_var_0.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, t, dl_h_var_0, ul_h_var_0, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, t, dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, t, ul_h_var_0, h_entity);

  // ----------------  SLOT 5  ------------------

  // Update ACKs
  dl_h_var_1.ack = false;
  ul_h_var_1.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, t, dl_h_var_1, ul_h_var_1, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_1, t, dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, t, ul_h_var_1, h_entity);

  // ----------------  SLOT 6  ------------------

  // Update ACKs
  dl_h_var_2.ack = true;
  ul_h_var_2.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, t, dl_h_var_2, ul_h_var_2, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_2, t, dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, t, ul_h_var_2, h_entity);

  // ----------------  SLOT 7  ------------------

  // Update ACKs
  dl_h_var_3.ack = false;
  ul_h_var_3.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, t, dl_h_var_3, ul_h_var_3, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_3, t, dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, t, ul_h_var_3, h_entity);

  // ----------------  SLOT 8  ------------------

  // Update ACKs
  dl_h_var_0.ack = true;
  ul_h_var_0.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, t, dl_h_var_0, ul_h_var_0, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, t, dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, t, ul_h_var_0, h_entity);

  // ----------------  SLOT 9  ------------------
  // Make PID 1 fail, this will trigger the packets to be discarded
  dl_h_var_1.ack = false;
  ul_h_var_1.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, t, dl_h_var_1, ul_h_var_1, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_1, t, dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, t, ul_h_var_1, h_entity);

  // ----------------  SLOT 10  ------------------

  // Update ACKs
  dl_h_var_2.ack = true;
  ul_h_var_2.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, t, dl_h_var_2, ul_h_var_2, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_2, t, dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, t, ul_h_var_2, h_entity);

  // ----------------  SLOT 11  ------------------

  // Update ACKs
  dl_h_var_3.ack = true;
  ul_h_var_3.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, t, dl_h_var_3, ul_h_var_3, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  h_entity.new_slot(++t);
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_3, t, dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, t, ul_h_var_3, h_entity);
}

int main()
{
  // Initialize logger
  srslog::init();
  auto& harq_logger = srslog::fetch_basic_logger("MAC-NR");
  harq_logger.set_level(srslog::basic_levels::info);

  test_harq(harq_logger);
}