/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_scheduling/harq_process.h"
#include "srsgnb/support/test_utils.h"

#define TEST_HARQ_ASSERT_MSG(SLOT, PID) "Failed at slot: '{}', HARQ PID: '{}'", SLOT, PID

using namespace srsgnb;

/// Helper struct to test HARQs and update loggers slot context
struct test_bench {
  static srslog::basic_logger& test_logger;
  static srslog::basic_logger& mac_logger;

  test_bench(harq_entity& h, unsigned tx_gnb_delay_ = 0, unsigned tx_ack_delay_ = 1) :
    h_entity(h), tx_gnb_delay(tx_gnb_delay_), tx_ack_delay(tx_ack_delay_)
  {
    test_logger.set_context(0);
    mac_logger.set_context(0);
    t = slot_point{0, 0}; // first slot is 0
  }

  void new_slot()
  {
    ++t;
    test_logger.set_context(t.to_uint());
    mac_logger.set_context(t.to_uint());
    h_entity.slot_indication(t);
  }

  slot_point slot_rx() { return t - tx_gnb_delay; }
  slot_point slot_tx() { return t; }
  slot_point dl_slot_ack() { return t + tx_ack_delay; }

private:
  harq_entity&   h_entity;
  const unsigned tx_gnb_delay;
  const unsigned tx_ack_delay;
  slot_point     t;
};
srslog::basic_logger& test_bench::test_logger = srslog::fetch_basic_logger("TEST");
srslog::basic_logger& test_bench::mac_logger  = srslog::fetch_basic_logger("MAC");

struct harq_proc_params {
  uint32_t arq_slot_delay;
  uint32_t max_n_rtx;
  uint32_t mcs;
  uint32_t tbs;
};

struct harq_proc_vars {
  uint32_t         pid;
  bool             ack        = false;
  bool             latest_ndi = false;
  dl_harq_process* dl_proc    = nullptr;
  ul_harq_process* ul_proc    = nullptr;
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
                                slot_point                       slot,
                                struct harq_proc_vars&           dl_var,
                                struct harq_proc_vars&           ul_var,
                                harq_entity&                     h_entity)
{
  // Receive feedback for HARQ
  // We add 1 ot the slot, as  this function is called before the slot gets updated
  // No need to get feedback for the first common.nof_h_procs slots
  if ((slot + 1).to_uint() < common.nof_h_procs) {
    return;
  }

  // DL: Report ACK
  h_entity.dl_ack_info(dl_var.pid, 0, dl_var.ack);
  TESTASSERT(&(h_entity.dl_harq(dl_var.pid)) == dl_var.dl_proc, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));

  // If positive ACK or if pck is going to be discarded, reset HARQ pointer (the h_entity might assign a new one)
  if (dl_var.ack or
      (not dl_var.ack and h_entity.dl_harq(dl_var.pid).nof_retx() >= h_entity.dl_harq(dl_var.pid).max_nof_retx())) {
    dl_var.dl_proc = nullptr;
  }

  // UL: Report ACK (or read CRC from received pck)
  h_entity.ul_crc_info(ul_var.pid, ul_var.ack);
  TESTASSERT(&(h_entity.ul_harq(ul_var.pid)) == ul_var.ul_proc, TEST_HARQ_ASSERT_MSG(slot, ul_var.pid));

  // If positive ACK (CRC) or if pck is going to be discarded, reset HARQ pointer (the h_entity might assign a new one)
  if (ul_var.ack or
      (not ul_var.ack and h_entity.ul_harq(ul_var.pid).nof_retx() >= h_entity.ul_harq(ul_var.pid).max_nof_retx())) {
    ul_var.ul_proc = nullptr;
  }
}

static void test_dl_harq_entity_slot(const struct harq_entity_params& common,
                                     const struct harq_proc_params&   dl_param,
                                     slot_point                       slot,
                                     struct harq_proc_vars&           dl_var,
                                     harq_entity&                     h_entity)

