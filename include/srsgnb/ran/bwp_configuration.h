
#ifndef SRSGNB_BWP_CONFIGURATION_H
#define SRSGNB_BWP_CONFIGURATION_H

#include "ofdm_symbol_range.h"
#include "resource_block.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/ran/prb_grant.h"
#include <bitset>

namespace srsgnb {

/// \remark See TS 38.331, --TAG-CONTROLRESOURCESET-START.
const size_t CORESET_FREQ_DOMAIN_RESOURCE_SIZE = 45;

/// Max number of OFDM symbols in a control resource set.
/// \remark See TS 38.331, "maxCoReSetDuration".
const size_t MAX_CORESET_DURATION = 3;

/// \brief CORESET identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofControlResourceSets" CORESETS configured.
/// \remark See TS 38.331, "maxNrofControlResourceSets".
enum coreset_id : uint8_t { MIN_CORESET_ID = 0, MAX_CORESET_ID = 11, MAX_NOF_CORESETS = 12 };

inline coreset_id to_coreset_id(unsigned cs_id)
{
  return static_cast<coreset_id>(cs_id);
}

/// \brief Search Space identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofControlResourceSets" CORESETS configured.
enum search_space_id : uint8_t { MIN_SEARCH_SPACE_ID = 0, MAX_SEARCH_SPACE = 39, MAX_NOF_SEARCH_SPACES = 40 };

inline search_space_id to_search_space_id(unsigned ss_id)
{
  return static_cast<search_space_id>(ss_id);
}

/// CORESET configuration.
/// \remark See TS 38.331, --TAG-CONTROLRESOURCESET-START.
struct coreset_configuration {
  struct interleaved_mapping_type {
    /// Values: (2, 3, 6).
    unsigned reg_bundle_sz;
    /// Values: (2, 3, 6).
    unsigned interleaver_sz;
    /// Values: (0..MAX_NOF_PRBS-1).
    optional<unsigned> shift_index;
  };
  enum class precoder_granularity_type { same_as_reg_bundle, all_contiguous_rbs };

  coreset_id                                     id;
  std::bitset<CORESET_FREQ_DOMAIN_RESOURCE_SIZE> freq_domain_resources;
  /// Duration in number of symbols. Values: (1..maxCORESETDuration).
  unsigned                           duration;
  optional<interleaved_mapping_type> interleaved;
  precoder_granularity_type          precoder_granurality;
};

struct search_space_configuration {
  search_space_id id;
  coreset_id      cs_id;
};

struct pdcch_configuration_common {
  /// May Contain CORESET#0 and common CORESET.
  slot_vector<coreset_configuration>      coresets;
  slot_vector<search_space_configuration> search_spaces;
  search_space_id                         sib1_search_space_id;
  search_space_id                         other_si_search_space_id;
  search_space_id                         paging_search_space_id;
  search_space_id                         ra_search_space_id;
};

struct bwp_configuration {
  bool               cp_present;
  subcarrier_spacing scs;
  prb_interval       prbs;
};

struct bwp_downlink_common {
  bwp_configuration          generic_params;
  pdcch_configuration_common pdcch_common;
};

/// Downlink Configuration, common to the serving cell.
/// \remark See TS 38.331, "DownlinkConfigCommonSIB".
struct dl_configuration_common {
  bwp_downlink_common init_dl_bwp;
};

} // namespace srsgnb

#endif // SRSGNB_BWP_CONFIGURATION_H
