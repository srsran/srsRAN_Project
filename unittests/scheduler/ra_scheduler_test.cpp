/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generators.h"
#include "lib/scheduler/cell/ra_scheduler.h"
#include "lib/scheduler/pdcch_scheduler/pdcch_scheduler.h"
#include "lib/scheduler/support/config_helpers.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsgnb/srslog/bundled/fmt/ranges.h"
#include "srsgnb/support/test_utils.h"
#include <list>
#include <map>
#include <set>

using namespace srsgnb;

const unsigned gnb_tx_delay       = 2;
const unsigned nof_prbs_per_rar   = 4;
const unsigned nof_prbs_per_msg3  = 3;
const unsigned nof_grants_per_rar = 16;

/// Struct used to convey parameters to the test functions
struct ra_sched_param {
  bool                 is_tdd = false;
  std::vector<uint8_t> k2_list{2};
};

class dummy_pdcch_scheduler : public pdcch_scheduler
{
public:
  pdcch_dl_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                           rnti_t                        rnti,
                                           search_space_id               ss_id,
                                           aggregation_level             aggr_lvl) override
  {
    TESTASSERT_EQ(ss_id, slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id);
    slot_alloc.result.dl.dl_pdcchs.emplace_back();
    slot_alloc.result.dl.dl_pdcchs.back().ctx.rnti = rnti;
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl,
                                          dci_dl_format                 dci_fmt) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl,
                                          dci_ul_format                 dci_fmt) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }
};

