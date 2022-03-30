#include "../../../lib/mac/sched/cell/phy_helpers.h"
#include "../../../lib/mac/sched/cell/ra_sched.h"
#include "config_generators.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/test_utils.h"
#include <list>
#include <map>

using namespace srsgnb;

const unsigned gnb_tx_delay       = 2;
const unsigned nof_prbs_per_rar   = 4;
const unsigned nof_prbs_per_msg3  = 3;
const unsigned nof_grants_per_rar = 16;

/// Helper function that computes the RA-RNTI from the rach_indication_message.
static uint16_t get_ra_rnti(const rach_indication_message& msg)
{
  // See 38.321, 5.1.3 - Random Access Preamble transmission
  uint16_t ra_rnti = 1 + msg.symbol_index + 14 * msg.slot_rx.slot_index() + 14 * 80 * msg.frequency_index;
  return ra_rnti;
};

/// Tests whether the fields in a RAR grant are consistent. Current tests:
/// - no repeated C-RNTIs as MSG3 grants.
/// - No collision in UL PRBs between MSG3 grants.
void test_rar_consistency(const cell_configuration& cfg, span<const rar_information> rars)
{
  prb_bitmap                                total_ul_prbs{cfg.nof_ul_prbs};
  circular_map<rnti_t, rnti_t, MAX_NOF_UES> crntis;

  for (const rar_information& rar : rars) {
    TESTASSERT(not rar.grants.empty(),
               "RAR corrsponding to RA-RNTI '{}' has no corresponding allocated MSG3 grants",
               rar.ra_rnti);
    for (const msg3_information& msg3 : rar.grants) {
      TESTASSERT(msg3.prbs.length() > 0,
                 "RAR corrsponding to RA-RNTI '{}' has no PRBs allocated for MSG3 grants",
                 rar.ra_rnti);
      TESTASSERT(not total_ul_prbs.any(msg3.prbs.start(), msg3.prbs.stop()));
      TESTASSERT(not crntis.contains(msg3.temp_crnti));

      total_ul_prbs.fill(msg3.prbs.start(), msg3.prbs.stop());
      crntis.insert(msg3.temp_crnti, msg3.temp_crnti);
    }
  }
}

/// Helper function that tests match between MSG3 grant with RACH Indication message.
/// \param rach_ind scheduled RACH indication.
/// \param msg3_grant MSG3 grant.
static void test_rach_ind_msg3_grant(const rach_indication_message& rach_ind, const msg3_information& msg3_grant)
{
  TESTASSERT_EQ(
      rach_ind.timing_advance, msg3_grant.ta, "Time-advance mismatch for MSG3 RAPID '{}'", rach_ind.preamble_id);
  TESTASSERT_EQ(rach_ind.crnti, msg3_grant.temp_crnti, "C-RNTI mismatch for MSG3 RAPID '{}'", rach_ind.preamble_id);
  TESTASSERT(msg3_grant.prbs.length() > 0);
}

/// Tests whether a RACH indication is served in RAR grant and verify RAR and Msg3 parameter consistency with RACH
/// indication. Current tests:
/// - RAR RAPID matches RA-RNTI.
/// \param cfg cell configuration.
/// \param rach_ind scheduled RACH indication.
/// \param rar RAR grant.
/// \return true if a Msg3 grant was found with the same TC-RNTI as that of RACH indication.
bool test_rach_ind_in_rar(const cell_configuration&      cfg,
                          const rach_indication_message& rach_ind,
                          const rar_information&         rar)
{
  uint16_t ra_rnti = get_ra_rnti(rach_ind);
  TESTASSERT_EQ(ra_rnti, rar.ra_rnti);

  for (const msg3_information& msg3 : rar.grants) {
    if (rach_ind.crnti == msg3.temp_crnti) {
      test_rach_ind_msg3_grant(rach_ind, msg3);
      return true;
    }
  }
  return false;
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  const du_bwp_id_t     bwp_id      = 0;
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  cell_configuration      cfg{make_cell_cfg_req()};
  cell_resource_grid_pool res_grid{cfg};
  cell_resource_allocator res_alloc{res_grid};

  void slot_indication(slot_point sl_tx)
  {
    mac_logger.set_context((sl_tx - gnb_tx_delay).to_uint());
    res_grid.slot_indication(sl_tx);
    test_logger.set_context((sl_tx - gnb_tx_delay).to_uint());
    test_logger.info("Starting new slot");
  }
};

