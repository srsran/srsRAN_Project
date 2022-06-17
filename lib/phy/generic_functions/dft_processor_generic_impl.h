
#ifndef LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_GENERIC_IMPL_H
#define LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_GENERIC_IMPL_H

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

/// Generic interface of FFT size N
class generic_fft_N
{
public:
  virtual ~generic_fft_N()                          = default;
  virtual void run(cf_t* out, const cf_t* in) const = 0;
};

/// Describes a DFT processor class configuration based on the FFTW library.
class dft_processor_generic_impl : public dft_processor
{
private:
  /// Stores the DFT direction.
  direction dir;
  /// DFT input buffer ownership.
  srsvec::aligned_vec<cf_t> input;
  /// DFT output buffer ownership.
  srsvec::aligned_vec<cf_t> output;
  /// Generic FFT.
  std::unique_ptr<generic_fft_N> generic_fft;

public:
  /// \brief Constructs a generic DFT processor.
  /// \param [in] dft_config Provides the generic DFT processor parameters.
  dft_processor_generic_impl(const configuration& dft_config);

  /// Determines whether the initialization was successful.
  bool is_valid() const { return generic_fft != nullptr; }

  // See interface for documentation.
  direction get_direction() const override { return dir; }

  // See interface for documentation.
  unsigned int get_size() const override { return input.size(); }

  // See interface for documentation.
  span<cf_t> get_input() override { return input; }

  // See interface for documentation.
  span<const cf_t> run() override;
};

} // namespace srsgnb

#endif // LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_GENERIC_IMPL_H
