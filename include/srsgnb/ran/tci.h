/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "csi_rs.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "ssb_configuration.h"
#include <cstdint>

namespace srsgnb {

/// \brief TCI-State id used to identify a TCI-State configuration.
/// \remark See TS 38.331, "TCI-StateId" and "maxNrofTCI".
enum tci_state_id_t : uint8_t { MIN_TCI_STATE_ID = 0, MAX_TCI_STATE_ID = 127, MAX_NOF_TCI_STATES = 128 };

/// \brief Associates one or two DL reference signals with a corresponding quasi-colocation (QCL) type.
/// \remark See TS 38.331, TCI-State.
struct qcl_info {
  bool operator==(const qcl_info& rhs) const
  {
    return cell == rhs.cell && bwp_id == rhs.bwp_id && ref_sig == rhs.ref_sig && qcl_type == rhs.qcl_type;
  }
  bool operator!=(const qcl_info& rhs) const { return !(rhs == *this); }

  /// \brief Reference signal with which quasi-collocation information is provided as specified in TS 38.214,
  /// subclause 5.1.5.
  struct reference_signal {
    /// \brief DL reference signal type.
    enum class reference_signal_type { csi_rs, ssb };

    bool operator==(const reference_signal& rhs) const
    {
      bool ret = type == rhs.type;
      switch (type) {
        case reference_signal_type::csi_rs:
          return ret && csi_rs == rhs.csi_rs;
        case reference_signal_type::ssb:
          return ret && ssb == rhs.ssb;
      }
      return false;
    }
    bool operator!=(const reference_signal& rhs) const { return !(rhs == *this); }

    reference_signal_type type;
    union {
      nzp_csi_rs_res_id_t csi_rs;
      ssb_id_t            ssb;
    };
  };

  /// \brief QCL type as specified in TS 38.214, subclause 5.1.5.
  enum class qcl_type : uint8_t { type_a = 0, type_b, type_c, type_d };

  /// The UE's serving cell in which the referenceSignal is configured. If the field is absent, it applies to the
  /// serving cell in which the TCI-State is configured. The RS can be located on a serving cell other than the serving
  /// cell in which the TCI-State is configured only if the qcl-Type is configured as typeC or typeD.
  optional<du_cell_index_t> cell;
  /// The DL BWP which the RS is located in. This field is mandatory present if csi-rs is included, absent otherwise.
  optional<bwp_id_t> bwp_id;
  reference_signal   ref_sig;
  qcl_type           qcl_type;
};

/// \brief Associates one or two DL reference signals with a corresponding quasi-colocation (QCL) type.
/// \remark See TS 38.331, TCI-State.
struct tci_state {
  bool operator==(const tci_state& rhs) const
  {
    return state_id == rhs.state_id && qcl_type1 == rhs.qcl_type1 && qcl_type2 == rhs.qcl_type2;
  }
  bool operator!=(const tci_state& rhs) const { return !(rhs == *this); }

  tci_state_id_t     state_id;
  qcl_info           qcl_type1;
  optional<qcl_info> qcl_type2;
};

} // namespace srsgnb
