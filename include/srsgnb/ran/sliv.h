
#pragma once

#include <type_traits>

namespace srsran {

/// \brief Convert SLIV to start S and length L.
/// \param[in] N Max length.
/// \param[in] sliv An index giving a combination (jointly encoded) of start and length indicator (SLIV).
/// \param[out] S Start.
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

/// \brief Convert start S and length L into SLIV.
/// \param[in] N Max length.
/// \param[out] S Start symbols.
/// \param[out] L Length.
/// \return An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
constexpr inline unsigned sliv_from_s_and_l(unsigned N, unsigned S, unsigned L)
{
  if ((L - 1) <= N / 2) {
    return N * (L - 1) + S;
  }
  return N * (N - L + 1) + (N - 1 - S);
}

} // namespace srsran
