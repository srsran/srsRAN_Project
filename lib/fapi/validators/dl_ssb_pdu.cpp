#include "dl_ssb_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/fapi/validator_report.h"

using namespace srsgnb;
using namespace fapi;

/// This validator checks the SSB PDU.
static constexpr dl_pdu_type pdu_type = dl_pdu_type::SSB;

/// Validates the PCI property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_phy_cell_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1007;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Physical cell ID", pdu_type, report);
}

/// Validates the block index property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_block_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "SS/PBCH block index", pdu_type, report);
}

/// Validates the subcarrier offset property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_subcarrier_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 31;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier offset", pdu_type, report);
}

/// Validates the offset point A property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_offset_point_a(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2199;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Offset point A", pdu_type, report);
}

/// Validates the DMRS type A position property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4 in table PHY
/// MIB.
static bool validate_dmrs_type_a_position(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Dmrs type A position", pdu_type, report);
}

/// Validates the cell barred property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4 in table PHY
/// MIB.
static bool validate_cell_barred(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cell barred", pdu_type, report);
}

/// Validates the intra freq reselection property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4 in table
/// PHY MIB.
static bool validate_intra_freq_reselection(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Intra frequency reselection", pdu_type, report);
}

/// Validates the baseband power scaling for SS-PBCH property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4
/// in table maintenance v3.
static bool validate_ss_pbch_power_scaling(int value, validator_report& report)
{
  static constexpr int L1_DEFINES_PSS_POWER = -32768;
  static constexpr int MIN_VALUE            = -11000;
  static constexpr int MAX_VALUE            = 12000;

  if (value == L1_DEFINES_PSS_POWER) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Baseband power scaling applied to SS-PBCH", pdu_type, report);
}

/// Validates the beta PSS profile SSS property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4
/// in table maintenance v3.
static bool validate_beta_pss_profile_sss(const dl_ssb_pdu& pdu, validator_report& report)
{
  static constexpr int BETA_PSS_PROFILE_NR_DEFINES_PSS_POWER = -32768;
  static constexpr int MIN_VALUE                             = -32767;
  static constexpr int MAX_VALUE                             = 32767;

  int power_value = pdu.ssb_maintenance_v3.beta_pss_profile_sss;
  if (pdu.beta_pss_profile_nr == beta_pss_profile_type::beta_pss_profile_sss && MIN_VALUE <= power_value &&
      power_value <= MAX_VALUE) {
    return true;
  }

  if (pdu.beta_pss_profile_nr != beta_pss_profile_type::beta_pss_profile_sss &&
      power_value == BETA_PSS_PROFILE_NR_DEFINES_PSS_POWER) {
    return true;
  }

  report.append(power_value, "Beta PSS profile SSS", pdu_type);
  return false;
}

/// Validates the LMax property of the DL_TTI.request, as per SCF-222 v4.0 section 3.4.2.4 in table maintenance v3.
static bool validate_lmax(unsigned value, validator_report& report)
{
  if (value == 4 || value == 8 || value == 64) {
    return true;
  }

  report.append(value, "LMax", pdu_type);
  return false;
}

bool srsgnb::fapi::validate_dl_ssb_pdu(const dl_ssb_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_phy_cell_id(pdu.phys_cell_id, report);
  result &= validate_block_index(pdu.ssb_block_index, report);
  result &= validate_subcarrier_offset(pdu.ssb_subcarrier_offset, report);
  result &= validate_offset_point_a(pdu.ssb_offset_pointA, report);
  result &= validate_dmrs_type_a_position(pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position, report);
  result &= validate_cell_barred(pdu.bch_payload.phy_mib_pdu.cell_barred, report);
  result &= validate_intra_freq_reselection(pdu.bch_payload.phy_mib_pdu.intrafreq_reselection, report);
  result &= validate_lmax(pdu.ssb_maintenance_v3.lmax, report);
  result &= validate_ss_pbch_power_scaling(pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling, report);
  result &= validate_beta_pss_profile_sss(pdu, report);

  return result;
}
