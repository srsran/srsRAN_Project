/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// Defines the maximum number of Resource Blocks (RB) that a resource grid can carry
constexpr unsigned MAX_RB = 275;

/// Defines the number of Resource Elements (RE) per Resource Block (RB) in frequency domain
constexpr unsigned NRE = 12;

/// Defines the SSB bandwidth in Resource Blocks (RBs)
constexpr unsigned SSB_BW_RB = 20;

/// SS/PBCH Block duration in OFDM symbols.
constexpr unsigned SSB_DURATION_NSYMB = 4;

/// Defines the SSB bandwidth in Resource Elements
constexpr unsigned SSB_BW_RE = NRE * SSB_BW_RB;

/// Defines the maximum number of ports for any channel transmission
constexpr unsigned MAX_PORTS = 16;

namespace phys_cell_id {

/// Number of NR N_id_1 Physical Cell Identifier (PCI) as described in TS38.211 Section 7.4.2.1 Physical-layer cell
/// identities.
constexpr unsigned NOF_NID_1 = 336;

/// Number of NR N_id_2 Physical Cell Identifier (PCI) as described in TS38.211 Section 7.4.2.1 Physical-layer cell
/// identities.
constexpr unsigned NOF_NID_2 = 3;

/// Number of NR N_id Physical Cell Identifier (PCI) as described in TS38.211 Section 7.4.2.1 Physical-layer cell
/// identities.
constexpr unsigned NOF_NID = NOF_NID_1 * NOF_NID_2;

/// Compute N_id_1 from the Physical Cell Identifier (PCI) as described in TS38.211 Section 7.4.2.1 Physical-layer cell
/// identities.
constexpr unsigned NID_1(unsigned NID)
{
  return NID / NOF_NID_2;
}

/// Compute N_id_2 from the Physical Cell Identifier (PCI) as described in TS38.211 Section 7.4.2.1 Physical-layer cell
/// identities.
constexpr unsigned NID_2(unsigned NID)
{
  return NID % NOF_NID_2;
}

} // namespace phys_cell_id
} // namespace srsran
