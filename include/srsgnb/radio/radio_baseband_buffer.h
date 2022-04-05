
#ifndef SRSGNB_RADIO_RADIO_BASEBAND_BUFFER_H
#define SRSGNB_RADIO_RADIO_BASEBAND_BUFFER_H

#include "radio_base.h"
#include "srsgnb/adt/span.h"
#include <complex>
#include <vector>

namespace srsgnb {

/// Defines the radio sample type.
using radio_sample_type = std::complex<float>;

/// Describes a baseband buffer interface.
class radio_baseband_buffer : public radio_base
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

  /// \brief See for get_channel_buffer() documentation.
  span<radio_sample_type> operator[](unsigned channel_index) { return get_channel_buffer(channel_index); }
};

/// Describes a baseband
class radio_baseband_buffer_dynamic : public radio_baseband_buffer
{
private:
  unsigned                       nof_channels;
  unsigned                       nof_samples;
  std::vector<radio_sample_type> data;

public:
  unsigned                get_nof_channels() const override { return nof_channels; }
  unsigned                get_nof_samples() const override { return nof_samples; }
  span<radio_sample_type> get_channel_buffer(unsigned channel_idx) override
  {
    unsigned offset = nof_samples * channel_idx;
    return span<radio_sample_type>(data).subspan(offset, nof_samples);
  }

  /// Resize buffer.
  void resize(unsigned new_nof_samples)
  {
    srsran_always_assert(new_nof_samples <= data.size(), "New size exceeds maximum size");
    nof_samples = new_nof_samples;
  }

  /// Default constructor.
  radio_baseband_buffer_dynamic(unsigned nof_channels_, unsigned max_nof_samples_) :
    nof_channels(nof_channels_), nof_samples(max_nof_samples_), data(nof_channels_ * max_nof_samples_)
  {
    // Do nothing.
  }

  /// Move constructor.
  radio_baseband_buffer_dynamic(radio_baseband_buffer_dynamic&& other) :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Do nothing.
  }
};

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_BASEBAND_BUFFER_H
