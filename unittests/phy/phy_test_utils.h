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

// Constructs a factory of spy components.
#define PHY_SPY_FACTORY_TEMPLATE(COMPONENT)                                                                            \
  class COMPONENT##_factory_spy : public COMPONENT##_factory                                                           \
  {                                                                                                                    \
  public:                                                                                                              \
    std::unique_ptr<COMPONENT> create() override                                                                       \
    {                                                                                                                  \
      std::unique_ptr<COMPONENT##_spy> spy = std::make_unique<COMPONENT##_spy>();                                      \
      entries.push_back(spy.get());                                                                                    \
      return spy;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    std::vector<COMPONENT##_spy*>& get_entries() { return entries; }                                                   \
                                                                                                                       \
  private:                                                                                                             \
    std::vector<COMPONENT##_spy*> entries;                                                                             \
  }
