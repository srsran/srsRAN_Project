
#include "mac_cell_asn1_helpers.h"
#include "srsgnb/asn1/rrc_nr/serving_cell.h"

using namespace srsgnb;

namespace {

void fill_tdd_cfg_from_asn1(const asn1::rrc_nr::tdd_ul_dl_cfg_common_s& in, tdd_configuration& out)
{
  // TODO
}

} // namespace

void srsgnb::fill_mac_cell_cfg_from_asn1(const asn1::rrc_nr::serving_cell_cfg_common_sib_s& in,
                                         mac_cell_configuration&                            out)
{
  // Carrier Configuration.
  // TODO

  // TDD Configuration.
  if (in.tdd_ul_dl_cfg_common_present) {
    out.tdd_cfg.emplace();
    fill_tdd_cfg_from_asn1(in.tdd_ul_dl_cfg_common, out.tdd_cfg.value());
  }
}
