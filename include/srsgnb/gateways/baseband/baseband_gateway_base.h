
#ifndef SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BASE_H
#define SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BASE_H

namespace srsgnb {

/// Describes the base interface for baseband gateway interfaces.
class baseband_gateway_base
{
protected:
  /// Allow default constructor for child.
  baseband_gateway_base() = default;

public:
  /// Provides default destructor.
  virtual ~baseband_gateway_base() = default;

  /// Forbid copy constructor.
  baseband_gateway_base(const baseband_gateway_base& other) = delete;

  /// Forbid move constructor.
  baseband_gateway_base(const baseband_gateway_base&& other) = delete;

  /// Forbid copy assigment operator.
  baseband_gateway_base& operator=(const baseband_gateway_base&) = delete;

  /// Forbid move assigment operator.
  baseband_gateway_base& operator=(baseband_gateway_base&&) = delete;
};

} // namespace srsgnb

#endif // SRSGNB_GATEWAYS_BASEBAND_BASEBAND_GATEWAY_BASE_H