{
  dl_harq_process* dl_proc;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_dl(rbg_btmp);

  // Case of NEW TX
  if (dl_var.dl_proc == nullptr) {
    // If the HARQ process hasn't been a pointer, get one from the h_entity
    dl_proc = h_entity.find_empty_dl_harq();
    // Get the PID assigned by the h_entity
    dl_var.pid     = dl_proc->id;
    dl_var.dl_proc = dl_proc;

    // Create new transmission and set MCS + TBS
    dl_proc->new_tx(slot, slot + dl_param.arq_slot_delay, grant_dl, dl_param.mcs, dl_param.max_n_rtx);
    dl_proc->set_tbs(dl_param.tbs);
    test_bench::test_logger.info(
        "New DL HARQ tx for pid={}: [ndi={}, ack_slot={}]", dl_proc->id, dl_proc->ndi(0), dl_proc->harq_slot_ack());

    // For a new TX, verify the NDI gets toggled
    TESTASSERT_EQ(dl_proc->ndi(0), not dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));
  }
  // Case of NEW RE-TX
  else {
    // Verify the saved pointer coincides with the one in the h_entity
    TESTASSERT(&(h_entity.dl_harq(dl_var.pid)) == dl_var.dl_proc, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));
    dl_proc = dl_var.dl_proc;

    // Create new re-transmission and set TBS
    dl_proc->new_retx(slot, slot + dl_param.arq_slot_delay, grant_dl);
    dl_proc->set_tbs(dl_param.tbs);
    test_bench::test_logger.info(
        "New DL HARQ retx for pid={}: [ndi={}, ack_slot={}]", dl_proc->id, dl_proc->ndi(0), dl_proc->harq_slot_ack());

    // For a new RE-TX, verify the NDI is the same as for the original TX
    TESTASSERT_EQ(dl_proc->ndi(0), dl_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));
  }

  dl_var.latest_ndi = dl_proc->ndi(0);
  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT_EQ(dl_proc->mcs(0), dl_param.mcs, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));
  TESTASSERT_EQ(dl_proc->tbs(0), dl_param.tbs, TEST_HARQ_ASSERT_MSG(slot, dl_var.pid));
}

static void test_ul_harq_entity_slot(const struct harq_entity_params& common,
                                     const struct harq_proc_params&   ul_param,
                                     slot_point                       slot,
                                     struct harq_proc_vars&           ul_var,
                                     harq_entity&                     h_entity)
{
  ul_harq_process* ul_proc;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_ul = rbg_btmp;

  // Case of NEW TX
  if (ul_var.ul_proc == nullptr) {
    // If the HARQ process hasn't been a pointer, get one from the h_entityeen a pointer, get one from the h_entity
    ul_proc = h_entity.find_empty_ul_harq();
    // Get the PID assigned by the h_entity
    ul_var.pid     = ul_proc->id;
    ul_var.ul_proc = ul_proc;

    // Create new transmission and set MCS + TBS
    ul_proc->new_tx(slot + ul_param.arq_slot_delay, grant_ul, ul_param.mcs, ul_param.max_n_rtx);
    ul_proc->set_tbs(ul_param.tbs);

    // For a new TX, verify the NDI gets toggled
    TESTASSERT_EQ(ul_proc->ndi(0), not ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_var.pid));
  }
  // Case of NEW RE-TX: verify the saved pointer coincides with the one in the h_entity
  else {
    // Verify the saved pointer coincides with the one in the h_entity
    TESTASSERT(&(h_entity.ul_harq(ul_var.pid)) == ul_var.ul_proc);
    ul_proc = ul_var.ul_proc;

    // NACK received for PID 1 -> Re-TX packet
    ul_proc->new_retx(slot + ul_param.arq_slot_delay, grant_ul);
    ul_proc->set_tbs(ul_param.tbs);

    // For a new RE-TX, verify the NDI is the same as for the original TX
    TESTASSERT_EQ(ul_proc->ndi(0), ul_var.latest_ndi, TEST_HARQ_ASSERT_MSG(slot, ul_var.pid));
  }

  ul_var.latest_ndi = ul_proc->ndi(0);
  // test whether DCI has been filled correctly and DL HARQ has the correct parameters
  TESTASSERT_EQ(ul_proc->mcs(0), ul_param.mcs, TEST_HARQ_ASSERT_MSG(slot, ul_var.pid));
  TESTASSERT_EQ(ul_proc->tbs(0), ul_param.tbs, TEST_HARQ_ASSERT_MSG(slot, ul_var.pid));
}