/// Helper function that executes and tests the success of handle_rach_indication() for a rach_indication_message list.
void static enqueue_rach_indications(const std::list<rach_indication_message>& rach_ind_list,
                                     const slot_point&                         slot_rx_prach,
                                     ra_sched&                                 rach_sched)
{
  // Process all RACH indications received at sl_rx_prach
  for (auto& rach_ind_item : rach_ind_list) {
    if (rach_ind_item.slot_rx == slot_rx_prach) {
      TESTASSERT(rach_sched.handle_rach_indication(rach_ind_item));
    }
  }
}

/// Helper function that tests whether the scheduler has allocated the correct number of MSG3 grants.
/// \param cell_cfg cell configuration.
/// \param slot_rx slot at which the scheduler runs the allocation algorithm.
/// \param pending_rach_ind_list list of RACH indication messages that still need to be processed by the scheduler.
/// \param resource_grid scheduler resource grid with allocation.
static unsigned test_expected_nof_allocation(const cell_configuration&                 cell_cfg,
                                             const slot_point&                         slot_rx,
                                             const std::list<rach_indication_message>& pending_rach_ind_list,
                                             cell_resource_allocator&                  resource_grid)
{
  // Check how many MSG3 grant allocations have been scheduled
  slot_resource_allocator rar_alloc          = resource_grid[0];
  unsigned                nof_allocated_msg3 = 0;
  for (auto& rar : rar_alloc.dl_res().rars) {
    nof_allocated_msg3 += rar.grants.size();
  }

  // Find the number of RACH indication messages eligible to be allocated (rach_ind.slot_rx <= slot_rx)
  unsigned nof_eligible_rach_ind = 0;
  for (auto& rach_ind : pending_rach_ind_list) {
    if (rach_ind.slot_rx <= slot_rx) {
      nof_eligible_rach_ind++;
    }
  }

  // Compute the available resources to allocate RAR and MSG3 grants
  unsigned nof_available_grants =
      std::min(cell_cfg.nof_ul_prbs / nof_prbs_per_msg3, cell_cfg.nof_dl_prbs / nof_prbs_per_rar);

  // If there are more available grants than eligible RACH indications, then we expect the scheduler allocate exactly as
  // many MSG3 grants as RACH indications
  if (nof_available_grants >= nof_eligible_rach_ind) {
    TESTASSERT_EQ(nof_eligible_rach_ind, nof_allocated_msg3);
  } else {
    // If there are more eligible RACH indications than available grants, then we expect the scheduler allocate AT LEAST
    // as many MSG3 grants as RACH indications
    TESTASSERT(nof_allocated_msg3 >= nof_available_grants, "Less than the expected num. of MSG3 have been allocated ");
  }

  return nof_allocated_msg3;
}

/// Helper function that tests whether RARs have been allocated DL PRBs.
static void test_rar_general_allocation(cell_resource_allocator& resource_grid)
{
  slot_resource_allocator rar_alloc = resource_grid[0];

  if (rar_alloc.dl_res().rars.size() > 0) {
    TESTASSERT(rar_alloc.used_dl_prbs().count() > 0);
  }
}

