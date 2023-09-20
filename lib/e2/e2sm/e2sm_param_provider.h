/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>
namespace srsran {

struct action_parameter_t {
  uint32_t    id;
  std::string name;
};

class e2sm_action_provider
{
public:
  e2sm_action_provider(std::string name_) : name(name_) {}
  e2sm_action_provider(const e2sm_action_provider& other) : name(other.name), action_params(other.action_params) {}

  void        add_parameter_support(uint32_t id, std::string param_name) { action_params[id] = {id, param_name}; }
  std::string name;
  std::map<uint32_t, action_parameter_t> action_params;
};

class e2sm_style_provider
{
public:
  e2sm_style_provider(std::string name_) : name(name_) {}
  e2sm_style_provider(const e2sm_style_provider& other) : name(other.name), action_providers(other.action_providers) {}
  void add_action_provider(uint16_t id, const e2sm_action_provider& provider)
  {
    action_providers.emplace(id, e2sm_action_provider(provider));
  }
  std::string                              name;
  std::map<uint32_t, e2sm_action_provider> action_providers;
};

class e2sm_service_provider
{
public:
  e2sm_service_provider(std::string name_) : name(name_) {}
  e2sm_service_provider(const e2sm_service_provider& other) : name(other.name), style_providers(other.style_providers)
  {
  }
  void add_style_provider(uint32_t id, e2sm_style_provider provider)
  {
    style_providers.emplace(id, e2sm_style_provider(provider));
  }
  std::string                             name;
  std::map<uint32_t, e2sm_style_provider> style_providers;
};

class e2sm_param_provider
{
public:
  e2sm_param_provider()
  {
    // Slice-level PRB quota(6) -> Radio Resource Allocation Control(2)
    std::vector<action_parameter_t> action_params2_6;
    action_params2_6.push_back({10, "Min PRB Policy Ratio"});
    action_params2_6.push_back({11, "Max PRB Policy Ratio"});
    e2sm_action_provider action_provider6("Slice-level PRB quota");
    for (auto& param : action_params2_6) {
      action_provider6.add_parameter_support(param.id, param.name);
    }
    e2sm_style_provider style_provider2("Radio Resource Allocation Control");
    style_provider2.add_action_provider(6, action_provider6);

    e2sm_service_provider ctrl_provider("Control Service");
    ctrl_provider.add_style_provider(2, style_provider2);
    service_providers.emplace(1, ctrl_provider);
  }
  e2sm_param_provider(const e2sm_param_provider& other) : name(other.name), service_providers(other.service_providers)
  {
  }
  void add_service_provider(uint32_t id, e2sm_service_provider service_provider)
  {
    service_providers.emplace(id, e2sm_service_provider(service_provider));
  }
  std::string                               name;
  std::map<uint32_t, e2sm_service_provider> service_providers;
};

} // namespace srsran
