#ifndef SRSGNB_FAPI_MESSAGES_BUILDER_H
#define SRSGNB_FAPI_MESSAGES_BUILDER_H

#include "srsgnb/fapi/messages.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {
namespace fapi {

/// DL TTI request message builder.
class dl_tti_request_message_builder
{
  /// Maximum number of DL PDU types supported (5 for release v222.10.04).
  static constexpr unsigned NUM_DL_TYPES = 5;

public:
  explicit dl_tti_request_message_builder(dl_tti_request_message& msg) : msg(msg) { msg.num_dl_types = NUM_DL_TYPES; }

  /// Sets the message's timing and return a reference to the builder.
  dl_tti_request_message_builder& set_timing(const slot_point& point)
  {
    msg.sfn  = point.sfn();
    msg.slot = point.slot_index();

    srsran_assert(msg.sfn < 1024, "SFN out of range");
    srsran_assert(msg.slot < 160, "Slot index out of range");

    return *this;
  }

  /// Sets the message's number of groups and return a reference to the builder.
  dl_tti_request_message_builder& set_num_groups(uint16_t value)
  {
    srsran_assert(value < 3823, "Number of PDU Groups out of range");

    msg.num_groups = value;
    return *this;
  }

  /// Adds a PDCCH pdu to the message and returns a PDCCH pdu builder.
  dl_pdcch_pdu_builder add_pdcch_pdu()
  {
    ++msg.num_pdus_of_each_type[static_cast<int>(dl_pdu_type::PDCCH)];
    // :TODO: Need to fill the number of DIDCIs across all PDCCH PDU in the message.

    dl_tti_request_pdu& pdu = msg.pdus[msg.num_pdus];
    pdu.pdu_type            = dl_pdu_type::PDCCH;
    // :TODO: fill this one.
    pdu.pdu_size;

    dl_pdcch_pdu_builder builder(pdu.pdcch_pdu);
    ++msg.num_pdus;

    return builder;
  }

  /// Adds a PDSCH pdu to the message and returns a PDSCH pdu builder.
  dl_pdsch_pdu_builder add_pdsch_pdu()
  {
    ++msg.num_pdus_of_each_type[static_cast<int>(dl_pdu_type::PDSCH)];

    dl_tti_request_pdu& pdu = msg.pdus[msg.num_pdus];
    pdu.pdu_type            = dl_pdu_type::PDSCH;
    // :TODO: fill this one.
    pdu.pdu_size;

    dl_pdsch_pdu_builder builder(pdu.pdsch_pdu);
    ++msg.num_pdus;

    return builder;
  }

  /// Adds a CSI-RS pdu to the message and returns a CSI-RS pdu builder.
  dl_csi_rs_pdu_builder add_csi_rs_pdu()
  {
    ++msg.num_pdus_of_each_type[static_cast<int>(dl_pdu_type::CSI_RS)];

    dl_tti_request_pdu& pdu = msg.pdus[msg.num_pdus];
    pdu.pdu_type            = dl_pdu_type::CSI_RS;
    // :TODO: fill this one.
    pdu.pdu_size;

    dl_csi_rs_pdu_builder builder(pdu.csi_rs_pdu);
    ++msg.num_pdus;

    return builder;
  }

  /// Adds a SSB pdu to the message and returns a SSB pdu builder.
  dl_ssb_pdu_builder add_ssb_pdu()
  {
    dl_tti_request_pdu& pdu = msg.pdus[msg.num_pdus];

    // Fill the ssb pdu index value. The ide will be the index of the pdu in the array of SSB pdus.
    dl_ssb_maintenance_v3& info        = pdu.ssb_pdu.ssb_maintenance_v3;
    auto&                  num_ssb_pdu = msg.num_pdus_of_each_type[static_cast<int>(dl_pdu_type::SSB)];
    info.ssb_pdu_index                 = num_ssb_pdu;

    // Increase the number of SSB pdus in the request.
    ++num_ssb_pdu;

    pdu.pdu_type = dl_pdu_type::SSB;
    // :TODO: fill this one.
    pdu.pdu_size;

    dl_ssb_pdu_builder builder(pdu.ssb_pdu);
    ++msg.num_pdus;

    return builder;
  }

  /// Helper function that will calculate each pdu size and update pdu_size value.
  void update_pdu_size()
  {
    // :TODO: Implement me!
  }

  //: TODO: groups array
  //: TODO: top level rate match patterns

private:
  dl_tti_request_message& msg;
};

/// DL SSB pdu builder.
class dl_ssb_pdu_builder
{
public:
  explicit dl_ssb_pdu_builder(dl_ssb_pdu& pdu) : pdu(pdu) {}

  /// Sets the physical cell id to the given value and returns a reference to the builder.
  dl_ssb_pdu_builder& set_phys_cell_id(uint16_t value)
  {
    srsran_assert(value < 1008, "SSB physical cell ID out of range");

    pdu.phys_cell_id = value;
    return *this;
  }

  /// Sets the beta pss profile nr to the given value and returns a reference to the builder.
  dl_ssb_pdu_builder& set_beta_pss_profile_nr(beta_pss_profile_type value)
  {
    pdu.beta_pss_profile_nr = value;
    return *this;
  }

  /// Sets the SSB parameters of the pdu with the given values and returns a reference to the builder.
  dl_ssb_pdu_builder& set_ssb_info(uint ssb_block_index, uint8_t ssb_subcarrier_offset, uint16_t ssb_offset_pointA)
  {
    srsran_assert(ssb_block_index < 64, "SSB block index out of range");
    pdu.ssb_block_index = ssb_block_index;

    srsran_assert(ssb_subcarrier_offset < 32, "SSB subcarrier offset out of range");
    pdu.ssb_subcarrier_offset = ssb_subcarrier_offset;

    srsran_assert(ssb_offset_pointA < 2200, "Offset of lowest subcarrier of lowest resource block out of range");
    pdu.ssb_offset_pointA = ssb_offset_pointA;

    return *this;
  }

