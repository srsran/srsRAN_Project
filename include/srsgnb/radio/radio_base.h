
#ifndef SRSGNB_RADIO_RADIO_BASE_H
#define SRSGNB_RADIO_RADIO_BASE_H

namespace srsgnb {

/// Describes the base interface for radio classes.
class radio_base
{
protected:
  /// Allow default constructor for child.
  radio_base() = default;

public:
  /// Provides default destructor.
  virtual ~radio_base() = default;

  /// Forbid copy constructor.
  radio_base(const radio_base& other) = delete;

  /// Forbid move constructor.
  radio_base(const radio_base&& other) = delete;

  /// Forbid copy assigment operator.
  radio_base& operator=(const radio_base&) = delete;

  /// Forbid move assigment operator.
  radio_base& operator=(radio_base&&) = delete;
};
} // namespace srsgnb

#endif // SRSGNB_RADIO_BASE_H
