
#ifndef SRSGNB_HELPERS_H_
#define SRSGNB_HELPERS_H_

namespace srsgnb {

/// Describes the different SS/PBCH block patterns defined in 38.213 section 4.1 Cell search
enum class ssb_pattern_case { A, B, C, D, E };

struct ssb_mapping_t {
  /// Start symbol index in 5ms SS/PBCH burst
  unsigned l_start;
  /// Start subcarrier index relative to pointA
  unsigned k_start;
  /// Maximum number of SS/PBCH blocks in a 5ms SS/PBCH burst
  unsigned L_max;
};

} // namespace srsgnb

#endif // SRSGNB_HELPERS_H_
