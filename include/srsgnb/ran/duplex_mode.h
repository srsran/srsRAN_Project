#ifndef SRSGNB_DUPLEX_MODE_H
#define SRSGNB_DUPLEX_MODE_H

namespace srsgnb {

/// NR Duplex mode.
enum class duplex_mode {
  /// Paired FDD.
  FDD = 0,
  /// Unpaired TDD.
  TDD,
  /// Supplementary DownLink (Unpaired).
  SDL,
  /// Supplementary UpLink (Unpaired).
  SUL,
  INVALID
};

} // namespace srsgnb

#endif // SRSGNB_DUPLEX_MODE_H