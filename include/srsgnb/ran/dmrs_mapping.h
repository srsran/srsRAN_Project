
#ifndef SRSGNB_DMRS_MAPPING_H
#define SRSGNB_DMRS_MAPPING_H

namespace srsgnb {

/// Defines the DMRS for PDSCH and PUSCH types
enum class dmrs_type { TYPE1 = 0, TYPE2 };

/// \brief Calculates the number of resource elements that carry DMRS for PDSCH or PUSCH per resource block and symbol.
///
/// \param[in] type Provides the type of DMRS.
/// \return The number of resource elements occupied by DMRS in a resource block and symbol.
static inline constexpr unsigned dmrs_nof_dmrs_per_rb(dmrs_type type)
{
  return (type == dmrs_type::TYPE1) ? 6 : 4;
}

/// Defines the maxinum number of ports for DMRS type 1.
static constexpr unsigned DMRS_MAX_PORTS_TYPE1 = 8;

/// Defines the maxinum number of ports for DMRS type 2.
static constexpr unsigned DMRS_MAX_PORTS_TYPE2 = 12;

/// Provides the maximum number of ports supported by the DMRS type.
static inline constexpr unsigned dmrs_max_ports_type(dmrs_type type)
{
  return (type == dmrs_type::TYPE1) ? DMRS_MAX_PORTS_TYPE1 : DMRS_MAX_PORTS_TYPE2;
}

/// Defines the maximum number of ports DMRS can be map to.
static constexpr unsigned DMRS_MAX_NPORTS =
    std::max(dmrs_max_ports_type(dmrs_type::TYPE1), dmrs_max_ports_type(dmrs_type::TYPE2));

} // namespace srsgnb

#endif // SRSGNB_DMRS_MAPPING_H
