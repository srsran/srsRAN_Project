
#ifndef SRSGNB_SLIV_H
#define SRSGNB_SLIV_H

#include <type_traits>

namespace srsgnb {

/// \brief Convert SLIV to symbol start S and length L.
/// \param[in] N Number of symbols per slot.
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] S Start symbols.
/// \param[out] L Length.
template <typename Integer>
constexpr inline void sliv_to_s_and_l(unsigned N, unsigned sliv, Integer& S, Integer& L)
{
  static_assert(std::is_integral<Integer>::value, "Non-integer type passed as S + L output.");
  unsigned low  = sliv % N;
  unsigned high = sliv / N;
  if (high + 1 + low <= N) {
    S = low;
    L = high + 1;
  } else {
    S = N - 1 - low;
    L = N - high + 1;
  }
}

} // namespace srsgnb

#endif // SRSGNB_SLIV_H
