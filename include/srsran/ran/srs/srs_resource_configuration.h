/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/srs/srs_constants.h"
#include <cstdint>

namespace srsran {

/// \brief Parameters required to receive the Sounding Reference Signals described in 3GPP TS38.211 Section 6.4.1.4.
///
/// Higher layer parameters mentioned in this structure are contained in the 3GPP TS38.331 Section 6.3.2 Information
/// Element \e SRS-Resource.
struct srs_resource_configuration {
  /// Enumeration for attributes that accept only one, two, or four.
  enum class one_two_four_enum : uint8_t { one = 1, two = 2, four = 4 };
  /// Frequency hopping selection.
  enum class group_or_sequence_hopping_enum : uint8_t { neither = 0, group_hopping, sequence_hopping };
  /// Enumeration for the comb size.
  enum class comb_size_enum : uint8_t { two = 2, four = 4 };

  /// Collects parameters related to the SRS transmission periodicity.
  struct periodicity_and_offset {
    /// Periodicity in slots, parameter \f$T_{SRS}\f$.
    uint16_t periodicity;
    /// Offset within the periodicity, parameter \f$T_{offset}\f$.
    uint16_t offset;
  };

  /// Number of transmit antenna ports, parameter \f$N^{SRS}_{ap}\f$.
  one_two_four_enum nof_antenna_ports;
  /// \brief Consecutive number of OFDM symbols used for the transmission, parameter \f$N^{SRS}_{symb}\f$.
  ///
  /// It is given by the higher layer parameter \e nrofSymbols contained in the higher layer parameter
  /// \e resourceMapping.
  one_two_four_enum nof_symbols;
  /// \brief Starting position in the time domain, parameter \f$l_0\f$.
  ///
  /// The parameter is given by \f$l_0=N^{slot}_{symb}-1-l_{offset}\f$ where the offset \f$l_0\in\{0,1,...,5\}\f$
  /// counts symbols backwards from the end of the slot and is given by the field \e startPosition contained in the
  /// higher layer parameter \e resourceMapping.
  ///
  /// The valid range is {0...13}.
  ///
  /// The number of symbols plus the start symbol must not exceed the number of symbols in one slot.
  bounded_integer<uint8_t, 0, 13> start_symbol;
  /// \brief Bandwidth configuration index, parameter \f$C_{SRS}\f$.
  ///
  /// It is given by the field \e c-SRS contained in the higher-layer parameter \e freqHopping.
  ///
  /// The valid range is {0...63}.
  bounded_integer<uint8_t, 0, 63> configuration_index;
  /// \brief Sequence identifier to initialize the pseudo-random group and sequence hopping, parameter
  /// \f$n^{SRS}_{ID}\f$.
  ///
  /// It is given by the higher-layer parameter \e sequenceId.
  ///
  /// The valid range is {0...1023}.
  bounded_integer<uint16_t, 0, 1023> sequence_id;
  /// \brief Bandwidth index, parameter \f$B_{SRS}\f$.
  ///
  /// It is given by the field \e b-SRS contained in the higher-layer parameter \e freqHopping.
  ///
  /// The valid range is {0...3}.
  bounded_integer<uint8_t, 0, 3> bandwidth_index;
  /// \brief Comb size, parameter \f$K_{TC}\f$.
  ///
  /// It is given by the higher-layer field \e transmissionComb.
  ///
  /// Valid values are 2 and 4.
  comb_size_enum comb_size;
  /// \brief Comb offset, parameter \f$\bar{k}_{TC}\f$.
  ///
  /// It is given by the higher-layer field \e combOffset-n2 or \e combOffset-n4 contained in the parameter
  /// \e transmissionComb.
  ///
  /// The valid range is {0...\f$K_{TC}-1\f$}.
  bounded_integer<uint8_t, 0, 3> comb_offset;
  /// \brief Cyclic shift, parameter \f$n^{CS}_{SRS}\f$.
  ///
  /// It is given by the field \e cyclicShift contained in the higher-layer parameter \e transmissionComb.
  ///
  /// The valid range is:
  /// - {0...7} if \f$K_{TC} = 2\f$; and
  /// - {0...11} if \f$K_{TC} = 4\f$.
  bounded_integer<uint8_t, 0, 11> cyclic_shift;
  /// \brief Frequency domain position, parameter \f$n_{RRC}\f$.
  ///
  /// It is given by the higher layer parameter \e freqDomainPosition.
  ///
  /// The valid range is {0...67}.
  bounded_integer<uint8_t, 0, 67> freq_position;
  /// \brief Frequency domain shift, parameter \f$n_{shift}\f$.
  ///
  /// It is given by the higher layer parameter \e freqDomainShift.
  ///
  /// The valid range is {0...268}.
  bounded_integer<uint16_t, 0, 268> freq_shift;
  /// \brief Frequency hopping, parameter \f$b_{hop}\f$.
  ///
  /// It is given by the field \e b-hop contained in the higher layer parameter \e freqHopping.
  ///
  /// The valid range is {0...3}.
  bounded_integer<uint8_t, 0, 3> freq_hopping;
  /// \brief Group or sequence hopping configuration.
  ///
  /// It is given by the higher layer parameter \e groupOrSequenceHopping.
  group_or_sequence_hopping_enum hopping;
  /// \brief Set if frequency hopping is enabled and the SRS resource is either periodic or semi-persistent.
  ///
  /// The periodicity and offset are given by the higher layer parameter \e resourceType.
  optional<periodicity_and_offset> periodicity;
};

} // namespace srsran
