/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "nlohmann/json.hpp"
#include "srsgnb/support/config_parsers.h"
#include "fmt/format.h"

using namespace srsgnb;

namespace {

/// \brief JSON configuration parser implementation.
class json_config_parser : public CLI::Config
{
public:
  std::string to_config(const CLI::App* app, bool default_also, bool, std::string) const override;

  std::vector<CLI::ConfigItem> from_config(std::istream& input) const override;

private:
  std::vector<CLI::ConfigItem> from_config_impl(const nlohmann::json&           j,
                                                const std::string&              name   = "",
                                                const std::vector<std::string>& prefix = {}) const;
};

} // namespace

std::string json_config_parser::to_config(const CLI::App* app, bool default_also, bool, std::string) const
{
  nlohmann::json j;

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
        j[name] = opt->results().front();
      } else if (opt->count() > 1) {
        j[name] = opt->results();
      } else if (default_also && !opt->get_default_str().empty()) {
        // If the option has a default and is requested by optional argument.
        j[name] = opt->get_default_str();
      }
      continue;
    }

    // Flags.
    if (opt->count() == 1) {
      // Flag, one passed.
      j[name] = true;
      continue;
    }
    if (opt->count() > 1) {
      // Flag, multiples passed.
      j[name] = opt->count();
      continue;
    }
    if (opt->count() == 0 && default_also) {
      // Flag, not present.
      j[name] = false;
      continue;
    }
  }

  for (const CLI::App* subcom : app->get_subcommands({})) {
    j[subcom->get_name()] = nlohmann::json::parse(to_config(subcom, default_also, false, ""));
  }

  return j.dump(4);
}

std::vector<CLI::ConfigItem> json_config_parser::from_config(std::istream& input) const
{
  nlohmann::json j;
  input >> j;
  return from_config_impl(j);
}

std::vector<CLI::ConfigItem> json_config_parser::from_config_impl(const nlohmann::json&           j,
                                                                  const std::string&              name,
                                                                  const std::vector<std::string>& prefix) const
{
  std::vector<CLI::ConfigItem> results;

  if (j.is_object()) {
    for (nlohmann::json::const_iterator i = j.cbegin(), e = j.cend(); i != e; ++i) {
      auto copy_prefix = prefix;
      if (!name.empty()) {
        copy_prefix.push_back(name);
      }
      auto sub_results = from_config_impl(*i, i.key(), copy_prefix);
      results.insert(results.end(), sub_results.begin(), sub_results.end());
    }
    return results;
  }

  if (!name.empty()) {
    results.emplace_back();
    CLI::ConfigItem& res = results.back();
    res.name             = name;
    res.parents          = prefix;

    if (j.is_boolean()) {
      res.inputs = {j.get<bool>() ? "true" : "false"};
    } else if (j.is_number()) {
      fmt::memory_buffer buffer;
      fmt::format_to(buffer, "{}", j.get<double>());
      res.inputs = {to_string(buffer)};
    } else if (j.is_string()) {
      res.inputs = {j.get<std::string>()};
    } else if (j.is_array()) {
      for (const auto& str : j) {
        res.inputs.push_back(str);
      }
    } else {
      throw CLI::ConversionError("Failed to convert " + name);
    }
    return results;
  }

  throw CLI::ConversionError("You must make all top level values objects in json!");
}

std::unique_ptr<CLI::Config> srsgnb::create_json_config_parser()
{
  return std::make_unique<json_config_parser>();
}
