/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/rohc/rohc_config.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::rohc;

/// Fixture class for ROHC config tests
class rohc_config_test : public ::testing::Test
{
public:
  rohc_config_test()
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init ROHC logger
    srslog::fetch_basic_logger("ROHC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ROHC", false).set_hex_dump_max_size(-1);

    logger.info("ROHC config test initialized.");
  }
  ~rohc_config_test() override
  {
    // flush logger after each test
    srslog::flush();
  }

protected:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
};

TEST_F(rohc_config_test, default_values)
{
  rohc_config cfg = {};
  logger.debug("Default config: {}", cfg);
  EXPECT_EQ(cfg.rohc_type, rohc_type_t::rohc);
  EXPECT_EQ(cfg.max_cid, default_rohc_max_cid);
  for (auto profile : all_rohc_profiles) {
    EXPECT_FALSE(cfg.profiles.is_profile_enabled(profile)) << fmt::format("Unexpected active profile: {}\n", profile);
  }
  EXPECT_FALSE(cfg.continue_rohc);
}

TEST_F(rohc_config_test, format)
{
  rohc_config cfg = {};
  logger.debug("Config: {}", cfg);
  std::string str = fmt::format("{}", cfg);
  EXPECT_EQ(str, "rohc_type=rohc max_cid=15 profiles= continue_rohc=false");
  EXPECT_NE(str.find("rohc_type=rohc"), std::string::npos);
  EXPECT_NE(str.find("max_cid=15"), std::string::npos);
  EXPECT_NE(str.find("profiles="), std::string::npos);
  EXPECT_NE(str.find("continue_rohc=false"), std::string::npos);

  cfg.profiles.set_profile(srsran::rohc::rohc_profile::profile0x0001, true);
  cfg.profiles.set_profile(srsran::rohc::rohc_profile::profile0x0006, true);
  logger.debug("Config: {}", cfg);
  str = fmt::format("{}", cfg);
  EXPECT_NE(str.find("profiles="), std::string::npos);
  EXPECT_NE(str.find("v1-RTP/UDP/IP"), std::string::npos);
  EXPECT_NE(str.find("v1-TCP/IP"), std::string::npos);

  cfg.profiles.set_profile(srsran::rohc::rohc_profile::profile0x0006, false);
  logger.debug("Config: {}", cfg);
  str = fmt::format("{}", cfg);
  EXPECT_NE(str.find("profiles="), std::string::npos);
  EXPECT_NE(str.find("v1-RTP/UDP/IP"), std::string::npos);
  EXPECT_EQ(str.find("v1-TCP/IP"), std::string::npos);
}

TEST_F(rohc_config_test, set_get_profile_bitmap)
{
  rohc_config cfg = {};
  for (uint16_t bitmap = 0; bitmap <= 0x1ff; bitmap++) {
    cfg.profiles.set_profile_bitmap(bitmap);
    logger.debug("Config: {}", cfg);
    for (auto profile : all_rohc_profiles) {
      if (bitmap & (1 << to_number(profile))) {
        EXPECT_TRUE(cfg.profiles.is_profile_enabled(profile))
            << fmt::format("Unexpected inactive profile: {}\n", profile);
      } else {
        EXPECT_FALSE(cfg.profiles.is_profile_enabled(profile))
            << fmt::format("Unexpected active profile: {}\n", profile);
      }
      EXPECT_EQ(cfg.profiles.get_profile_bitmap(), bitmap);
    }
  }
}

TEST_F(rohc_config_test, set_unset_profile)
{
  rohc_config cfg = {};
  for (uint16_t bitmap = 0; bitmap <= 0x1ff; bitmap++) {
    cfg.profiles.set_profile_bitmap(bitmap);
    logger.debug("Config: {}", cfg);
    // Toggle each profile twice and check results.
    for (auto profile : all_rohc_profiles) {
      uint16_t old_bitmap = cfg.profiles.get_profile_bitmap();
      bool     old_state  = cfg.profiles.is_profile_enabled(profile);
      cfg.profiles.set_profile(profile, !old_state);
      // The profile state changed.
      EXPECT_NE(cfg.profiles.is_profile_enabled(profile), old_state);
      // Only one bit has flipped in bitmap.
      uint16_t new_bitmap = old_bitmap ^ (1 << to_number(profile));
      EXPECT_EQ(cfg.profiles.get_profile_bitmap(), new_bitmap);

      cfg.profiles.set_profile(profile, old_state);
      // The profile state changed back.
      EXPECT_EQ(cfg.profiles.is_profile_enabled(profile), old_state);
      // The bitmap changed back.
      EXPECT_EQ(cfg.profiles.get_profile_bitmap(), old_bitmap);
    }
  }
}

#ifdef ASSERTS_ENABLED
TEST_F(rohc_config_test, set_invalid_profile_bitmap)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");

  rohc_config cfg = {};
  ASSERT_DEATH(cfg.profiles.set_profile_bitmap(0x200), R"(Invalid ROHC profile bitmap=0x200 exceeds 0x1ff)");
}
#endif

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
