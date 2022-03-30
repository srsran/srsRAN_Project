#include "phy_helpers.h"

#define SRSRAN_NSYMB_PER_SLOT_NR 14U

using namespace srsgnb;

/// \brief Convert SLIV to symbol start S and length L.
/// \param[in] N Number of symbols per slot.
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] S Start symbols.
/// \param[out] L Length.
// TODO: (PHY) Review and port this to PHY or replace with equivalent PHY helper
static void sliv_to_s_and_l(uint32_t N, uint32_t sliv, uint32_t& S, uint32_t& L)
{
  uint32_t low  = sliv % N;
  uint32_t high = sliv / N;
  if (high + 1 + low <= N) {
    S = low;
    L = high + 1;
  } else {
    S = N - 1 - low;
    L = N - high + 1;
  }
}

/// \brief Overload function that converts SLIV to symbol start S and length L.
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] symbols Symbol interval as [S, S+L).
static symbol_interval sliv_to_s_and_l(uint32_t sliv)
{
  uint32_t symbol_S, symbol_L;
  sliv_to_s_and_l(SRSRAN_NSYMB_PER_SLOT_NR, sliv, symbol_S, symbol_L);
  symbol_interval symbols;

  symbols.set(static_cast<uint8_t>(symbol_S), static_cast<uint8_t>(symbol_S + symbol_L));
  return symbols;
}

/// \brief Get MSG3 Delay.
///
/// \remark This function adds.
/// \param[in] ul_cfg_common ASN1 BWP-UplinkCommon struct.
/// \param[in] sl_dci An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] pusch_delay MSG3 delay.
/// \param[out] symbols PUSCH OFDM symbol interval where MSG3 is allocated.
void srsgnb::get_msg3_delay(const asn1::rrc_nr::ul_cfg_common_sib_s& ul_cfg_common,
                            const slot_point&                        sl_dci,
                            unsigned&                                pusch_delay,
                            symbol_interval&                         symbols)
{
  // Perform sanity checks.
  srsran_assert(ul_cfg_common.init_ul_bwp.pusch_cfg_common_present,
                "BWP-UplinkCommon has no pusch-ConfigCommon configured");
  srsran_assert(ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list.size() > 0,
                "PUSCH-ConfigCommon has no PUSCH-TimeDomainResourceAllocationList configured");
  srsran_assert(ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2_present,
                "K2 not present in PUSCH-TimeDomainAllocationList[0]");

  uint8_t k2        = ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2;
  auto&   pusch_scs = ul_cfg_common.init_ul_bwp.generic_params.subcarrier_spacing;

  // In TS 38.214, Table 6.1.2.1.1-5, Delta is only defined for PUSCH SCS within [kHz15, kHz120kHz].
  srsran_assert(pusch_scs <= asn1::rrc_nr::subcarrier_spacing_opts::options::khz120,
                "PUSCH subcarrier spacing not supported for MSG3 delay");

  // The array represents Table 6.1.2.1.1-5, in TS 38.214.
  // TODO: Check if this is implemented in the PHY.
  static const std::array<uint8_t, 4> DELTAS{2, 3, 4, 6};

  // The MSG3 slot is defined as MSG3_slot = floor( n * (2^*(mu_PUSCH) ) / (2^*(mu_PDCCH) ) ) + k2 + Delta.
  // Given the assumption mu_PUSCH == mu_PDCCH, MSG3_delay simplifies to MSG3_delay =  k2 + Delta
  // [TS 38.214, Section 6.1.2.1 and 6.1.2.1.1].
  pusch_delay = static_cast<int>(k2 + DELTAS[pusch_scs]);

  uint32_t sliv =
      ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].start_symbol_and_len;

  symbol_interval symbs = sliv_to_s_and_l(sliv);
  symbols.set(symbs.start(), symbs.stop());
}