/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsran {

class radio_uhd_device_type
{
public:
  enum class types {
    /// The device is not registered.
    UNKNOWN = 0,
    /// B2xx series models: B200mini, B200mini-i, B205mini-i, B210 and B200.
    B200,
    /// USRP Embedded series: E320, E312, E310 and E313.
    E3X0,
    /// N3x0 series models: N300 and N310.
    N300,
    /// N32x series models: N320 and N321.
    N320,
    /// X3x0 series models: X300 and X310.
    X300,
    /// X410 model.
    X400
  };

  radio_uhd_device_type() = default;
  radio_uhd_device_type(types type_) : type(type_) {}
  radio_uhd_device_type(std::string mboard_name)
  {
    for (char& c : mboard_name) {
      c = std::toupper(c);
    }

    if (mboard_name.find("B2") != std::string::npos) {
      type = types::B200;
    } else if (mboard_name.find("X3") != std::string::npos) {
      type = types::X300;
    } else if (mboard_name.find("X4") != std::string::npos) {
      type = types::X400;
    } else if (mboard_name.find("N32") != std::string::npos) {
      type = types::N320;
    } else if (mboard_name.find("N3") != std::string::npos) {
      type = types::N300;
    } else {
      type = types::UNKNOWN;
    }
  }
  std::string to_string()
  {
    switch (type) {
      case types::B200:
        return "uhd_b200";
      case types::E3X0:
        return "uhd_e3x0";
      case types::N300:
        return "uhd_n300";
      case types::N320:
        return "uhd_n320";
      case types::X300:
        return "uhd_x300";
      case types::X400:
        return "uhd_x400";
      case types::UNKNOWN:
      default:
        return "uhd_unknown";
    }
  }
  bool operator==(const radio_uhd_device_type& other) const { return type == other.type; }
  bool operator==(const types& other) const { return type == other; }
       operator const types&() { return type; }

private:
  types type = types::UNKNOWN;
};
} // namespace srsran
