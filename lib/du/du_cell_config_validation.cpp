/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/du/du_cell_config_validation.h"

using namespace srsgnb;

#define HANDLE_RETURN(cond)                                                                                            \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (ret.is_error()) {                                                                                              \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

#define CHECK_TRUE(cond, ...)                                                                                          \
  if (not(cond)) {                                                                                                     \
    return {fmt::format(__VA_ARGS__)};                                                                                 \
  }

#define CHECK_COMPARE_(val, expected_val, comparison, ...)                                                             \
  CHECK_TRUE(                                                                                                          \
      (val comparison expected_val), "Invalid {} ({} " #comparison "{})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_EQ(val, expected_val, ...)                                                                               \
  CHECK_TRUE((val == expected_val), "Invalid {} ({} != {})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_NEQ(val, forbidden_val, ...)                                                                             \
  CHECK_TRUE((val != forbidden_val), "Invalid {} ({} == {})", fmt::format(__VA_ARGS__), val, forbidden_val)

/// Checks if "val" is below or equal to "max_val".
#define CHECK_EQ_OR_BELOW(val, max_val, ...)                                                                           \
  CHECK_TRUE((val <= max_val), "Invalid {} ({} > {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is below "max_val".
#define CHECK_BELOW(val, max_val, ...)                                                                                 \
  CHECK_TRUE((val < max_val), "Invalid {} ({} >= {})", fmt::format(__VA_ARGS__), val, max_val)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper typedef.
using check_outcome = error_type<std::string>;

/// Checks whether CORESET#0 table index is valid as per TS38.213, Table 13-{1,...,10}.
static check_outcome is_coreset0_idx_valid(const du_cell_config& cell_cfg)
{
  CHECK_BELOW(cell_cfg.coreset0_idx, 15, "CORESET#0 index table");
  // TODO: Check which table to use.
  return {};
}

/// Checks whether CORESET#0 configuration matches the values specified in TS38.211-7.3.2.2.
static check_outcome is_coreset0_params_valid(const coreset_configuration& cs_cfg, pci_t pci)
{
  CHECK_TRUE(cs_cfg.duration >= 1, "Invalid CORESET#0 slot duration ({})", cs_cfg.duration);
  // Implicit, invariant values for CORESET#0 as per TS38.211-7.3.2.2.
  CHECK_EQ(cs_cfg.id, 0, "CORESET#0 ID");
  CHECK_TRUE(cs_cfg.interleaved.has_value(), "CORESET#0 must be interleaved");
  CHECK_EQ(cs_cfg.interleaved->interleaver_sz, 2, "CORESET#0 interleaver size (R)");
  CHECK_EQ(cs_cfg.interleaved->reg_bundle_sz, 6, "CORESET#0 REG Bundle size (L)");
  CHECK_TRUE(not cs_cfg.interleaved->shift_index.has_value(), "CORESET#0 shift index should be equal to PCI");
  CHECK_EQ(cs_cfg.precoder_granurality,
           coreset_configuration::precoder_granularity_type::same_as_reg_bundle,
           "CORESET#0 Precoder Granularity");
  return {};
}

static check_outcome check_dl_config_common(const du_cell_config& cell_cfg)
{
  const bwp_downlink_common& bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
  // PDCCH
  if (bwp.pdcch_common.coreset0.has_value()) {
    HANDLE_RETURN(is_coreset0_params_valid(*bwp.pdcch_common.coreset0, cell_cfg.pci));
  }
  if (bwp.pdcch_common.sib1_search_space_id != srsgnb::MAX_NOF_SEARCH_SPACES) {
    CHECK_EQ(bwp.pdcch_common.sib1_search_space_id, 0, "SearchSpaceSIB1 must be equal to 0 for initial DL BWP");
  }
  if (bwp.pdcch_common.common_coreset.has_value()) {
    CHECK_NEQ(bwp.pdcch_common.common_coreset->id, 0, "Common CORESET");
  }
  for (const search_space_configuration& ss : bwp.pdcch_common.search_spaces) {
    CHECK_NEQ(ss.duration, 0, "SearchSpace#{} duration", ss.id);
    CHECK_EQ(ss.type, search_space_configuration::common, "common SearchSpace#{} type", ss.id);
    if (ss.cs_id == 0) {
      CHECK_TRUE(
          bwp.pdcch_common.coreset0.has_value(), "common SearchSpace#{} points to CORESET#0 which is inactive", ss.id);
    } else {
      CHECK_TRUE(bwp.pdcch_common.common_coreset.has_value() and ss.cs_id == bwp.pdcch_common.common_coreset->id,
                 "common SearchSpace#{} points to CORESET#{} which is inactive",
                 ss.id,
                 ss.cs_id);
    }
  }
  // PDSCH
  CHECK_TRUE(not bwp.pdsch_common.pdsch_td_alloc_list.empty(), "Empty PDSCH-TimeDomainAllocationList");
  for (const auto& pdsch : bwp.pdsch_common.pdsch_td_alloc_list) {
    CHECK_EQ_OR_BELOW(pdsch.k0, 32, "PDSCH k0");
    // TODO: Remaining.
  }
  return {};
}

check_outcome srsgnb::is_du_cell_config_valid(const du_cell_config& cell_cfg)
{
  CHECK_EQ_OR_BELOW(cell_cfg.pci, MAX_PCI, "cell PCI");
  CHECK_EQ_OR_BELOW(cell_cfg.scs_common, subcarrier_spacing::kHz120, "SCS common");
  HANDLE_RETURN(is_coreset0_idx_valid(cell_cfg));
  HANDLE_RETURN(check_dl_config_common(cell_cfg));
  // TODO: Remaining.
  return {};
}
