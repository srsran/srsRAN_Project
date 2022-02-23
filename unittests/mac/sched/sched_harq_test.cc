
#include "../../../lib/mac/sched/sched_harq.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

constexpr uint32_t max_n_rtx  = 1;
constexpr uint32_t max_n_pids = 4;

struct harq_proc_param {
  uint32_t pid;
  bool     ndi   = false;
  uint32_t n_rtx = 0;
  uint32_t mcs   = 0;
  uint32_t tbs   = 0;
  bool     acks[max_n_rtx]{false};
};

struct harq_time_elem {
  uint32_t pid_to_schedule = 0;
  bool     ack             = true;
};

void test_harq()
{
  uint32_t pids_to_schedule[10]       = { 0, 1, 2, 3, 0, 1, 2, 3, 4 }
  //struct harq_time_elem time_harq_vector[10];
  time_harq_vector[0].pid_to_schedule = 1;
  time_harq_vector[1].pid_to_schedule = 1;

  // create dl HARQ process
  dl_harq_proc dl_h{0, 52};
  TESTASSERT(dl_h.empty());

  // create ul HARQ process
  ul_harq_proc ul_h{0, 52};
  TESTASSERT(ul_h.empty());

  srslog::init();
  auto& harq_logger = srslog::fetch_basic_logger("MAC-NR");
  harq_logger.set_level(srslog::basic_levels::info);
  // create HARQ entity
  harq_entity h_entity = harq_entity(0x04601, 52, 4, harq_logger);
  h_entity.new_slot(1);

  dl_harq_proc* dl_harq_pic_0 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_0 != nullptr);

  rbg_bitmap rgb_btmp(18);
  rgb_btmp.set(1);
  rgb_btmp.set(5);
  rgb_btmp.set(9);
  rgb_btmp.set(13);
  rgb_btmp.set(17);

  prb_grant       grant_dl[max_n_pids];
  struct dci_dl_t dci_dl[max_n_pids];
  for (uint32_t n; n < max_n_pids; n++) {
    grant_dl[n] = rgb_btmp;
  }

  dl_harq_proc* dl_harq_proc;

  for (uint32_t t; t < 20; t++) {
    h_entity.new_slot(t);

    if (t < max_n_pids) {
      dl_harq_proc* dl_harq_proc = h_entity.find_empty_dl_harq();
    }
  }

  // SLOT 0:
  // Add DL HARQ pid 0
  int t = 0;
  h_entity.new_slot(t);

  prb_grant       grant_dl_h_pic_0{rgb_btmp};
  const int       mcs_pic_0    = 17;
  const uint32_t  tbs_dl_pic_0 = 3152;
  struct dci_dl_t dci_pic_0;
  dl_harq_pic_0->set_mcs(mcs_pic_0);
  dl_harq_pic_0->new_tx(t, t + 4, grant_dl_h_pic_0, mcs_pic_0, 4, dci_pic_0);
  dl_harq_pic_0->set_tbs(tbs_dl_pic_0);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_0.mcs == mcs_pic_0 and dci_pic_0.ndi == true and dci_pic_0.pid == 0);
  TESTASSERT(dl_harq_pic_0->mcs() == mcs_pic_0 and dl_harq_pic_0->tbs() == tbs_dl_pic_0 and
             dl_harq_pic_0->ndi() == true);

  // Add DL HARQ pid 1
  t = 1;
  h_entity.new_slot(t);
  dl_harq_proc* dl_harq_pic_1 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_1 != nullptr);
  prb_grant       grant_dl_h_pic_1{rgb_btmp};
  const int       mcs_pic_1 = 11;
  struct dci_dl_t dci_pic_1;
  dl_harq_pic_1->set_mcs(mcs_pic_1);
  dl_harq_pic_1->new_tx(t, t + 4, grant_dl_h_pic_1, mcs_pic_1, 4, dci_pic_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_1.mcs == mcs_pic_1 and dci_pic_1.ndi == true and dci_pic_1.pid == 1);
  dl_harq_pic_1->set_tbs(3152);
  TESTASSERT(h_entity.find_pending_dl_retx() == nullptr);
  TESTASSERT(h_entity.find_empty_dl_harq() != nullptr);

  // Add DL HARQ pid 2
  t = 2;
  h_entity.new_slot(t);
  dl_harq_proc* dl_harq_pic_2 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_2 != nullptr);
  prb_grant       grant_dl_h_pic_2{rgb_btmp};
  const int       mcs_pic_2 = 13;
  struct dci_dl_t dci_pic_2;
  dl_harq_pic_2->set_mcs(mcs_pic_2);
  dl_harq_pic_2->new_tx(t, t + 4, grant_dl_h_pic_2, mcs_pic_2, 4, dci_pic_2);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_2.mcs == mcs_pic_2 and dci_pic_2.ndi == true and dci_pic_2.pid == 2);
  dl_harq_pic_1->set_tbs(3152);

  // Add DL HARQ pid 3
  t = 3;
  h_entity.new_slot(t);
  dl_harq_proc* dl_harq_pic_3 = h_entity.find_empty_dl_harq();
  TESTASSERT(dl_harq_pic_3 != nullptr);
  prb_grant       grant_dl_h_pic_3{rgb_btmp};
  const int       mcs_pic_3 = 13;
  struct dci_dl_t dci_pic_3;
  dl_harq_pic_3->set_mcs(mcs_pic_3);
  dl_harq_pic_3->new_tx(t, t + 4, grant_dl_h_pic_3, mcs_pic_3, 4, dci_pic_3);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_3.mcs == mcs_pic_3 and dci_pic_3.ndi == true and dci_pic_3.pid == 3);
  dl_harq_pic_1->set_tbs(3152);

  // Add DL HARQ pid
  t = 4;
  h_entity.new_slot(t);
  h_entity.dl_ack_info(0, 0, true);
  TESTASSERT(h_entity.find_empty_dl_harq() == dl_harq_pic_0);
  dl_harq_pic_0->set_mcs(mcs_pic_0);
  dl_harq_pic_0->new_tx(t, t + 4, grant_dl_h_pic_0, mcs_pic_0, 4, dci_pic_0);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_0.mcs == mcs_pic_0 and dci_pic_0.ndi == false and dci_pic_0.pid == 0);
  dl_harq_pic_1->set_tbs(3152);

  // Add DL HARQ pid
  t = 5;
  h_entity.new_slot(t);
  h_entity.dl_ack_info(1, 0, false);
  TESTASSERT(h_entity.find_empty_dl_harq() == nullptr);
  dl_harq_pic_1->new_retx(t, t + 4, grant_dl_h_pic_1, dci_pic_1);

  // test whether DCI has been filled correctly
  TESTASSERT(dci_pic_1.mcs == mcs_pic_1 and dci_pic_1.ndi == true and dci_pic_1.pid == 1);
  dl_harq_pic_1->set_tbs(3152);
}

int main()
{
  test_harq();
}