/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/config_parsers.h"
#include "srsgnb/support/error_handling.h"
#include <yaml-cpp/yaml.h>

using namespace srsgnb;

namespace {

/// \brief YAML configuration parser implementation.
class yaml_config_parser : public CLI::Config
{
public:
  std::string to_config(const CLI::App* app, bool default_also, bool, std::string) const override;

  std::vector<CLI::ConfigItem> from_config(std::istream& input) const override;

private:
  std::vector<CLI::ConfigItem> from_config_impl(const YAML::Node&               config,
                                                const std::vector<std::string>& prefix = {}) const;
};

} // namespace

std::string yaml_config_parser::to_config(const CLI::App* app, bool default_also, bool, std::string) const
{
  YAML::Node config;

  for (const CLI::Option* opt : app->get_options()) {
    // Only process the option that has a long-name (starts with a --) and is configurable.
    if (opt->get_lnames().empty() || !opt->get_configurable()) {
      continue;
    }

    const std::string& name = opt->get_lnames().front();

    // Non-flags.
    if (opt->get_type_size() != 0) {
      // If the option was found on command line.
      if (opt->count() == 1) {
        config[name] = opt->results().front();
      } else if (opt->count() > 1) {
        // Recover the items from the string.
        for (const auto& str : opt->results()) {
          config[name].push_back(YAML::Load(str));
        }
      } else if (default_also && !opt->get_default_str().empty()) {
        // If the option has a default and is requested by optional argument.
        config[name] = opt->get_default_str();
      }
      continue;
    }

    // Flags.
    if (opt->count() == 1) {
      // Flag, one passed.
      config[name] = true;
      continue;
    }
    if (opt->count() > 1) {
      // Flag, multiples passed.
      config[name] = opt->count();
      continue;
    }
    if (opt->count() == 0 && default_also) {
      // Flag, not present.
      config[name] = false;
      continue;
    }
  }

  for (const CLI::App* subcom : app->get_subcommands({})) {
    config[subcom->get_name()] = YAML::Load(to_config(subcom, default_also, false, ""));
  }

  return YAML::Dump(config);
}

std::vector<CLI::ConfigItem> yaml_config_parser::from_config(std::istream& input) const
{
  YAML::Node config = YAML::Load(input);
  return from_config_impl(config);
}

std::vector<CLI::ConfigItem> yaml_config_parser::from_config_impl(const YAML::Node&               config,
                                                                  const std::vector<std::string>& prefix) const
{
  std::vector<CLI::ConfigItem> results;

  // Opening ConfigItem that enables subcommand callbacks.
  // This block inserts an especial ConfigItem in CLI that enables the subcommand to execute callbacks (preparse, parse
  // and finish CLI callbacks). It is done by surrounding the subcommand with an ConfigItem with name '++' before the
  // subcommand and another ConfigItem with name '--' when the subcommand finishes.
  {
    results.emplace_back();
    CLI::ConfigItem& res = results.back();
    res.name             = "++";
    res.parents          = prefix;
    res.inputs           = {};
  }

  for (const auto& node : config) {
    const auto& key      = node.first;
    const auto& value    = node.second;
    const auto& key_name = key.as<std::string>();

    if (value.IsScalar()) {
      results.emplace_back();
      CLI::ConfigItem& res = results.back();
      res.name             = key_name;
      res.parents          = prefix;
      res.inputs           = {value.Scalar()};
      continue;
    }

    if (value.IsMap()) {
      auto copy_prefix = prefix;
      copy_prefix.push_back(key_name);

      auto sub_results = from_config_impl(value, copy_prefix);
      results.insert(results.end(), sub_results.begin(), sub_results.end());
      continue;
    }
    // Sequences are stored as a vector of strings.
    if (value.IsSequence()) {
      results.emplace_back();
      CLI::ConfigItem& res = results.back();
      res.name             = key_name;
      res.parents          = prefix;
      for (const auto& str : value) {
        res.inputs.push_back(YAML::Dump(str));
      }
    }
  }

  // Closing ConfigItem that enables subcommand callbacks.
  {
    results.emplace_back();
    CLI::ConfigItem& res = results.back();
    res.name             = "--";
    res.parents          = prefix;
    res.inputs           = {};
  }

  return results;
}

std::unique_ptr<CLI::Config> srsgnb::create_yaml_config_parser()
{
  return std::make_unique<yaml_config_parser>();
}
