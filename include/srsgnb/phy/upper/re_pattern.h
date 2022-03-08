
#ifndef SRSGNB_RAN_RE_PATTERN_H_
#define SRSGNB_RAN_RE_PATTERN_H_

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"

namespace srsgnb {

/// Describes a resource element pattern within a resource grid.
struct re_pattern {
  /// Resource block where the pattern begins in frequency domain, the range is (0...274).
  unsigned rb_begin{0};
  /// Resource block where the pattern ends in frequency domain (excluded), the range is (1...275).
  unsigned rb_end{1};
  /// Resource block index jump.
  unsigned rb_stride{1};
  /// Resource element mask per resource block. True entries indicate the resource elements affected by the pattern.
  std::array<bool, NRE> re_mask{};
  /// Symbol mask. True entries indicate the symbols affected by the pattern.
  std::array<bool, NSYMB_PER_SLOT_NORM> symbol_mask{};

  /// Default constructor.
  re_pattern() = default;

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides the reference to other resource element pattern to copy.
  re_pattern(const re_pattern& other) :
    rb_begin(other.rb_begin),
    rb_end(other.rb_end),
    rb_stride(other.rb_stride),
    re_mask(other.re_mask),
    symbol_mask(other.symbol_mask)
  {
    // Do nothing.
  }

  /// \brief Compare the resource element pattern resource block allocation is equal to other resource element pattern.
  ///
  /// \param[in] other References other resource element pattern to compare.
  /// \return True if the resource element mask matches, otherwise false.
  bool rb_equal_to(const re_pattern& other) const
  {
    return (rb_begin == other.rb_begin) && (rb_end == other.rb_end) && (rb_stride == other.rb_stride);
  }

  /// \brief Compare the resource element pattern subcarrier mask is equal to other resource element pattern.
  ///
  /// \param[in] other References other resource element pattern to compare.
  /// \return True if the resource element mask matches, otherwise false.
  bool re_mask_equal_to(const re_pattern& other) const
  {
    return std::equal(re_mask.begin(), re_mask.end(), other.re_mask.begin());
  }

  /// \brief Compare the resource element pattern symbol mask is equal to other resource element pattern.
  ///
  /// \param[in] other References other resource element pattern to compare.
  /// \return True if the symbol mask matches, otherwise false.
  bool symbol_mask_equal_to(const re_pattern& other) const
  {
    return std::equal(symbol_mask.begin(), symbol_mask.end(), other.symbol_mask.begin());
  }

  /// \brief Include in a resource grid symbol mask the described resource element pattern.
  ///
  /// This method sets to true the elements that are described in the pattern for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be included.
  /// \note This method expects that mask number of elements is equal to or greater than \c rb_end
  void include_mask(span<bool> mask, unsigned symbol) const;

  /// \brief Excludes in a resource grid symbol mask the described resource element pattern.
  ///
  /// This method sets to false the elements that are described in the pattern for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that mask number of elements is equal to or greater than \c rb_end
  void exclude_mask(span<bool> mask, unsigned symbol) const;
};

/// Describes a resource element pattern list.
class re_pattern_list
{
private:
  /// Defines the maximum number or RE pattern a list can carry.
  static constexpr unsigned MAX_RE_PATTERN = 4;

  /// Resource element pattern storage.
  static_vector<re_pattern, MAX_RE_PATTERN> list = {};

public:
  /// Clear the current pattern list.
  void clear() { list.clear(); }

  /// Get the number of internal resource element pattern.
  unsigned get_nof_entries() const { return list.size(); }

  /// \brief Merges a given resource element pattern into the list.
  ///
  /// This method tries to combine a new pattern with the existing ones in the list according to if
  /// - the resource block allocation parameters are identical, and
  /// - the symbol mask or the resource element mask is identical.
  /// Otherwise, the pattern is appended in the list.
  ///
  /// \param[in] pattern Provides the reference to a resource element pattern.
  void merge(const re_pattern& pattern);

  /// \brief Includes in a resource grid symbol mask the described resource element pattern list.
  ///
  /// This method sets to true the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be included.
  /// \note This method expects that mask number of elements is equal to or greater than \c rb_end.
  void include_mask(span<bool> mask, unsigned symbol) const;

  /// \brief Excludes in a resource grid symbol mask the described resource element pattern list
  ///
  /// This method sets to false the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that mask number of elements is equal to or greater than \c rb_end.
  void exclude_mask(span<bool> mask, unsigned symbol) const;
};

} // namespace srsgnb
#endif // SRSGNB_RAN_RE_PATTERN_H_
