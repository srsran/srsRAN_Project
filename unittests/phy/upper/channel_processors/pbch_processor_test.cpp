
#include "../../resource_grid_test_doubles.h"
#include "../signal_processors/dmrs_pbch_processor_doubles.h"
#include "../signal_processors/pss_processor_doubles.h"
#include "../signal_processors/sss_processor_doubles.h"
#include "pbch_encoder_doubles.h"
#include "pbch_modulator_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pbch_processor.h"
#include <cassert>
#include <random>

using namespace srsgnb;

namespace srsgnb {
struct pbch_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

std::unique_ptr<pbch_processor> create_pbch_processor(pbch_processor_config& config);
} // namespace srsgnb

static std::mt19937 rgen(0);

int main()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> pci_dist(0, phys_cell_id::NOF_NID - 1);
  std::uniform_int_distribution<unsigned> bit_dist(0, 1);

  resource_grid_dummy grid;

  pbch_encoder_spy*        encoder   = new pbch_encoder_spy;
  pbch_modulator_spy*      modulator = new pbch_modulator_spy;
  dmrs_pbch_processor_spy* dmrs      = new dmrs_pbch_processor_spy;
  pss_processor_spy*       pss       = new pss_processor_spy;
  sss_processor_spy*       sss       = new sss_processor_spy;

  pbch_processor_config config = {};
  config.encoder               = std::unique_ptr<pbch_encoder>(encoder);
  config.modulator             = std::unique_ptr<pbch_modulator>(modulator);
  config.dmrs                  = std::unique_ptr<dmrs_pbch_processor>(dmrs);
  config.pss                   = std::unique_ptr<pss_processor>(pss);
  config.sss                   = std::unique_ptr<sss_processor>(sss);

  std::unique_ptr<pbch_processor> pbch = create_pbch_processor(config);

  // Iterate all possible SSB pattern cases
  for (const ssb_pattern_case& pattern_case :
       {ssb_pattern_case::A, ssb_pattern_case::B, ssb_pattern_case::C, ssb_pattern_case::D, ssb_pattern_case::E}) {
    // Iterate half frames
    for (unsigned subframe : {0, 5}) {
      // Iterate possible L_max
      for (unsigned L_max : {4, 8, 64}) {
        // Iterate possible beta PSS
        for (float beta_pss : {0.0F, -3.0F}) {
          // Select numerology from case
          unsigned numerology = 0; // 15 kHz
          switch (pattern_case) {
            case ssb_pattern_case::A:
              // 15 kHz
              break;
            case ssb_pattern_case::B:
            case ssb_pattern_case::C:
              numerology = 1; // 30 kHz
              break;
            case ssb_pattern_case::D:
              numerology = 3; // 120 kHz
              break;
            case ssb_pattern_case::E:
              numerology = 4; // 240 kHz
              break;
          }

          // Skip invalid pattern and L_max combinations
          if (pattern_case != ssb_pattern_case::E && pattern_case != ssb_pattern_case::D && L_max == 64) {
            continue;
          }

          // Generate PBCH PDU
          pbch_processor::pdu_t pdu = {};
          pdu.slot.numerology       = numerology;
          pdu.slot.frame            = sfn_dist(rgen);
          pdu.slot.subframe         = subframe;
          pdu.slot.slot             = 0;
          pdu.phys_cell_id          = pci_dist(rgen);
          pdu.beta_pss              = beta_pss;
          pdu.ssb_idx               = 0; //< Important to match slot index
          pdu.L_max                 = L_max;
          pdu.ssb_subcarrier_offset = 0;
          pdu.ssb_offset_pointA     = 0;
          pdu.pattern_case          = pattern_case;
          for (uint8_t& b : pdu.bch_payload) {
            b = bit_dist(rgen);
          }

          // Reset spies
          encoder->reset();
          modulator->reset();

          // Process PDU
          pbch->proccess(pdu, grid);

          // Assert encoder
          assert(encoder->get_nof_entries() == 1);
          assert(encoder->get_entries()[0].N_id == pdu.phys_cell_id);
          assert(encoder->get_entries()[0].ssb_idx == pdu.ssb_idx);
          assert(encoder->get_entries()[0].L_max == L_max);
          assert(encoder->get_entries()[0].hrf == pdu.slot.get_half_radio_frame());

          // Assert modulator
          assert(modulator->get_nof_entries() == 1);
        }
      }
    }
  }

  return 0;
}
