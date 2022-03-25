
#ifndef SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H
#define SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// \brief Describes a Discrete Fourier Transform (DFT) processor.
///
/// Describes a Discrete Fourier Transform (DFT) processor that owns the input and output buffers.
class dft_processor
{
public:
  /// Indicates the DFT direction.
  enum class direction {
    /// Converts a time domain signal into frequency domain.
    FORWARD = 0,
    /// Converts a frequency domain signal into frequency (inverse DFT).
    BACKWARD
  };

  /// Default destructor.
  virtual ~dft_processor() = default;

  /// \brief Get the DFT direction.
  /// \return The DFT direction.
  virtual direction get_direction() const = 0;

  /// \brief Get the DFT number of points.
  virtual unsigned get_size() const = 0;

  /// \brief Get in DFT input buffer.
  /// \return A view of the internal input DFT buffer.
  virtual span<cf_t> get_input() const = 0;

  /// \brief Execute the DFT from the internal input data.
  /// \return A view of the internal output DFT buffer.
  virtual span<const cf_t> run() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H
