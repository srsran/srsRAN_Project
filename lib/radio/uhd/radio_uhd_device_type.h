/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsgnb {

class radio_uhd_device_type
{
public:
  enum class types { UNKNOWN = 0, B200, X300, N300, E3X0 };

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
    } else if (mboard_name.find("N3") != std::string::npos) {
      type = types::N300;
    } else {
      type = types::UNKNOWN;
    }
  }
  std::string to_string()
  {
    switch (type) {
      case types::UNKNOWN:
        return "uhd_unknown";
      case types::B200:
        return "uhd_b200";
      case types::X300:
        return "uhd_x300";
      case types::N300:
        return "uhd_n300";
      case types::E3X0:
        return "uhd_e3x0";
    }
  }
  bool operator==(const radio_uhd_device_type& other) const { return type == other.type; }
  bool operator==(const types& other) const { return type == other; }
       operator const types&() { return type; }

private:
  types type = types::UNKNOWN;
};
} // namespace srsgnb