/// This function receives a list of "rach_indication_message"s that can be received at different times slots and
/// verifies if the scheduler allocates the RAR and MSG3 grants for the rach_indication_message lists received at a
/// given slot.
/// \param [in] cell_cfg Cell configuration.
/// \param [in] slot_rx Slot for which the RACH requests (rach_indication_message) should have had the RAR/MSG3 grants
/// allocated by the scheduler.
/// \param [in] nof_allocations Number of MSG3 grants allocated by the scheduler.
/// \param [in] rach_ind_list List of RACH indication messges received by the GNB.
/// \param [in] resource_grid Resource allocator that gives this function access to the scheduling outcomes for
/// assessment.
static void test_per_ra_ranti_rapid_grants(const cell_configuration&                 cell_cfg,
                                           const slot_point&                         slot_rx,
                                           const unsigned                            nof_allocations,
                                           const std::list<rach_indication_message>& rach_ind_list,
                                           cell_resource_allocator&                  resource_grid)
{
  // This function, first, retrieves all RACH indications that have been received (and not yet allocated) in the
  // past up until slot_rx; among these RACH indications, only an expected number can be allocated by the
  // scheduler, depending on the available UL and DL resources. Second, it verifies if this number of RACH
  // indications have been allocated by the scheduler for slot_rx+gnb_tx_delay, which is pointed by resource_grid[0].

  // We group the rach_indication_messages in a map organized as follows:
  // - Each map element contains a list with all the unprocessed rach_indication_messages with common RA-RNTI
  // - In a list, rach_indication_messages have the same RA-RNTI, but different RAPIDs
  std::map<uint16_t, std::list<rach_indication_message> > rach_per_ra_rnti_map;

  // NOTE: We use a common counter as, for each RACH indication, under the assumptions above, 1 MAC subPDU (as part of
  // the RAR MAC PDU) and 1 MSG3 gets allocated, so the RBs occupancy of RAR and MSG3 grants scale with the number of
  // RACH indication messages.
  unsigned msg3_grant_cnt = 0;

  while (not rach_ind_list.empty()) {
    const rach_indication_message& rach_ind_to_process = rach_ind_list.front();
    // Look for RACH indications received up until slot_rx.
    if (rach_ind_to_process.slot_rx > slot_rx) {
      break;
    }
    uint16_t ra_rnti = get_ra_rnti(rach_ind_to_process);
    // If the map has no list for the RA-RNTI, build one
    if (rach_per_ra_rnti_map.find(ra_rnti) == rach_per_ra_rnti_map.end()) {
      rach_per_ra_rnti_map.emplace(ra_rnti, std::list<rach_indication_message>{});
    }
    // Copy the rach_indication_message to the RA-RNTI RACH list
    rach_per_ra_rnti_map[ra_rnti].push_back(rach_ind_to_process);
    msg3_grant_cnt++;
    // The iterator is moved from for( ; ; ) to here to avoid iterator invalidation due to popping from the list
    // Remove RACH indication from the input list
    if (msg3_grant_cnt >= nof_allocations) {
      break;
    }
  }

  // If the map is empty, there is nothing to check.
  if (rach_per_ra_rnti_map.empty()) {
    return;
  }

  slot_resource_allocator rar_alloc = resource_grid[0];

  // For each RA-RNTI RACH list, test the RACH indication and corresponding RAR/MSG3 grants
  for (const auto& it : rach_per_ra_rnti_map) {
    const auto&    rach_ra_rnti_list = it.second;
    const uint16_t ra_ranti          = it.first;

    // Define lambda to find RAR grant (from allocated RARs grants) with given RA-RNTI
    auto find_rar = [](const static_vector<rar_information, MAX_GRANTS>& rar_grants, uint16_t expected_ra_rnti) {
      for (auto rar_iter = rar_grants.begin(); rar_iter != rar_grants.end(); ++rar_iter) {
        if (rar_iter->ra_rnti == expected_ra_rnti) {
          return rar_iter;
        }
      }
      return rar_grants.end();
    };

    // Retrive RAR for given RA-RNTI from all RAR grants
    auto wanted_rar_ra_rnti = find_rar(rar_alloc.dl_res().rars, ra_ranti);

    // Per-RA-RANTI checks on RAR grant
    TESTASSERT(
        wanted_rar_ra_rnti != rar_alloc.dl_res().rars.end(), "RAR with corrsponding RA-RNTI '{}' not found", ra_ranti);
    TESTASSERT_EQ(rach_ra_rnti_list.front().cell_index,
                  wanted_rar_ra_rnti->cell_index,
                  "Cell-index mismatch for RAR corrsponding to RA-RNTI '{}'",
                  ra_ranti);

    // Define lambda to find MSG3 grant with given RAPID
    auto find_rapid = [](const static_vector<msg3_information, MAX_GRANTS>& msg3_grants, unsigned expected_rapid) {
      for (auto msg3_grant_iter = msg3_grants.begin(); msg3_grant_iter != msg3_grants.end(); ++msg3_grant_iter) {
        if (msg3_grant_iter->rapid == expected_rapid) {
          return msg3_grant_iter;
        }
      }
      return msg3_grants.end();
    };

    // Per-RAPID checks on MSG3 grants. For the RA-RNTI RACH list, test each MSG3 grant (MSG3 <--> RAPID)
    for (const auto& rach_ind_rapid : rach_ra_rnti_list) {
      unsigned rapid = rach_ind_rapid.preamble_id;

      // Retrieve MSG3 for RAPID for the corresponding to RACH indication message
      auto wanted_msg3_grant_rapid = find_rapid(wanted_rar_ra_rnti->grants, rapid);

      TESTASSERT(wanted_msg3_grant_rapid != wanted_rar_ra_rnti->grants.end(), "RAPID '{}' not found", rapid);

      test_rach_ind_msg3_grant(rach_ind_rapid, *wanted_msg3_grant_rapid);
    }
  }
}

