/// \file
/// \brief LDPC rate matching and recovering unit test.

#include "ldpc_rm_test_sets.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_rate_matching.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

int run_rv_mod_lbrm_test(unsigned                                           rv,
                         modulation_scheme                                  mod,
                         bool                                               lbrm,
                         const std::array<std::vector<uint8_t>, nof_rates>& matched);

static std::unique_ptr<ldpc_rate_matching> rm = create_ldpc_rate_matching();

int main()
{
  // BPSK
  assert(run_rv_mod_lbrm_test(0, modulation_scheme::BPSK, false, matched_rv0_bpsk) == 0);
  assert(run_rv_mod_lbrm_test(1, modulation_scheme::BPSK, false, matched_rv1_bpsk) == 0);
  assert(run_rv_mod_lbrm_test(2, modulation_scheme::BPSK, false, matched_rv2_bpsk) == 0);
  assert(run_rv_mod_lbrm_test(3, modulation_scheme::BPSK, false, matched_rv3_bpsk) == 0);
  assert(run_rv_mod_lbrm_test(0, modulation_scheme::BPSK, true, matched_rv0_bpsk_lbrm) == 0);
  assert(run_rv_mod_lbrm_test(1, modulation_scheme::BPSK, true, matched_rv1_bpsk_lbrm) == 0);
  assert(run_rv_mod_lbrm_test(2, modulation_scheme::BPSK, true, matched_rv2_bpsk_lbrm) == 0);
  assert(run_rv_mod_lbrm_test(3, modulation_scheme::BPSK, true, matched_rv3_bpsk_lbrm) == 0);

  // QPSK
  assert(run_rv_mod_lbrm_test(0, modulation_scheme::QPSK, false, matched_rv0_qpsk) == 0);
}

int run_rv_mod_lbrm_test(unsigned                                           rv,
                         modulation_scheme                                  mod,
                         bool                                               lbrm,
                         const std::array<std::vector<uint8_t>, nof_rates>& matched_bm)
{
  for (int i = 0; i != nof_rates; ++i) {
    unsigned mod_order = static_cast<unsigned>(mod);
    unsigned rm_length = static_cast<unsigned>(floor(message_length / rate_list[i] / mod_order)) * mod_order;
    std::vector<uint8_t>         matched(rm_length);
    unsigned                     n_ref = lbrm ? test_Nref : 0;
    ldpc_rate_matching::config_t rm_cfg{rv, mod, n_ref};
    rm->rate_match(matched, codeblock, rm_cfg);

    assert(matched_bm[i].size() == rm_length);
    if (!std::equal(matched.cbegin(), matched.cend(), matched_bm[i].cbegin())) {
      return 1;
    }
  }
  return 0;
}
