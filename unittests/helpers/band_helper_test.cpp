#include "../../lib/helpers/band_helper.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_band_helper_basic()
{
  band_helper helper;

  TESTASSERT(helper.nr_arfcn_to_freq(530000) > 0);
  TESTASSERT(band_helper::get_band_from_dl_arfcn(530000) < UINT16_MAX);
  TESTASSERT(band_helper::get_ssb_pattern(1, subcarrier_spacing::kHz15) < ssb_pattern_case::invalid);
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);

  srslog::init();

  test_band_helper_basic();
}
