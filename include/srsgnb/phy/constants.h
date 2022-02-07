#ifndef SRSGNB_PHY_CONSTANTS_H_
#define SRSGNB_PHY_CONSTANTS_H_

namespace srsgnb {

namespace phys_cell_id {
/**
 * @brief Number of NR N_id_1 Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
static const unsigned NOF_NID_1 = 336;

/**
 * @brief Number of NR N_id_2 Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
static const unsigned NOF_NID_2 = 336;

/**
 * @brief Number of NR N_id Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
static const unsigned NOF_NID = NOF_NID_1 * NOF_NID_2;

/**
 * @brief Compute N_id_1 from the Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1
 * Physical-layer cell identities
 */
static inline unsigned NID_1(unsigned NID)
{
  return NID / NOF_NID_2;
}

/**
 * @brief Compute N_id_2 from the Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1
 * Physical-layer cell identities
 */
static inline unsigned NID_2(unsigned NID)
{
  return NID % NOF_NID_2;
}

} // namespace phys_cell_id
} // namespace srsgnb

#endif // SRSGNB_PHY_CONSTANTS_H_