/// Function that removes the RACH Indications that have been verified by the test function from the list.
static void remove_processed_rach_ind(const unsigned nof_allocations, std::list<rach_indication_message>& rach_ind_list)
{
  for (unsigned n = 0; n < nof_allocations and not rach_ind_list.empty(); ++n) {
    rach_ind_list.pop_front();
  }
}

/// This function verifies the correct scheduling of a RAR and Msg3 in an FDD frame, when multiple RACHs are received.
/// It processes a list of RACH indication messages for the same RA-RNTI and different RAPIDs.
/// The scheduler is expected to allocate a RAR and multiple MSG3 grants (as many as RACH indication messages).
void test_ra_sched_fdd_1_rar_multiple_msg3()
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - SINGLE RAR & MULTIPLE MSG3"};

  // We can increase the test length and create a list with RACH messages received at different time slots
  const unsigned nof_test_slots = 20;
  const unsigned slot_rx_prach  = 5;

  test_bench bench;
  ra_sched   ra_sch{bench.cfg};

  // Create the list of RACH indication messages with as many elements as the nof_grants_per_rar (1 RA-RNTI and
  // different RAPIDs)
  std::list<rach_indication_message> rach_ind_list;
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    uint16_t crnti = 0x4601 + n;
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 0,
                                                    .frequency_index = 0,
                                                    .preamble_id     = n});
  }

  for (unsigned t = 0; t < nof_test_slots; ++t) {
    // Slot at which MAC receives notification from PHY
    slot_point slot_rx{0, t};
    // Slot for which Scheduler schedules PHY DL transmission (RAR)
    slot_point slot_tx{0, t + gnb_tx_delay};

    // Update slot in the resource allocation grid pool
    bench.slot_indication(slot_tx);

    // Process RACH INDICATION MESSAGES, if any
    enqueue_rach_indications(rach_ind_list, slot_rx, ra_sch);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_alloc);

    // Test whether the scheduler has allocated the expected number of MSG3 grants
    unsigned nof_msg3_allocation = test_expected_nof_allocation(bench.cfg, slot_rx, rach_ind_list, bench.res_alloc);

    // Test whether the RAR info is consistent with cell_config and there are no duplicates C-RNTI
    test_rar_consistency(bench.cfg, bench.res_alloc[0].dl_res().rars);

    // Test whether the RAR has grants and PRBs allocated
    test_rar_general_allocation(bench.res_alloc);

    // Test the match between RACH indication messages and RAR/MSG3 grants
    test_per_ra_ranti_rapid_grants(bench.cfg, slot_rx, nof_msg3_allocation, rach_ind_list, bench.res_alloc);

    remove_processed_rach_ind(nof_msg3_allocation, rach_ind_list);
  }

  // Verify if the list of RACH indication message is empty, meaning all RACHs have been allocated
  TESTASSERT_EQ(
      true, rach_ind_list.empty(), "RACH indication list is not empty, meaning not all RACHs have been allocated");
}