void test_dl_invalid_paths(srslog::basic_logger& harq_logger)
{
  // This test aims at testing DL functionalities of class ul_harq_proc and class h_entity
  // It tests how the functions respond to operations that are valid and invalid (not permitted)

  using harq_proc_params = struct harq_proc_params;
  using harq_proc_vars   = struct harq_proc_vars;

  // Test parameters
  constexpr uint32_t        max_n_rtx      = 3;
  constexpr harq_id_t       pid            = to_harq_id(2);
  constexpr uint32_t        nof_prbs       = 52;
  constexpr uint32_t        nof_h_procs    = 4;
  constexpr uint32_t        arq_slot_delay = 1;
  struct harq_entity_params common_param;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_dl(rbg_btmp);

  // Create DL_HARQ object
  dl_harq_process dl_test = dl_harq_process(pid);
  TESTASSERT_EQ(dl_test.id, pid);

  // Create HARQ entity
  harq_entity h_entity = harq_entity(to_rnti(0x04601), nof_prbs, nof_h_procs, arq_slot_delay);
  test_bench  bench(h_entity, 0, arq_slot_delay);

  // ----------------  SLOT 0  ------------------
  // Verify that there are empty DL HARQs
  TESTASSERT(h_entity.find_empty_dl_harq() != nullptr);

  // Increment slot
  bench.new_slot();

  // Define DL HARQ PID 0
  const harq_proc_params dl_h_par_0{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};
  harq_proc_vars         dl_h_var_0{};

  // Retrive the DL HARQ pointers from the h_entity
  dl_harq_process* dl_proc = h_entity.find_empty_dl_harq();

  // Verify that allocating a RETX without allocating a TX first leads to the function returning false
  TESTASSERT_EQ(dl_proc->new_retx(bench.slot_tx(), bench.dl_slot_ack(), grant_dl), false);

  // Verify that allocating a new TX leads to the function returning true
  TESTASSERT_EQ(dl_proc->new_tx(bench.slot_tx(), bench.dl_slot_ack(), grant_dl, dl_h_par_0.mcs, dl_h_par_0.max_n_rtx),
                true);

  // Report ACK and advance slots number
  h_entity_report_ack(common_param, dl_h_par_0, dl_h_par_0, bench.slot_tx(), dl_h_var_0, dl_h_var_0, h_entity);
  bench.new_slot();
  bench.new_slot();

  // Verify that there should be a pending TX
  TESTASSERT(h_entity.find_pending_dl_retx() == dl_proc);

  // Define DL HARQ PID 1
  const harq_proc_params dl_h_par_1{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};

  // Retrive the UL HARQ pointers from the h_entity
  dl_proc = h_entity.find_empty_dl_harq();

  // Increase ACK report delay and schedule a transmission (expect SUCCESS)
  TESTASSERT_EQ(dl_proc->new_tx(bench.slot_tx(), bench.dl_slot_ack(), grant_dl, dl_h_par_1.mcs, dl_h_par_1.max_n_rtx),
                true);
  bench.new_slot();
  // Schedule a transmission for the same PID before the ACK is received (expect FAIL)
  TESTASSERT_EQ(dl_proc->new_tx(bench.slot_tx(), bench.dl_slot_ack(), grant_dl, dl_h_par_1.mcs, dl_h_par_1.max_n_rtx),
                false);
}

void test_ul_invalid_paths(srslog::basic_logger& harq_logger)
{
  // This test aims at testing UL functionalities of class ul_harq_proc and class h_entity
  // It tests how the functions respond to operations that are valid and invalid (not permitted)

  using harq_proc_params = struct harq_proc_params;
  using harq_proc_vars   = struct harq_proc_vars;

  // Test parameters
  constexpr uint32_t        max_n_rtx    = 3;
  constexpr harq_id_t       pid          = to_harq_id(2);
  constexpr uint32_t        nof_prbs     = 52;
  constexpr uint32_t        nof_h_procs  = 4;
  constexpr uint32_t        ul_tx_delay  = 4;
  constexpr uint32_t        tx_gnb_delay = 1;
  struct harq_entity_params common_param;

  // The specific grant does not matter for this test
  rbg_bitmap rbg_btmp(18);
  rbg_btmp.from_uint64(0b101000111000111101);
  prb_grant grant_ul(rbg_btmp);

  // Create UL_HARQ object
  ul_harq_process ul_test = ul_harq_process(pid);
  TESTASSERT_EQ(ul_test.id, pid);

  // Create HARQ entity
  harq_entity h_entity = harq_entity(to_rnti(0x4601), nof_prbs, nof_h_procs, tx_gnb_delay);

  // ----------------  SLOT 0  ------------------
  // Update slot
  // Let it do the job of finding the MAX UINT 32
  slot_point t{0, 0};

  // Verify that there are empty UL HARQs
  TESTASSERT(h_entity.find_empty_ul_harq() != nullptr);

  // Increment slot
  h_entity.slot_indication(t);

  // Define UL HARQ PID 0
  const harq_proc_params ul_h_par_0{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};
  harq_proc_vars         ul_h_var_0{};

  // Retrive the UL HARQ pointers from the h_entity
  ul_harq_process* ul_proc = h_entity.find_empty_ul_harq();

  // Verify that allocating a RETX without allocating a TX first leads to the function returning false
  TESTASSERT_EQ(ul_proc->new_retx(t + ul_tx_delay, grant_ul), false);

  // Verify that allocating a new TX leads to the function returning true
  TESTASSERT_EQ(ul_proc->new_tx(t + ul_tx_delay, grant_ul, ul_h_par_0.mcs, ul_h_par_0.max_n_rtx), true);

  // Report ACK and advance slots number
  h_entity_report_ack(common_param, ul_h_par_0, ul_h_par_0, t, ul_h_var_0, ul_h_var_0, h_entity);
  for (unsigned i = 0; i != ul_tx_delay + tx_gnb_delay - 1; ++i) {
    h_entity.slot_indication(++t);
  }

  // Verify that there should be a pending TX
  h_entity.slot_indication(++t);
  TESTASSERT(h_entity.find_pending_ul_retx() == ul_proc);

  // Define UL HARQ PID 1
  const harq_proc_params ul_h_par_1{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};

  // Retrive the UL HARQ pointers from the h_entity
  ul_proc = h_entity.find_empty_ul_harq();

  // Increase ACK report delay and schedule a transmission (expect SUCCESS)
  TESTASSERT_EQ(ul_proc->new_tx(t, grant_ul, ul_h_par_1.mcs, ul_h_par_1.max_n_rtx), true);
  h_entity.slot_indication(++t);
  // Schedule a transmission for the same PID before the ACK is received (expect FAIL)
  TESTASSERT_EQ(ul_proc->new_tx(t, grant_ul, ul_h_par_1.mcs, ul_h_par_1.max_n_rtx), false);
}

