/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_RESOURCE_ALLOCATION_RESOURCE_ALLOCATION_FREQUENCY_H
#define SRSGNB_RAN_RESOURCE_ALLOCATION_RESOURCE_ALLOCATION_FREQUENCY_H

namespace srsgnb {

/// \brief Describes the resource allocation in frequency domain for Type1 as per TS38.214 Section 5.1.2.2.2 for
/// Downlink and Section 6.1.2.2 for Uplink.
///
/// In downlink resource allocation of type 1, the resource block assignment information indicates to a scheduled UE a
/// set of contiguously allocated non-interleaved or interleaved virtual resource blocks.
///
/// In uplink resource allocation of type 1, the resource block assignment information indicates to a scheduled UE a
/// set of contiguously allocated non-interleaved virtual resource blocks.
struct ra_frequency_type1_configuration {
  /// \brief BWP size in resource blocks, parameter \f$N_{BWP}^{size}\f$.
  ///
  /// For downlink, the value must be set to:
  /// - the size of CORESET0 if
  ///   - the DCI is transmitted in a common SS, and
  ///   - CORESET0 is configured for the cell;
  /// - the size of the initial BWP if
  ///   - the DCI is transmitted in a common SS, and
  ///   - CORESET0 is not configured for the cell;
  /// - the active BWP size otherwise.
  ///
  /// For uplink, the value must be set to:
  /// - the size of the initial BWP \f$N_{BWP,0}^{size}\f$ if the DCI is transmitted in a common SS;
  /// - the active BWP size otherwise.
  unsigned N_bwp_size;
  /// Start VRB index \f$RB_{start}\f$.
  unsigned start_vrb;
  /// Length in terms of contiguously allocated resource blocks \f$L_{RBs}\f$.
  unsigned length_vrb;
};

/// \brief Calculates the resource indication value \f$RIV\f$ as per TS38.214 Section 5.1.2.2.2 for
/// Downlink and Section 6.1.2.2 for Uplink.
///
/// if \f$(L_{RB} - 1) \leq \left \lfloor N_{BWP}^{size}/2 \right \rfloor\f$, then <br>
///   \f$RIV = N_{BWP}^{size}(L_{RBs}-1)+RB_{start}\f$ <br>
/// Otherwise, <br>
///   \f$RIV = N_{BWP}^{size}(N_{BWP}^{size}-L_{RBs}+1)+(N_{BWP}^{size}-1-RB_{start})\f$.
unsigned ra_frequency_type1_get_riv(const ra_frequency_type1_configuration& config);

/// \brief Describes the resource allocation in frequency domain for Type1 special case as per TS38.214
/// Section 5.1.2.2.2 for Downlink and Section 6.1.2.2 for Uplink.
///
/// The special case in downlink is when the DCI size for DCI format 1_0 in USS is derived from the size of DCI format
/// 1_0 in CSS but applied to an active BWP.
///
/// The special case in uplink is when the DCI size for DCI format 0_0 in USS is derived from the initial UL BWP but
/// applied to another active BWP.
///
/// This special cases occur when step 3 in TS38.212 Section 7.3.1.0 fails. In this case, DCI formats 0_0 and 1_0 in USS
/// are derived as they were in CSS (step 4).
struct ra_frequency_type1_special_configuration {
  /// \brief Initial BWP size in resource blocks, parameter \f$N_{BWP}^{initial}\f$.
  ///
  /// For downlink, the value must be set to:
  /// - the size of CORESET 0 if CORESET 0 is configured for the cell;
  /// - the size of initial DL bandwidth part otherwise.
  ///
  /// For uplink, the value must be set to the initial bandwidth part size \f$N_{BWP,0}^{size}\f$.
  unsigned N_bwp_initial;
  /// Active BWP size in resource blocks, parameter \f$N_{BWP}^{active}\f$.
  unsigned N_bwp_active;
  /// Start VRB index \f$RB_{start}\f$.
  unsigned start_vrb;
  /// Length in terms of contiguously allocated resource blocks \f$L_{RBs}\f$.
  unsigned length_vrb;
};

/// \brief Calculates the resource indication value \f$RIV\f$ for special cases as per TS38.214 Section 5.1.2.2.2 for
/// Downlink and Section 6.1.2.2 for Uplink.
///
/// if \f$(L'_{RB} - 1) \leq \left \lfloor N_{BWP}^{initial}/2 \right \rfloor\f$, then <br>
///   \f$RIV = N_{BWP}^{initial}(L'_{RBs}-1)+RB'_{start}\f$ <br>
/// Otherwise, <br>
///   \f$RIV = N_{BWP}^{initial}(N_{BWP}^{initial}-L'_{RBs}+1)+(N_{BWP}^{initial}-1-RB'_{start})\f$. <br>
/// Where \f$L'_{RB}=L_{RB}/K\f$, \f$RB'_{start}=RB_{start}/K\f$. <br>
/// if \f$N_{BWP}^{active}>N_{BWP}^{initial}\f$ then \f$K\f$ is the maximum  value from set {1,2,4,8} which satisfies
/// \f$K \leq \left \lfloor N_{BWP}^{active}/ N_{BWP}^{initial}\right \rfloor\f$; othewise \f$K=1\f$.
unsigned ra_frequency_type1_special_get_riv(const ra_frequency_type1_special_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_RAN_RESOURCE_ALLOCATION_RESOURCE_ALLOCATION_FREQUENCY_H
