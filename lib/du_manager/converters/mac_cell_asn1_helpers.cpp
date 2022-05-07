
#include "mac_cell_asn1_helpers.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/asn1/rrc_nr/serving_cell.h"
#include "ssb_configuration_asn1_helpers.h"

using namespace srsgnb;

namespace {

void fill_tdd_cfg_from_asn1(const asn1::rrc_nr::tdd_ul_dl_cfg_common_s& in, tdd_configuration& out)
{
  // TODO
}

} // namespace

void srsgnb::fill_mac_cell_cfg_from_asn1(mac_cell_configuration&                            out,
                                         const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_sib_in,
                                         const asn1::rrc_nr::mib_s&                         mib_in,
                                         subcarrier_spacing                                 ssb_scs)
{
  // Carrier Configuration.
  // TODO

  // TDD Configuration.
  if (serv_cell_sib_in.tdd_ul_dl_cfg_common_present) {
    out.tdd_cfg.emplace();
    fill_tdd_cfg_from_asn1(serv_cell_sib_in.tdd_ul_dl_cfg_common, out.tdd_cfg.value());
  }

  fill_ssb_configuration_from_asn1(out.ssb_cfg, mib_in, serv_cell_sib_in, ssb_scs);
}
