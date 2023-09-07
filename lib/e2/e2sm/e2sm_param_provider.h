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
    // DRB QoS Configuration(1) -> Radio Bearer control(1)
    std::vector<action_parameter_t> action_params1_1;
    action_params1_1.push_back({1, "DRB ID"});
    action_params1_1.push_back({2, "5QI"});
    action_params1_1.push_back({3, "Packet Delay Budget"});
    action_params1_1.push_back({4, "Packet Error Rate"});

    e2sm_action_provider action_provider1("DRB QoS Configuration");
    for (auto& param : action_params1_1) {
      action_provider1.add_parameter_support(param.id, param.name);
    }
    e2sm_style_provider style_provider1("Radio Bearer control");
    style_provider1.add_action_provider(1, action_provider1);

    // Slice-level PRB quota(6) -> Radio Resource Allocation Control(2)
    std::vector<action_parameter_t> action_params2_6;
    action_params2_6.push_back({1, "RRM Policy Ratio List"});
    action_params2_6.push_back({2, "RRM Policy Ratio Group"});
    action_params2_6.push_back({3, "RRM Policy"});
    action_params2_6.push_back({4, "RRM Policy Member List"});
    action_params2_6.push_back({5, "RRM Policy Member"});
    action_params2_6.push_back({6, "PLMN Identity"});
    action_params2_6.push_back({7, "S-NSSAI"});
    action_params2_6.push_back({8, "SST"});
    action_params2_6.push_back({9, "SD"});
    action_params2_6.push_back({10, "Min PRB Policy Ratio"});
    action_params2_6.push_back({11, "Max PRB Policy Ratio"});
    action_params2_6.push_back({12, "Dedicated PRB Policy Ratio"});

    e2sm_action_provider action_provider6("Slice-level PRB quota");
    for (auto& param : action_params2_6) {
      action_provider6.add_parameter_support(param.id, param.name);
    }
    e2sm_style_provider style_provider2("Radio Resource Allocation Control");
    style_provider2.add_action_provider(6, action_provider6);

    e2sm_service_provider provider1("Control Service");
    provider1.add_style_provider(1, style_provider1);
    provider1.add_style_provider(2, style_provider2);
    service_providers.emplace(1, provider1);
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