/// This function verifies the correct scheduling of a RAR and Msg3 in an FDD frame, when multiple RACHs are received.
/// It processes a list of RACH indication messages for the different RA-RNTI and different RAPIDs.
/// The scheduler is expected to allocate several RAR (as many as RA-RNTIs) and multiple MSG3 grants (as many as RACH
/// indication messages).
void test_ra_sched_fdd_multiple_rar_multiple_msg3()
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - MULTIPLE RAR & MSG3"};
  // We can increase the test length and create a list with RACH messages received at different time slots
  const unsigned nof_test_slots = 20;
  const unsigned slot_rx_prach  = 5;

  test_bench bench;
  ra_sched   ra_sch{bench.cfg};

  // Create the list of RACH indication messages with different RA-RNTIs and different RAPIDs
  std::list<rach_indication_message> rach_ind_list;
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    uint16_t crnti = 0x4601 + n;
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 0,
                                                    .frequency_index = 0,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  uint16_t last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    uint16_t crnti = last_crnti + n;
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 2,
                                                    .frequency_index = 1,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    uint16_t crnti = last_crnti + n;
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 4,
                                                    .frequency_index = 2,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    uint16_t crnti = last_crnti + n;
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 6,
                                                    .frequency_index = 3,
                                                    .preamble_id     = n});
  }

  for (unsigned t = 0; t < nof_test_slots; ++t) {
    // Slot at which MAC receives notification from PHY
    slot_point slot_rx{0, t};
    // Slot for which Scheduler schedules PHY DL transmission (RAR)
    slot_point slot_tx{0, t + gnb_tx_delay};

    // Update slot in the resource allocation grid pool
    bench.slot_indication(slot_tx);

    // Process RACH INDICATION MESSAGES, if any
    enqueue_rach_indications(rach_ind_list, slot_rx, ra_sch);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_alloc);

    // Test whether the scheduler has allocated the expected number of MSG3 grants
    unsigned nof_msg3_allocation = test_expected_nof_allocation(bench.cfg, slot_rx, rach_ind_list, bench.res_alloc);

    // Test whether the RAR info is consistent with cell_config and there are no duplicates C-RNTI
    test_rar_consistency(bench.cfg, bench.res_alloc[0].dl_res().rars);

    // Test whether the RAR has grants and PRBs allocated
    test_rar_general_allocation(bench.res_alloc);

    // Test the match between RACH indication messages and RAR/MSG3 grants
    test_per_ra_ranti_rapid_grants(bench.cfg, slot_rx, nof_msg3_allocation, rach_ind_list, bench.res_alloc);

    // Remove the RACH indications that have been tested from the list of RACHs
    remove_processed_rach_ind(nof_msg3_allocation, rach_ind_list);
  }

  // Verify if the list of RACH indication message is empty, meaning all RACHs have been allocated
  TESTASSERT_EQ(
      true, rach_ind_list.empty(), "RACH indication list is not empty, meaning not all RACHs have been allocated");
}

/// Verify the correct scheduling of a RAR and Msg3 in an FDD frame, when a single RACH is received.
void test_ra_sched_fdd_single_rach()
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - SINGLE RACH"};
  test_bench          bench;

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

    unsigned        msg3_delay;
    symbol_interval symbols;
    get_msg3_delay(bench.cfg.ul_cfg_common, sl_tx, msg3_delay, symbols);

    // Update slot
    bench.slot_indication(sl_tx);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_alloc);

    if (sl_rx == prach_sl_rx) {
      // RAR allocated right after PRACH is detected
      slot_resource_allocator pdcch_sl_res = bench.res_alloc[0];
      slot_resource_allocator msg3_sl_res  = bench.res_alloc[msg3_delay];

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
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  test_ra_sched_fdd_single_rach();

  test_ra_sched_fdd_1_rar_multiple_msg3();

  test_ra_sched_fdd_multiple_rar_multiple_msg3();
}