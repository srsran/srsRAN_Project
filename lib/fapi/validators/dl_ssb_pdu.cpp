#include "dl_ssb_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/validator_report.h"

using namespace srsgnb;
using namespace fapi;

/// Validates the pci property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_phy_cell_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1007;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Physical cell ID", report);
}

/// Validates the block index property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_block_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "SS/PBCH block index", report);
}

/// Validates the subcarrier offset property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_subcarrier_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 31;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier offset", report);
}

/// Validates the offset point A property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_offset_point_a(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2199;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Offset point A", report);
}

/// Validates the DMRS type A position property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4 in table PHY
/// MIB.
static bool validate_dmrs_type_a_position(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Dmrs type A position", report);
}

/// Validates the cell barred property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4 in table PHY
/// MIB.
static bool validate_cell_barred(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cell barred", report);
}

/// Validates the intra freq reselection property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4 in table
/// PHY MIB.
static bool validate_intra_freq_reselection(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Intra frequency reselection", report);
}

/// Validates the baseband power scaling for SS-PBCH property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4
/// in table maintenance v3.
static bool validate_ss_pbch_power_scaling(int value, validator_report& report)
{
  static constexpr int L1_DEFINES_PSS_POWER = -32768;
  static constexpr int MIN_VALUE            = -11000;
  static constexpr int MAX_VALUE            = 12000;

  if (value == L1_DEFINES_PSS_POWER) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Baseband power scaling applied to SS-PBCH", report);
}

/// Validates the LMax property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.4 in table
/// maintenance v3.
static bool validate_lmax(unsigned value, validator_report& report)
{
  if (value == 4 || value == 8 || value == 64) {
    return true;
  }

  report.emplace_back(value, "LMax");
  return false;
}

bool srsgnb::fapi::validate_dl_ssb_pdu(const dl_ssb_pdu& pdu, validator_report& report)
{
  // NOTE: bitwise operator to make sure that every function is called, so every field of the SSB pdu is checked.
  return validate_phy_cell_id(pdu.phys_cell_id, report) & validate_block_index(pdu.ssb_block_index, report) &
         validate_subcarrier_offset(pdu.ssb_subcarrier_offset, report) &
         validate_offset_point_a(pdu.ssb_offset_pointA, report) &
         validate_dmrs_type_a_position(pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position, report) &
         validate_cell_barred(pdu.bch_payload.phy_mib_pdu.cell_barred, report) &
         validate_intra_freq_reselection(pdu.bch_payload.phy_mib_pdu.intrafreq_reselection, report) &
         validate_lmax(pdu.ssb_maintenance_v3.lmax, report) &
         validate_ss_pbch_power_scaling(pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling, report);
}
