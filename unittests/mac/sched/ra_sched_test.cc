
#include "../../../lib/mac/sched/cell/ra_sched.h"
#include "config_generators.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Tests whether the fields in a RAR grant are consistent. Current tests:
/// - no repeated C-RNTIs as MSG3 grants.
/// - No collision in UL PRBs between MSG3 grants.
void test_rar_consistency(const cell_configuration& cfg, span<const rar_alloc_info> rars)
{
  prb_bitmap                                total_ul_prbs{cfg.nof_ul_prbs};
  circular_map<rnti_t, rnti_t, MAX_NOF_UES> crntis;

  for (const rar_alloc_info& rar : rars) {
    TESTASSERT(not rar.msg3s.empty());
    for (const msg3_freq_alloc& msg3 : rar.msg3s) {
      TESTASSERT(msg3.prbs.length() > 0);
      TESTASSERT(not total_ul_prbs.any(msg3.prbs.start(), msg3.prbs.stop()));
      TESTASSERT(not crntis.contains(msg3.tc_rnti));

      total_ul_prbs.fill(msg3.prbs.start(), msg3.prbs.stop());
      crntis.insert(msg3.tc_rnti, msg3.tc_rnti);
    }
  }
}

/// Tests whether a RACH indication is served in RAR grant and verify RAR and Msg3 parameter consistency with RACH
/// indication. Current tests:
/// - RAR RAPID matches RA-RNTI
/// \param cfg cell configuration
/// \param rach_ind scheduled RACH indication
/// \param rar RAR grant
/// \return true if a Msg3 grant was found with the same TC-RNTI as that of RACH indication.
bool test_rach_ind_in_rar(const cell_configuration&      cfg,
                          const rach_indication_message& rach_ind,
                          const rar_alloc_info&          rar)
{
  uint16_t ra_rnti =
      1 + rach_ind.symbol_index + 14 * rach_ind.slot_rx.slot_index() + 14 * 80 * rach_ind.frequency_index;
  TESTASSERT_EQ(ra_rnti, rar.rapid);
  TESTASSERT(rar.ta >= rach_ind.timing_advance); // check

  for (const msg3_freq_alloc& msg3 : rar.msg3s) {
    if (rach_ind.crnti == msg3.tc_rnti) {
      return true;
    }
  }
  return false;
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods
struct test_bench {
  const du_bwp_id_t     bwp_id     = 0;
  srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");

  cell_configuration      cfg{make_cell_cfg_req()};
  cell_resource_grid_pool res_grid{cfg};
  cell_resource_allocator res_alloc{res_grid};

  void slot_indication(slot_point sl_tx)
  {
    mac_logger.set_context(sl_tx.to_uint());
    res_grid.slot_indication(sl_tx);
  }
};

/// Verify the correct scheduling of a RAR and Msg3 in an FDD frame, when a single RACH is received.
void test_ra_sched_fdd_single_rach()
{
  unsigned tx_delay = 4;

  test_bench bench;

  cell_configuration cell_cfg{make_cell_cfg_req()};
  ra_sched           ra_sch{cell_cfg};

  slot_point              prach_sl_rx{0, 5};
  rach_indication_message rach_ind = generate_rach_ind_msg(prach_sl_rx, 0x4601);

  slot_point sl_rx{0, tx_delay};
  for (unsigned sl_count = 0; sl_count < 50; ++sl_count) {
    slot_point sl_tx{sl_rx + tx_delay};

    if (sl_rx == prach_sl_rx) {
      // Enqueue PRACH
      TESTASSERT(ra_sch.handle_rach_indication(rach_ind));
    }

    // Update slot
    bench.slot_indication(sl_tx);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_alloc);

    if (sl_rx == prach_sl_rx) {
      // RAR allocated right after PRACH is detected
      slot_resource_allocator pdcch_sl_res = bench.res_alloc[0];
      slot_resource_allocator msg3_sl_res  = bench.res_alloc[6];

      TESTASSERT(pdcch_sl_res.used_dl_prbs().count() > 0);

      TESTASSERT_EQ(1, pdcch_sl_res.dl_res().rars.size());
      test_rar_consistency(bench.cfg, pdcch_sl_res.dl_res().rars);

      // RAR
      rar_alloc_info& rar = pdcch_sl_res.dl_res().rars[0];

      // Msg3
      test_rach_ind_in_rar(bench.cfg, rach_ind, rar);
      TESTASSERT_EQ(1, rar.msg3s.size());
      TESTASSERT_EQ(rar.msg3s[0].prbs.length(), msg3_sl_res.used_ul_prbs().count());

    } else {
      TESTASSERT(bench.res_alloc[0].used_dl_prbs().count() == 0);
    }

    sl_rx++;
  }
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_ra_sched_fdd_single_rach();
}