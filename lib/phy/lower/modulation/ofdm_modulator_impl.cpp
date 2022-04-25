
#include "ofdm_modulator_impl.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include <complex.h>

using namespace srsgnb;

ofdm_symbol_modulator_impl::ofdm_symbol_modulator_impl(const ofdm_modulator_factory_config& factory_config,
                                                       const ofdm_modulator_configuration&  ofdm_config) :
  dft_size(ofdm_config.dft_size),
  rg_size(ofdm_config.bw_rb * NRE),
  cp(ofdm_config.cp),
  numerology(ofdm_config.numerology),
  scale(ofdm_config.scale),
  center_freq_hz(ofdm_config.center_freq_hz),
  dft(factory_config.dft_factory.create({dft_size, dft_processor::direction::INVERSE})),
  temp_buffer(dft_size)
{
  srsran_always_assert(std::isnormal(scale), "Invalid scaling factor %f", scale);
  srsran_always_assert(
      dft_size > rg_size, "The DFT size (%d) must be greater than the resource grid size (%d)", dft_size, rg_size);

  // Fill DFT input with zeros.
  srsvec::zero(dft->get_input());
}

unsigned ofdm_symbol_modulator_impl::get_symbol_offset(unsigned symbol_index) const
{
  // Calculate the offset in samples to the start of the symbol including the CPs
  unsigned phase_freq_offset = 0;
  for (unsigned symb_idx = 0; symb_idx != symbol_index; ++symb_idx) {
    phase_freq_offset += cp.get_length(symb_idx, numerology, dft_size) + dft_size;
  }
  phase_freq_offset += cp.get_length(symbol_index, numerology, dft_size);

  return phase_freq_offset;
}

cf_t ofdm_symbol_modulator_impl::get_phase_compensation(unsigned symbol_index) const
{
  // Calculate the phase compensation (TS 138.211, Section 5.4)
  unsigned nsymb         = get_nsymb_per_slot(cp);
  unsigned symbol_offset = get_symbol_offset(symbol_index % nsymb);
  double   scs           = scs_to_khz(subcarrier_spacing(numerology)) * 1e3;
  double   srate_hz      = scs * dft_size;
  double   phase_rad     = -2.0 * M_PI * center_freq_hz * (symbol_offset / srate_hz);

  // Calculate compensation phase in double precision and then convert to single
  return (cf_t)std::conj(std::exp(std::complex<double>(I * phase_rad)));
}

void ofdm_symbol_modulator_impl::modulate(srsgnb::span<srsgnb::cf_t>          output,
                                          const srsgnb::resource_grid_reader& grid,
                                          unsigned                            port_index,
                                          unsigned                            symbol_index)
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate cyclic prefix length.
  unsigned cp_len = cp.get_length(symbol_index, numerology, dft_size);

  // Make sure output buffer matches the symbol size.
  srsran_always_assert(output.size() == (cp_len + dft_size),
                       "The output buffer size ({}) does not match the symbol index {} size ({}+{}={}). Numerology={}.",
                       output.size(),
                       symbol_index,
                       cp_len,
                       dft_size,
                       cp_len + dft_size,
                       numerology);

  // Prepare lower bound frequency domain data.
  grid.get(dft->get_input().last(rg_size / 2), port_index, symbol_index % nsymb, 0);

  // Prepare upper bound frequency domain data.
  grid.get(dft->get_input().first(rg_size / 2), port_index, symbol_index % nsymb, rg_size / 2);

  // Execute DFT.
  span<const cf_t> dft_output = dft->run();

  // Get phase correction (TS 138.211, Section 5.4)
  cf_t phase_compensation = get_phase_compensation(symbol_index);

  // Apply scaling and phase compensation.
  srsvec::sc_prod(dft_output, phase_compensation * scale, output.last(dft_size));

  // Copy cyclic prefix.
  srsvec::copy(output.first(cp_len), output.last(cp_len));
}

unsigned ofdm_slot_modulator_impl::get_slot_size(unsigned slot_index) const
{
  unsigned nsymb = get_nsymb_per_slot(cp);
  unsigned count = 0;

  // Iterate all symbols of the slot and accumulate
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    count += symbol_modulator.get_symbol_size(nsymb * slot_index + symbol_idx);
  }

  return count;
}

void ofdm_slot_modulator_impl::modulate(span<cf_t>                  output,
                                        const resource_grid_reader& grid,
                                        unsigned                    port_index,
                                        unsigned                    slot_index)
{
  unsigned nsymb = get_nsymb_per_slot(cp);

  // For each symbol in the slot.
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    // Get the current symbol size.
    unsigned symbol_sz = symbol_modulator.get_symbol_size(nsymb * slot_index + symbol_idx);

    // Modulate symbol.
    symbol_modulator.modulate(output.first(symbol_sz), grid, port_index, nsymb * slot_index + symbol_idx);

    // Advance output buffer.
    output = output.last(output.size() - symbol_sz);
  }
}

std::unique_ptr<ofdm_modulator_factory> srsgnb::create_ofdm_modulator_factory(ofdm_modulator_factory_config& config)
{
  return std::make_unique<ofdm_modulator_factory_impl>(config);
}
