
#include "../../../lib/mac/sched/cell/ra_sched.h"
#include "config_generators.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Tests whether the fields in a RAR grant are consistent. Current tests:
/// - no repeated C-RNTIs as MSG3 grants.
/// - No collision in UL PRBs between MSG3 grants.
void test_rar_consistency(const cell_configuration& cfg, span<const rar_information> rars)
{
  prb_bitmap                                total_ul_prbs{cfg.nof_ul_prbs};
  circular_map<rnti_t, rnti_t, MAX_NOF_UES> crntis;

  for (const rar_information& rar : rars) {
    TESTASSERT(not rar.grants.empty());
    for (const msg3_information& msg3 : rar.grants) {
      TESTASSERT(msg3.prbs.length() > 0);
      TESTASSERT(not total_ul_prbs.any(msg3.prbs.start(), msg3.prbs.stop()));
      TESTASSERT(not crntis.contains(msg3.temp_crnti));

      total_ul_prbs.fill(msg3.prbs.start(), msg3.prbs.stop());
      crntis.insert(msg3.temp_crnti, msg3.temp_crnti);
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
                          const rar_information&         rar)
{
  // See 38.321, 5.1.3 - Random Access Preamble transmission
  uint16_t ra_rnti =
      1 + rach_ind.symbol_index + 14 * rach_ind.slot_rx.slot_index() + 14 * 80 * rach_ind.frequency_index;
  TESTASSERT_EQ(ra_rnti, rar.ra_rnti);

  for (const msg3_information& msg3 : rar.grants) {
    if (rach_ind.crnti == msg3.temp_crnti) {
      TESTASSERT_EQ(rach_ind.timing_advance, msg3.ta);
      TESTASSERT_EQ(rach_ind.preamble_id, msg3.rapid);
      TESTASSERT(msg3.prbs.length() > 0);
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
  unsigned gnb_tx_delay = 2;

  test_bench bench;

  ra_sched ra_sch{bench.cfg};

  slot_point              prach_sl_rx{0, 7};
  rach_indication_message rach_ind = generate_rach_ind_msg(prach_sl_rx, 0x4601);

  slot_point sl_rx{0, gnb_tx_delay};
  for (unsigned sl_count = 0; sl_rx <= prach_sl_rx; ++sl_count) {
    slot_point sl_tx{sl_rx + gnb_tx_delay};

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
      rar_information& rar = pdcch_sl_res.dl_res().rars[0];

      // Msg3
      test_rach_ind_in_rar(bench.cfg, rach_ind, rar);
      TESTASSERT_EQ(1, rar.grants.size());
      TESTASSERT_EQ(rar.grants[0].prbs.length(), msg3_sl_res.used_ul_prbs().count());

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