/// Tests whether the fields in a list of RAR grant are consistent. Current tests:
/// - No repeated RA-RNTIs across RAR grants and no repeated C-RNTIs across Msg3 grants.
/// - No collision in UL RBs between MSG3 grants.
/// - No collision in DL RBs between RARs.
/// - Consistent content in DCI of RARs (e.g. has to be 1_0, PRBs fall within CORESET#0 RB limits).
void test_rar_consistency(const cell_configuration& cfg, span<const rar_information> rars)
{
  prb_bitmap                 total_ul_prbs(cfg.nof_ul_prbs);
  prb_bitmap                 total_dl_prbs(cfg.nof_dl_prbs);
  std::set<rnti_t>           temp_crntis, ra_rntis;
  const pdsch_config_common& pdsch_cfg     = cfg.dl_cfg_common.init_dl_bwp.pdsch_common;
  const pusch_config_common& pusch_cfg     = *cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common;
  crb_interval               coreset0_lims = get_coreset0_crbs(cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  bwp_configuration          bwp_cfg       = cfg.dl_cfg_common.init_dl_bwp.generic_params;
  bwp_cfg.crbs                             = coreset0_lims;

  for (const rar_information& rar : rars) {
    rnti_t ra_rnti = rar.pdcch_cfg->ctx.rnti;
    TESTASSERT(not rar.grants.empty(), "RAR with RA-RNTI={:#x} has no corresponding MSG3 grants", ra_rnti);

    TESTASSERT(ra_rntis.count(ra_rnti) == 0, "Repeated RA-RNTI detected.");
    ra_rntis.insert(ra_rnti);

    // Test DCI content.
    TESTASSERT_EQ(dci_dl_rnti_config_type::ra_f1_0, rar.pdcch_cfg->dci.type);
    const dci_1_0_ra_rnti_configuration& f1_0 = rar.pdcch_cfg->dci.ra_f1_0;
    TESTASSERT(f1_0.time_resource < pdsch_cfg.pdsch_td_alloc_list.size());
    ra_frequency_type1_configuration ra_s_l   = ra_frequency_type1_from_riv(f1_0.N_rb_dl_bwp, f1_0.frequency_resource);
    prb_interval                     prbs     = {ra_s_l.start_vrb, ra_s_l.start_vrb + ra_s_l.length_vrb};
    crb_interval                     rar_crbs = prb_to_crb(bwp_cfg, prbs);
    TESTASSERT(coreset0_lims.contains(rar_crbs), "RAR outside of initial active DL BWP RB limits");

    // Verify no collisions in PDSCH.
    TESTASSERT(not total_dl_prbs.any(rar_crbs.start(), rar_crbs.stop()), "Collision between RAR RBs detected.");
    total_dl_prbs.fill(rar_crbs.start(), rar_crbs.stop());

    for (const rar_ul_grant& msg3 : rar.grants) {
      TESTASSERT(not msg3.prbs.empty(), "Msg3 with temp-CRNTI={:#x} has no RBs", msg3.temp_crnti);
      TESTASSERT(not total_ul_prbs.any(msg3.prbs.start(), msg3.prbs.stop()), "Msg3 UL PRB collision");
      TESTASSERT(temp_crntis.count(msg3.temp_crnti) == 0, "Repeated C-RNTI={:#x}", msg3.temp_crnti);
      TESTASSERT(msg3.time_resource_assignment < pusch_cfg.pusch_td_alloc_list.size());

      total_ul_prbs.fill(msg3.prbs.start(), msg3.prbs.stop());
      temp_crntis.insert(msg3.temp_crnti);
    }
  }
}

/// Helper function that tests match between MSG3 grant with RACH Indication message.
/// \param rach_ind scheduled RACH indication.
/// \param msg3_grant MSG3 grant.
static void test_rach_ind_msg3_grant(const rach_indication_message& rach_ind, const rar_ul_grant& msg3_grant)
{
  TESTASSERT_EQ(
      rach_ind.timing_advance, msg3_grant.ta, "Time-advance mismatch for MSG3 RAPID '{}'", rach_ind.preamble_id);
  TESTASSERT_EQ(rach_ind.crnti, msg3_grant.temp_crnti, "C-RNTI mismatch for MSG3 RAPID '{}'", rach_ind.preamble_id);
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
  TESTASSERT_EQ(ra_rnti, rar.pdcch_cfg->ctx.rnti);

  for (const rar_ul_grant& msg3 : rar.grants) {
    if (rach_ind.crnti == msg3.temp_crnti) {
      test_rach_ind_msg3_grant(rach_ind, msg3);
      return true;
    }
  }
  return false;
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  const bwp_id_t        bwp_id      = to_bwp_id(0);
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  cell_configuration      cfg;
  cell_resource_allocator res_grid;
  dummy_pdcch_scheduler   pdcch_sch;

  test_bench() : cfg(make_cell_cfg_req()), res_grid(cfg) {}

  test_bench(const ra_sched_param& paramters) : cfg(make_cell_cfg_request(paramters)), res_grid(cfg){};

  void slot_indication(slot_point sl_tx)
  {
    mac_logger.set_context(sl_tx.to_uint());
    test_logger.set_context(sl_tx.to_uint());
    test_logger.info("Starting new slot {}", sl_tx);
    res_grid.slot_indication(sl_tx);
  }

  static sched_cell_configuration_request_message make_cell_cfg_request(const ra_sched_param& params)
  {
    sched_cell_configuration_request_message cfg_req = make_cell_cfg_req();
    auto& pusch_list = cfg_req.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    pusch_list.resize(params.k2_list.size());
    for (unsigned i = 0; i < params.k2_list.size(); ++i) {
      pusch_list[i].k2       = params.k2_list[i];
      pusch_list[i].symbols  = {0, 14};
      pusch_list[i].map_type = pusch_time_domain_resource_allocation::mapping_type::typeA;
    }
    if (params.is_tdd) {
      cfg_req.tdd_ul_dl_cfg_common = test_helpers::make_default_tdd_ul_dl_config_common();
    }
    return cfg_req;
  }
};

/// Helper function that executes and tests the success of handle_rach_indication() for a rach_indication_message list.
void static enqueue_rach_indications(const std::list<rach_indication_message>& rach_ind_list,
                                     const slot_point&                         slot_rx_prach,
                                     ra_scheduler&                             rach_sched)
{
  // Process all RACH indications received at sl_rx_prach
  for (auto& rach_ind_item : rach_ind_list) {
    if (rach_ind_item.slot_rx == slot_rx_prach) {
      rach_sched.handle_rach_indication(rach_ind_item);
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
  cell_slot_resource_allocator& rar_alloc          = resource_grid[0];
  unsigned                      nof_allocated_msg3 = 0;
  for (auto& rar : rar_alloc.result.dl.rar_grants) {
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
  nof_available_grants =
      std::min(nof_available_grants,
               get_coreset_nof_prbs(*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0) / nof_prbs_per_rar);

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
  cell_slot_resource_allocator& rar_alloc = resource_grid[0];

  if (rar_alloc.result.dl.rar_grants.size() > 0) {
    TESTASSERT(rar_alloc.dl_res_grid.sch_crbs(rar_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
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

  cell_slot_resource_allocator& rar_alloc = resource_grid[0];

  // For each RA-RNTI RACH list, test the RACH indication and corresponding RAR/MSG3 grants
  for (const auto& it : rach_per_ra_rnti_map) {
    const auto&    rach_ra_rnti_list = it.second;
    const uint16_t ra_ranti          = it.first;

    // Define lambda to find RAR grant (from allocated RARs grants) with given RA-RNTI
    auto find_rar = [](const static_vector<rar_information, MAX_GRANTS>& rar_grants, uint16_t expected_ra_rnti) {
      for (auto rar_iter = rar_grants.begin(); rar_iter != rar_grants.end(); ++rar_iter) {
        if (rar_iter->pdcch_cfg->ctx.rnti == expected_ra_rnti) {
          return rar_iter;
        }
      }
      return rar_grants.end();
    };

    // Retrive RAR for given RA-RNTI from all RAR grants
    auto wanted_rar_ra_rnti = find_rar(rar_alloc.result.dl.rar_grants, ra_ranti);

    // Per-RA-RANTI checks on RAR grant
    TESTASSERT(wanted_rar_ra_rnti != rar_alloc.result.dl.rar_grants.end(),
               "RAR with corrsponding RA-RNTI '{}' not found",
               ra_ranti);

    // Define lambda to find MSG3 grant with given RAPID
    auto find_rapid = [](const static_vector<rar_ul_grant, MAX_GRANTS>& msg3_grants, unsigned expected_rapid) {
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
void test_ra_sched_fdd_1_rar_multiple_msg3(const ra_sched_param& params)
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - SINGLE RAR & MULTIPLE MSG3, K2={}", params.k2_list};

  // We can increase the test length and create a list with RACH messages received at different time slots
  const unsigned nof_test_slots = 20;
  const unsigned slot_rx_prach  = 5;

  test_bench bench{params};

  ra_scheduler ra_sch{bench.cfg, bench.pdcch_sch};

  // Create the list of RACH indication messages with as many elements as the nof_grants_per_rar (1 RA-RNTI and
  // different RAPIDs)
  std::list<rach_indication_message> rach_ind_list;
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    rnti_t crnti = to_rnti(0x4601 + n);
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
    ra_sch.run_slot(bench.res_grid);

    // Test whether the scheduler has allocated the expected number of MSG3 grants
    unsigned nof_msg3_allocation = test_expected_nof_allocation(bench.cfg, slot_rx, rach_ind_list, bench.res_grid);

    // Test whether the RAR info is consistent with cell_config and there are no duplicates C-RNTI
    test_rar_consistency(bench.cfg, bench.res_grid[0].result.dl.rar_grants);

    // Test whether the RAR has grants and PRBs allocated
    test_rar_general_allocation(bench.res_grid);

    // Test the match between RACH indication messages and RAR/MSG3 grants
    test_per_ra_ranti_rapid_grants(bench.cfg, slot_rx, nof_msg3_allocation, rach_ind_list, bench.res_grid);

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
void test_ra_sched_fdd_multiple_rar_multiple_msg3(const ra_sched_param& params)
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - MULTIPLE RAR & MSG3, K2={}", params.k2_list};
  // We can increase the test length and create a list with RACH messages received at different time slots
  const unsigned nof_test_slots = 20;
  const unsigned slot_rx_prach  = 5;

  test_bench bench{params};

  ra_scheduler ra_sch{bench.cfg, bench.pdcch_sch};

  // Create the list of RACH indication messages with different RA-RNTIs and different RAPIDs
  std::list<rach_indication_message> rach_ind_list;
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    rnti_t crnti = to_rnti(0x4601 + n);
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 0,
                                                    .frequency_index = 0,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  uint16_t last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    rnti_t crnti = to_rnti(last_crnti + n);
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 2,
                                                    .frequency_index = 1,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    rnti_t crnti = to_rnti(last_crnti + n);
    rach_ind_list.push_back(rach_indication_message{.crnti           = crnti,
                                                    .slot_rx         = slot_point{0, slot_rx_prach},
                                                    .symbol_index    = 4,
                                                    .frequency_index = 2,
                                                    .preamble_id     = n});
  }

  // Add nof_grants_per_rar RACH indication for a second RA-RNTI
  last_crnti = 0x4601 + rach_ind_list.size();
  for (unsigned n = 0; n < nof_grants_per_rar; n++) {
    rnti_t crnti = to_rnti(last_crnti + n);
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
    ra_sch.run_slot(bench.res_grid);

    // Test whether the scheduler has allocated the expected number of MSG3 grants
    unsigned nof_msg3_allocation = test_expected_nof_allocation(bench.cfg, slot_rx, rach_ind_list, bench.res_grid);

    // Test whether the RAR info is consistent with cell_config and there are no duplicates C-RNTI
    test_rar_consistency(bench.cfg, bench.res_grid[0].result.dl.rar_grants);

    // Test whether the RAR has grants and PRBs allocated
    test_rar_general_allocation(bench.res_grid);

    // Test the match between RACH indication messages and RAR/MSG3 grants
    test_per_ra_ranti_rapid_grants(bench.cfg, slot_rx, nof_msg3_allocation, rach_ind_list, bench.res_grid);

    // Remove the RACH indications that have been tested from the list of RACHs
    remove_processed_rach_ind(nof_msg3_allocation, rach_ind_list);
  }

  // Verify if the list of RACH indication message is empty, meaning all RACHs have been allocated
  TESTASSERT_EQ(
      true, rach_ind_list.empty(), "RACH indication list is not empty, meaning not all RACHs have been allocated");
}

/// Verify the correct scheduling of a RAR and Msg3 in an FDD frame, when a single RACH is received.
void test_ra_sched_fdd_single_rach(const ra_sched_param& params)
{
  test_delimit_logger delimiter{"RA SCHEDULER - FDD - SINGLE RACH"};
  test_bench          bench{params};

  ra_scheduler ra_sch{bench.cfg, bench.pdcch_sch};

  slot_point              prach_sl_rx{0, 7};
  rach_indication_message rach_ind = generate_rach_ind_msg(prach_sl_rx, to_rnti(0x4601));

  slot_point sl_rx{0, gnb_tx_delay};
  for (unsigned sl_count = 0; sl_rx <= prach_sl_rx; ++sl_count) {
    slot_point sl_tx{sl_rx + gnb_tx_delay};

    if (sl_rx == prach_sl_rx) {
      // Enqueue PRACH
      ra_sch.handle_rach_indication(rach_ind);
    }

    unsigned msg3_delay = get_msg3_delay(bench.cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0],
                                         bench.cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);

    // Update slot
    bench.slot_indication(sl_tx);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_grid);

    if (sl_rx == prach_sl_rx) {
      // RAR allocated right after PRACH is detected
      cell_slot_resource_allocator& pdcch_sl_res = bench.res_grid[0];
      cell_slot_resource_allocator& msg3_sl_res  = bench.res_grid[msg3_delay];

      TESTASSERT(pdcch_sl_res.dl_res_grid.sch_crbs(pdcch_sl_res.cfg.dl_cfg_common.init_dl_bwp.generic_params).any());

      TESTASSERT_EQ(1, pdcch_sl_res.result.dl.rar_grants.size());
      test_rar_consistency(bench.cfg, pdcch_sl_res.result.dl.rar_grants);

      // RAR
      rar_information& rar = pdcch_sl_res.result.dl.rar_grants[0];

      // Msg3
      test_rach_ind_in_rar(bench.cfg, rach_ind, rar);
      TESTASSERT_EQ(1, rar.grants.size());
      prb_interval prbs = rar.grants[0].prbs;

      TESTASSERT_EQ(
          prbs.length(),
          msg3_sl_res.ul_res_grid.sch_crbs(pdcch_sl_res.cfg.ul_cfg_common.init_ul_bwp.generic_params).count());

    } else {
      TESTASSERT(
          bench.res_grid[0].dl_res_grid.sch_crbs(bench.res_grid.cfg.dl_cfg_common.init_dl_bwp.generic_params).none());
    }

    sl_rx++;
  }
}

/// In this test, we check for the TDD case:
/// - the scheduler is able to choose a valid K2 to schedule Msg3. The K2 value needs to fall in a valid TDD UL slot.
void test_ra_sched_tdd_single_rach()
{
  test_delimit_logger delimiter{"RA SCHEDULER - TDD - SINGLE RACH"};

  ra_sched_param params{.is_tdd = true, .k2_list{1, 3}}; // Msg3 delays 3 and 5.
  test_bench     bench{params};

  // TDD configuration.
  ra_scheduler ra_sch{bench.cfg, bench.pdcch_sch};

  // Enqueue RACH.
  slot_point              prach_sl_rx{0, 7};
  rach_indication_message rach_ind = generate_rach_ind_msg(prach_sl_rx, to_rnti(0x4601));
  ra_sch.handle_rach_indication(rach_ind);

  slot_point rar_sl;
  slot_point sl_rx = prach_sl_rx;
  for (unsigned sl_count = 0; sl_count <= 1000 and not rar_sl.valid(); ++sl_count, ++sl_rx) {
    // Update slot
    slot_point sl_tx{sl_rx + gnb_tx_delay};
    bench.slot_indication(sl_tx);

    // Run RA scheduler
    ra_sch.run_slot(bench.res_grid);

    // In case slot_tx corresponds to a TDD UL slot, ensure no RARs are allocated.
    bool is_dl_slot = slot_is_dl(*bench.cfg.tdd_cfg_common, sl_tx);
    if (not is_dl_slot) {
      TESTASSERT_EQ(0, bench.res_grid[0].result.dl.rar_grants.size());
      continue;
    }

    // Find a Msg3 delay for which the respective PUSCH slot corresponds to a valid TDD UL slot.
    unsigned msg3_delay = 0;
    bool     is_ul_slot = false;
    for (const auto& pusch : bench.cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
      msg3_delay = get_msg3_delay(pusch, bench.cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);
      is_ul_slot = slot_is_ul(*bench.cfg.tdd_cfg_common, sl_tx + msg3_delay);
      if (is_ul_slot) {
        break;
      }
    }
    if (not is_ul_slot) {
      // No valid PUSCH for Msg3 was found.
      TESTASSERT_EQ(0, bench.res_grid[0].result.dl.rar_grants.size());
      continue;
    }
    bench.test_logger.info("Expecting RAR at slot={} and Msg3 at slot={}", sl_tx, sl_tx + msg3_delay);

    // RAR allocated right after PRACH is detected
    cell_slot_resource_allocator& pdcch_sl_res = bench.res_grid[0];
    cell_slot_resource_allocator& msg3_sl_res  = bench.res_grid[msg3_delay];

    TESTASSERT(pdcch_sl_res.dl_res_grid.sch_crbs(pdcch_sl_res.cfg.dl_cfg_common.init_dl_bwp.generic_params).any());

    TESTASSERT_EQ(1, pdcch_sl_res.result.dl.rar_grants.size());
    test_rar_consistency(bench.cfg, pdcch_sl_res.result.dl.rar_grants);

    // RAR
    rar_information& rar = pdcch_sl_res.result.dl.rar_grants[0];

    // Msg3
    test_rach_ind_in_rar(bench.cfg, rach_ind, rar);
    TESTASSERT_EQ(1, rar.grants.size());
    prb_interval prbs = rar.grants[0].prbs;
    TESTASSERT_EQ(prbs.length(),
                  msg3_sl_res.ul_res_grid.sch_crbs(pdcch_sl_res.cfg.ul_cfg_common.init_ul_bwp.generic_params).count());
    TESTASSERT_EQ(1, msg3_sl_res.result.ul.puschs.size());

    rar_sl = sl_tx;
  }
  slot_point expected_rar_sl{0, 11};
  TESTASSERT_EQ(expected_rar_sl, rar_sl);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  {
    // Test scheduler for single RACH FDD.
    ra_sched_param parameters{.k2_list{1}};
    test_ra_sched_fdd_single_rach(parameters);
  }

  {
    // Test scheduler for single RACH and TDD.
    test_ra_sched_tdd_single_rach();
  }

  // Test scheduler for multiple RACH (single RAR, multiple MSG3) for different k2 parameter (MSG3 delay)
  for (uint8_t k2 : {1, 2, 3, 4}) {
    ra_sched_param parameters{.k2_list{k2}};
    test_ra_sched_fdd_1_rar_multiple_msg3(parameters);
  }

  // Test scheduler for multiple RACH (multiple RAR, multiple MSG3) for different k2 parameter (MSG3 delay)
  for (uint8_t k2 : {1, 2, 3, 4}) {
    ra_sched_param parameters{.k2_list{k2}};
    test_ra_sched_fdd_multiple_rar_multiple_msg3(parameters);
  }

  return 0;
}