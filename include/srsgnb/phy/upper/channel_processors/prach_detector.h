/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"

namespace srsgnb {

/// \brief Describes a PRACH detector interface.
///
/// The PRACH sequence generation to detect is described in TS38.211 Section 6.3.3.
class prach_detector
{
public:
  /// Maximum number of preambles that can be detected in a slot.
  static constexpr unsigned MAX_NOF_PREAMBLES_PER_SLOT = 16;

  /// Describes the PRACH detector slot configuration for detecting PRACH.
  struct slot_configuration {
    /// Physical cell identifier {0...1007}.
    unsigned phys_cell_id;
    /// Root sequence index {0...837}.
    unsigned root_sequence_index;
    /// Frequency offset between Point A and the PRB overlapping with the lowest RE of the PRACH signal in PRB.
    unsigned frequency_offset;
    /// Starting symbol index for the first PRACH TD occasion in the current PRACH FD occasion.
    unsigned start_symbol;
    /// Number of occasions within a slot.
    unsigned nof_prach_occasions;
    /// Preamble format.
    preamble_format format;
    /// Start frequency domain occasion index as \f$n\f$ as per TS38.211 Section 6.3.3.2.
    unsigned start_fd_ra;
    /// Number of frequency domain occasions {1...8}.
    unsigned nof_fd_ra;
    /// Zero-correlation zone configuration as \f$N_{CS}\f$ as per TS38.211 section 6.3.3.1.
    unsigned n_cs;
    /// Start of preamble logical index to monitor in the PRACH occasions signaled in this slot {0...63}.
    unsigned start_preamble_index;
    /// Number of preamble indices to monitor {1...64}.
    unsigned nof_preamble_indices;
  };

  /// Describes the detection of a single preamble.
  struct preamble_indication {
    /// Indicates the detected preamble index.
    unsigned preamble_index;
    /// Optional timing advance measured for the UE between the observed arrival and the reference uplink time.
    float time_advance_us;
    /// Average RSRP value in dB, relative to 1.
    float power_dB;
    /// Average SNR value in dB.
    float snr_dB;
  };

  /// Describes a detection result.
  struct detection_result {
    /// Index of first symbol of the PRACH TD occasion within the slot.
    unsigned first_symbol_index;
    /// The index of first slot of the PRACH TD occasion in a system frame.
    unsigned slot_index;
    /// The index of the received PRACH frequency domain occasion.
    unsigned ra_index;
    /// Optional average RSSI value in dB, relative to 1.
    float rssi_dB;
    /// List of detected preambles.
    static_vector<preamble_indication, MAX_NOF_PREAMBLES_PER_SLOT> preambles;
  };

  /// Default destructor.
  virtual ~prach_detector() = default;

  /// \brief Detects PRACH transmissions described by \c config.
  ///
  /// \param[in] signal Provides the baseband signal buffer.
  /// \param[in] config Provides the PRACH configuration for the slot.
  /// \return The detection result.
  virtual detection_result detect(span<const cf_t> signal, const slot_configuration& config) = 0;
};

/// Describes a PRACH detector factory.
class prach_detector_factory
{
public:
  /// \brief Describes common parameters to configure a PRACH detector.
  ///
  /// The common parameters provide information about the baseband signal and processing configuration which is common
  /// for all the configurations in the gNb.
  struct common_configuration {
    /// Specifies the sampling rate.
    double sampling_rate_Hz;
    /// Channel bandwidth in PRB.
    unsigned channel_bandwidth_rb;
  };

  /// Default destructor.
  virtual ~prach_detector_factory() = default;

  /// \brief Creates a PRACH detector according to the provided configuration.
  ///
  /// \param[in] common_config Provides common PRACH detector configuration.
  /// \return Return a PRACH detector instance if the creation is successful. Otherwise, nullptr.
  virtual std::unique_ptr<prach_detector> create_prach_detector(const common_configuration& common_config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_H
