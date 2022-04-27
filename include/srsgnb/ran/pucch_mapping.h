
#ifndef SRSGNB_RAN_PUCCH_MAPPING_H
#define SRSGNB_RAN_PUCCH_MAPPING_H

namespace srsgnb {

/// \brief Configuration of group and sequence hopping as described in TS38.331 PUCCH-ConfigCommon IE.
enum class pucch_group_hopping {
  /// Implies neither group or sequence hopping is enabled.
  NEITHER,
  /// Enables group hopping and disables sequence hopping.
  ENABLE,
  /// Disables group hopping and enables sequence hopping.
  DISABLE
};

/// \brief PUCCH Formats as described in TS 38.213 clause 9.2.
enum class pucch_format { FORMAT_0, FORMAT_1, FORMAT_2, FORMAT_3, FORMAT_4, NOF_FORMATS };

/// Maximum number of symbols (without DMRS) that NR-PUCCH format 1 can transmit.
static constexpr unsigned PUCCH_FORMAT1_N_MAX = 7;

} // namespace srsgnb

#endif // SRSGNB_RAN_PUCCH_MAPPING_H