  /// Adds the bch payload configured by the MAC and returns a reference to the builder.
  // :TODO: when the MAC data structure is defined, update the function arguments and implement the function.
  dl_ssb_pdu_builder& add_bch_payload_mac_full(unsigned sfn,
                                               unsigned half_radio_frame_bit,
                                               unsigned ss_pbch_block_idx,
                                               unsigned bch_payload)
  {
    pdu.bch_payload_flag = bch_payload_type::mac_full;
    // :TODO: implement me!

    return *this;
  }

  /// Adds the bch payload and returns a reference to the builder. PHY configures the timing PBCH bits.
  // :TODO: when the MAC data structure is defined, update the function arguments and implement the function.
  dl_ssb_pdu_builder& add_bch_payload_mixed(unsigned bch_payload)
  {
    pdu.bch_payload_flag = bch_payload_type::phy_timing;
    // :TODO: implement me!

    return *this;
  }

  /// Adds the bch payload configured by the PHY and returns a reference to the builder.
  dl_ssb_pdu_builder& add_bch_payload_phy_full(uint8_t dmrs_type_a_position,
                                               uint8_t pdcch_config_sib1,
                                               uint8_t cell_barred,
                                               uint8_t intra_freq_reselection)
  {
    srsran_assert(dmrs_type_a_position < 2, "DMRS type A position value out of range");
    srsran_assert(cell_barred < 2, "Cell barred value out of range");
    srsran_assert(intra_freq_reselection < 2, "Intra-frequency reselection value out of range");

    pdu.bch_payload_flag                              = bch_payload_type::phy_full;
    pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position   = dmrs_type_a_position;
    pdu.bch_payload.phy_mib_pdu.pdcch_config_sib1     = pdcch_config_sib1;
    pdu.bch_payload.phy_mib_pdu.cell_barred           = cell_barred;
    pdu.bch_payload.phy_mib_pdu.intrafreq_reselection = intra_freq_reselection;
    return *this;
  }

  /// Sets the case type to the given value and returns a reference to the builder.
  dl_ssb_pdu_builder& set_case_type(ssb_case_type value)
  {
    dl_ssb_maintenance_v3& info = pdu.ssb_maintenance_v3;
    info.case_type              = value;
    return *this;
  }

  /// Sets LMax to the given value and returns a reference to the builder.
  dl_ssb_pdu_builder& set_lmax(uint8_t value)
  {
    // :TODO: max being the maximum number of SS/PBCH beams in an SS/PBCH period for a particular band as given by
    // [38.104]. :TODO: lmax = 4, 8 or 64.
    dl_ssb_maintenance_v3& info = pdu.ssb_maintenance_v3;
    info.lmax                   = value;
    return *this;
  }

  /// Sets the SSB power inforation and returns a reference to the builder.
  dl_ssb_pdu_builder& set_power_information(float power_scaling_ss_pbch_dB, float pss_to_sss_ratio_dB)
  {
    // Power scaling in SS-PBCH in hundredth of dB.
    int power_scaling_ss_pbch = power_scaling_ss_pbch_dB * 100;
    srsran_assert(power_scaling_ss_pbch <= 12000, "SS-PBCH power scaling value out of range");
    // :TODO: check this.
    srsran_assert(power_scaling_ss_pbch == std::numeric_limits<int16_t>::min() || power_scaling_ss_pbch >= -11000,
                  "SS-PBCH power scaling value out of range");

    dl_ssb_maintenance_v3& info      = pdu.ssb_maintenance_v3;
    info.ss_pbch_block_power_scaling = power_scaling_ss_pbch;

    // SSS to PSS ratio in thousandth of dB.
    int pss_to_sss_ratio      = pss_to_sss_ratio_dB * 1000;
    info.beta_pss_profile_sss = pss_to_sss_ratio;
    return *this;
  }

  /// Sets the subcarrier spacing to the given value and returns a reference to the builder.
  dl_ssb_pdu_builder& set_subcarrier_spacing(uint8_t value)
  {
    srsran_assert(value < 5, "SSB subcarrier spacing out of range");
    dl_ssb_maintenance_v3& info = pdu.ssb_maintenance_v3;
    info.subcarrier_spacing     = value;
    return *this;
  }

  //: TODO: params v4 - MU-MIMO.
  // :TODO: beamforming.
private:
  dl_ssb_pdu& pdu;
};

/// DL PDCCH pdu builder. Helps with the pdu build.
class dl_pdcch_pdu_builder
{
public:
  explicit dl_pdcch_pdu_builder(dl_pdcch_pdu& pdu) : pdu(pdu) {}

private:
  dl_pdcch_pdu& pdu;
};

/// DL PDSCH pdu builder. Helps with the pdu build.
class dl_pdsch_pdu_builder
{
public:
  explicit dl_pdsch_pdu_builder(dl_pdsch_pdu& pdu) : pdu(pdu) {}

private:
  dl_pdsch_pdu& pdu;
};

/// DL CSI RS pdu builder. Helps with the pdu build.
class dl_csi_rs_pdu_builder
{
public:
  explicit dl_csi_rs_pdu_builder(dl_csi_rs_pdu& pdu) : pdu(pdu) {}

private:
  dl_csi_rs_pdu& pdu;
}

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_MESSAGES_BUILDER_H
