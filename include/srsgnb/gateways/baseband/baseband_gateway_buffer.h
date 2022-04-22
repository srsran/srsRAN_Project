
#ifndef SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BUFFER_H
#define SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BUFFER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/gateways/baseband/baseband_gateway_base.h"
#include "srsgnb/support/srsran_assert.h"
#include <complex>
#include <vector>

namespace srsgnb {

/// Defines the radio sample type.
using radio_sample_type = std::complex<float>;

/// Describes a baseband buffer interface. It comprises get_nof_channels() channels of get_nof_samples() samples each.
class baseband_gateway_buffer : public baseband_gateway_base
{
public:
  /// Get the number of channels.
  virtual unsigned get_nof_channels() const = 0;

  /// Get the number of samples.
  virtual unsigned get_nof_samples() const = 0;

  /// \brief Get a channel buffer.
  /// \param[in] channel_idx Indicates the channel index.
  /// \return A view to a baseband buffer.
  /// \note An invalid channel index causes undefined behaviour.
  virtual span<radio_sample_type> get_channel_buffer(unsigned channel_idx) = 0;

  /// See get_channel_buffer() documentation.
  span<radio_sample_type> operator[](unsigned channel_index) { return get_channel_buffer(channel_index); }
};

/// \brief Describes a baseband buffer implementation that comprises a fix number of channels that can be dynamically
/// resized.
///
/// It contains a fix get_nof_channels() number of channels that contain the same number of get_nof_samples() samples.
/// The number of samples can be changed in runtime without re-allocating memory using resize(). The number samples
/// shall never exceed the maximum number of samples indicated in the constructor.
class baseband_gateway_buffer_dynamic : public baseband_gateway_buffer
{
private:
  unsigned                       nof_channels;
  unsigned                       nof_samples;
  std::vector<radio_sample_type> data;

public:
  // See interface for documentation.
  unsigned get_nof_channels() const override { return nof_channels; }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation.
  span<radio_sample_type> get_channel_buffer(unsigned channel_idx) override
  {
    srsran_always_assert(!data.empty(), "Data is empty. Was the buffer moved?");
    srsran_always_assert(
        channel_idx < nof_channels, "Channel index ({}) is out-of-range ({}).", channel_idx, nof_channels);
    unsigned offset = nof_samples * channel_idx;
    return span<radio_sample_type>(data).subspan(offset, nof_samples);
  }

  /// \brief Resize buffer.
  /// \param[in] new_nof_samples Indicates the new number of samples per channel.
  /// \note The new number of samples must be greater than 0 and must not exceed the maximum number of samples.
  void resize(unsigned new_nof_samples)
  {
    srsran_always_assert(!data.empty(), "Data is empty. Was the buffer moved?");
    srsran_always_assert(new_nof_samples > 0 && new_nof_samples * nof_channels <= data.size(),
                         "Invalid new size ({}). Maximum size ({}).",
                         new_nof_samples,
                         data.size() / nof_channels);
    nof_samples = new_nof_samples;
  }

  /// \brief Default constructor.
  /// \param[in] nof_channels_ Indicates the number of channels to create.
  /// \param[in] max_nof_samples_  Indicates the maximum number of samples.
  baseband_gateway_buffer_dynamic(unsigned nof_channels_, unsigned max_nof_samples_) :
    nof_channels(nof_channels_), nof_samples(max_nof_samples_), data(nof_channels_ * max_nof_samples_)
  {
    srsran_always_assert(nof_channels > 0, "Invalid number of channels ({}).", nof_channels);
    srsran_always_assert(nof_samples > 0, "Invalid maximum number of samples ({}).", nof_samples);
  }

  /// Move constructor.
  baseband_gateway_buffer_dynamic(baseband_gateway_buffer_dynamic&& other) :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Reset the other number of channels and samples to 0 to be consistent with other.data.size().
    other.nof_channels = 0;
    other.nof_samples  = 0;
  }
};

} // namespace srsgnb

#endif // SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BUFFER_H
