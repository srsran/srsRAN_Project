
#pragma once

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <functional>

namespace srsgnb {

/// Describes a DFT processor class configuration based on the FFTW library.
class dft_processor_fftx_impl : public dft_processor
{
private:
  /// Stores the DFT direction.
  direction dir;
  /// DFT input buffer ownership.
  srsvec::aligned_vec<cf_t> input;
  /// DFT output buffer ownership.
  srsvec::aligned_vec<cf_t> output;
  /// FFTX function wrapper.
  std::function<void(float*, float*)> function;

public:
  /// \brief Constructs a fftx DFT processor.
  /// \param [in] dft_config Provides the fftx DFT processor parameters.
  dft_processor_fftx_impl(const configuration& dft_config);

  /// Determines whether the instance has been initialised successfully.
  bool is_valid() const { return function != nullptr; }

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