void test_harq(srslog::basic_logger& harq_logger)
{
  /// This tests aims at verifying the class harq_entity. It simulates 4 DL and 4 UL HARQ processes, it assumes that, at
  /// each slot only 1 DL and 1 UL HARQ processes get allocated a transmission or a retransmission. The test runs for 12
  /// slots, and each Processes will be scheduled once in a 4-slot interval.
  /// It assumes: DL and UL HARQ processes 0 and 2 are always successful (ACK reported)
  /// DL and UL HARQ processes 1 get 2 consecutive NACKs and their packet will be discarded
  /// DL and UL HARQ processes 3 get 1 NACK and 1 ACK

  /// NOTE: The PID thatused in the comments and as variable name might not match the PID that is in the
  ///       struct harq_proc_vars{}, which takes the value internally used by the h_entity class.
  ///       The PID num in comments and variable name for ease of reference in the test.

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
  harq_entity h_entity = harq_entity(to_rnti(0x4601), nof_prbs, nof_h_procs, arq_slot_delay);
  test_bench  bench{h_entity, 0, arq_slot_delay};

  // ----------------  SLOT 0  ------------------

  // Define DL HARQ PID 0
  const harq_proc_params dl_h_par_0{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};
  harq_proc_vars         dl_h_var_0{};
  // Define UL HARQ PID 0
  const harq_proc_params ul_h_par_0{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};
  harq_proc_vars         ul_h_var_0{};

  // Update ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, bench.slot_tx(), dl_h_var_0, ul_h_var_0, h_entity);
  // Increment slot and, if necessary, clear HARQ processes
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, bench.slot_tx(), dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, bench.slot_tx(), ul_h_var_0, h_entity);

  // ----------------  SLOT 1  ------------------
  // Update slot

  // Define DL HARQ PID 1
  const harq_proc_params dl_h_par_1{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 17, .tbs = 3152};
  harq_proc_vars         dl_h_var_1{};
  // Define UL HARQ PID 1
  const harq_proc_params ul_h_par_1{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 12, .tbs = 3152};
  harq_proc_vars         ul_h_var_1{};

  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, bench.slot_tx(), dl_h_var_1, ul_h_var_1, h_entity);
  bench.new_slot();
  test_dl_harq_entity_slot(common_param, dl_h_par_1, bench.slot_tx(), dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, bench.slot_tx(), ul_h_var_1, h_entity);

  // ----------------  SLOT 2  ------------------
  // Update slot

  // Define DL HARQ PID 2
  const harq_proc_params dl_h_par_2{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 11, .tbs = 1918};
  harq_proc_vars         dl_h_var_2{};
  // Define UL HARQ PID 2
  const harq_proc_params ul_h_par_2{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 8, .tbs = 1616};
  harq_proc_vars         ul_h_var_2{};

  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, bench.slot_tx(), dl_h_var_2, ul_h_var_2, h_entity);
  bench.new_slot();
  test_dl_harq_entity_slot(common_param, dl_h_par_2, bench.slot_tx(), dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, bench.slot_tx(), ul_h_var_2, h_entity);

  // ----------------  SLOT 3  ------------------
  // Update slot

  // Define DL HARQ PID 2
  const harq_proc_params dl_h_par_3{.arq_slot_delay = arq_slot_delay, .max_n_rtx = max_n_rtx, .mcs = 8, .tbs = 1224};
  harq_proc_vars         dl_h_var_3{};
  // Define UL HARQ PID 2
  const harq_proc_params ul_h_par_3{.arq_slot_delay = ul_tx_delay, .max_n_rtx = max_n_rtx, .mcs = 23, .tbs = 4578};
  harq_proc_vars         ul_h_var_3{};

  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, bench.slot_tx(), dl_h_var_3, ul_h_var_3, h_entity);
  bench.new_slot();
  test_dl_harq_entity_slot(common_param, dl_h_par_3, bench.slot_tx(), dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, bench.slot_tx(), ul_h_var_3, h_entity);

  // ----------------  SLOT 4  ------------------
  // From this slot onwards, the ACK will be actually processed
  // Update ACKs
  dl_h_var_0.ack = true;
  ul_h_var_0.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, bench.slot_tx(), dl_h_var_0, ul_h_var_0, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, bench.slot_tx(), dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, bench.slot_tx(), ul_h_var_0, h_entity);

  // ----------------  SLOT 5  ------------------

  // Update ACKs
  dl_h_var_1.ack = false;
  ul_h_var_1.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, bench.slot_tx(), dl_h_var_1, ul_h_var_1, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_1, bench.slot_tx(), dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, bench.slot_tx(), ul_h_var_1, h_entity);

  // ----------------  SLOT 6  ------------------

  // Update ACKs
  dl_h_var_2.ack = true;
  ul_h_var_2.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, bench.slot_tx(), dl_h_var_2, ul_h_var_2, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_2, bench.slot_tx(), dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, bench.slot_tx(), ul_h_var_2, h_entity);

  // ----------------  SLOT 7  ------------------

  // Update ACKs
  dl_h_var_3.ack = false;
  ul_h_var_3.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, bench.slot_tx(), dl_h_var_3, ul_h_var_3, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_3, bench.slot_tx(), dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, bench.slot_tx(), ul_h_var_3, h_entity);

  // ----------------  SLOT 8  ------------------

  // Update ACKs
  dl_h_var_0.ack = true;
  ul_h_var_0.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_0, ul_h_par_0, bench.slot_tx(), dl_h_var_0, ul_h_var_0, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_0, bench.slot_tx(), dl_h_var_0, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_0, bench.slot_tx(), ul_h_var_0, h_entity);

  // ----------------  SLOT 9  ------------------
  // Make PID 1 fail, this will trigger the packets to be discarded
  dl_h_var_1.ack = false;
  ul_h_var_1.ack = false;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_1, ul_h_par_1, bench.slot_tx(), dl_h_var_1, ul_h_var_1, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_1, bench.slot_tx(), dl_h_var_1, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_1, bench.slot_tx(), ul_h_var_1, h_entity);

  // ----------------  SLOT 10  ------------------

  // Update ACKs
  dl_h_var_2.ack = true;
  ul_h_var_2.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_2, ul_h_par_2, bench.slot_tx(), dl_h_var_2, ul_h_var_2, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_2, bench.slot_tx(), dl_h_var_2, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_2, bench.slot_tx(), ul_h_var_2, h_entity);

  // ----------------  SLOT 11  ------------------

  // Update ACKs
  dl_h_var_3.ack = true;
  ul_h_var_3.ack = true;

  // Report ACK
  h_entity_report_ack(common_param, dl_h_par_3, ul_h_par_3, bench.slot_tx(), dl_h_var_3, ul_h_var_3, h_entity);
  // Update Scheduler slot and clear HARQ processes, if needed
  bench.new_slot();
  // Test DL and UL HARQ separately
  test_dl_harq_entity_slot(common_param, dl_h_par_3, bench.slot_tx(), dl_h_var_3, h_entity);
  test_ul_harq_entity_slot(common_param, ul_h_par_3, bench.slot_tx(), ul_h_var_3, h_entity);
}

int main()
{
  // Initialize logger
  srslog::init();
  auto& harq_logger = srslog::fetch_basic_logger("MAC");
  harq_logger.set_level(srslog::basic_levels::info);
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);

  // Test DL functions (with invalid behaviour or operations not permitted)
  test_dl_invalid_paths(harq_logger);
  // Test UL functions (with invalid behaviour or operations not permitted)
  test_ul_invalid_paths(harq_logger);
  // Test h_entity scheduling over time
  test_harq(harq_logger);
}