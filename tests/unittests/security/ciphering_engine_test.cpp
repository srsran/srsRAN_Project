/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/security/ciphering_engine_nea2.h"
#include "srsran/security/ciphering_engine.h"
#include "srsran/security/security.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::security;

/// Fixture class for ciphering engine tests
class ciphering_engine_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init SEC logger
    srslog::fetch_basic_logger("SEC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SEC", false).set_hex_dump_max_size(-1);

    logger.info("Created fixture for ciphering engine test");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
};

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
sec_key make_sec_key(const std::string& hex_str)
{
  byte_buffer key_buf = make_byte_buffer(hex_str).value();
  sec_key     key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// Converts a hex string (e.g. 01FA02) to a sec_128_as_key.
sec_128_key make_sec_128_key(const std::string& hex_str)
{
  byte_buffer key_buf = make_byte_buffer(hex_str).value();
  sec_128_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// Compares two byte arrays
int arrcmp(uint8_t const* const a, uint8_t const* const b, uint32_t len)
{
  uint32_t i = 0;

  for (i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return a[i] - b[i];
    }
  }
  return 0;
}

/// 128-NEA2 Test Set 1
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset1)
{
  // Testdata in plain format
  const char* key_cstr  = "d3c5d592327fb11c4035c6680af8c6d1";
  uint32_t    count     = 0x398a59b4;
  uint8_t     bearer    = 0x15;
  uint8_t     direction = 1;
  // uint32_t    length          = 253;
  const char* plaintext_cstr  = "981ba6824c1bfb1ab485472029b71d808ce33e2cc3c0b5fc1f3de8a6dc66b1f0";
  const char* ciphertext_cstr = "e9fed8a63d155304d71df20bf3e82214b20ed7dad2f233dc3c22d7bdeeed8e78";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

/// 128-NEA2 Test Set 2
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset2)
{
  // Testdata in plain format
  const char* key_cstr  = "2bd6459f82c440e0952c49104805ff48";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  // uint32_t    length    = 798;
  const char* plaintext_cstr =
      "7ec61272743bf1614726446a6c38ced166f6ca76eb5430044286346cef130f92922b03450d3a9975e5bd2ea0eb55ad8e1b199e3ec4316020"
      "e9a1b285e762795359b7bdfd39bef4b2484583d5afe082aee638bf5fd5a606193901a08f4ab41aab9b134880";
  const char* ciphertext_cstr =
      "5961605353c64bdca15b195e288553a910632506d6200aa790c4c806c99904cf2445cc50bb1cf168a49673734e081b57e324ce5259c0e78d"
      "4cd97b870976503c0943f2cb5ae8f052c7b7d392239587b8956086bcab18836042e2e6ce42432a17105c53d0";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

/// 128-NEA2 Test Set 3
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset3)
{
  // Testdata in plain format
  const char* key_cstr  = "0a8b6bd8d9b08b08d64e32d1817777fb";
  uint32_t    count     = 0x544d49cd;
  uint8_t     bearer    = 0x04;
  uint8_t     direction = 0;
  // uint32_t    length          = 310;
  const char* plaintext_cstr  = "fd40a41d370a1f65745095687d47ba1d36d2349e23f644392c8ea9c49d40c13271aff264d0f248";
  const char* ciphertext_cstr = "75750d37b4bba2a4dedb34235bd68c6645acdaaca48138a3b0c471e2a7041a576423d2927287f0";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

/// 128-NEA2 Test Set 4
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset4)
{
  // Testdata in plain format
  const char* key_cstr  = "aa1f95aea533bcb32eb63bf52d8f831a";
  uint32_t    count     = 0x72d8c671;
  uint8_t     bearer    = 0x10;
  uint8_t     direction = 1;
  // uint32_t    length         = 1022;
  const char* plaintext_cstr = "fb1b96c5c8badfb2e8e8edfde78e57f2ad81e74103fc430a534dcc37afcec70e1517bb06f27219dae49022d"
                               "dc47a068de4c9496a951a6b09edbdc864c7adbd740ac50c022f3082bafd22d78197c5d508b977bca13f32e6"
                               "52e74ba728576077ce628c535e87dc6077ba07d29068590c8cb5f1088e082cfa0ec961302d69cf3d44";
  const char* ciphertext_cstr = "dfb440acb3773549efc04628aeb8d8156275230bdc690d94b00d8d95f28c4b56307f60f4ca55eba661ebba"
                                "72ac808fa8c49e26788ed04a5d606cb418de74878b9a22f8ef29590bc4eb57c9faf7c41524a885b8979c42"
                                "3f2f8f8e0592a9879201be7ff9777a162ab810feb324ba74c4c156e04d39097209653ac33e5a5f2d8864";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

/// 128-NEA2 Test Set 5
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset5)
{
  // Testdata in plain format
  const char* key_cstr  = "9618ae46891f86578eebe90ef7a1202e";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  // uint32_t    length    = 1245;
  const char* plaintext_cstr =
      "8daa17b1ae050529c6827f28c0ef6a1242e93f8b314fb18a77f790ae049fedd612267fecaefc450174d76d9f9aa7755a30cd90a9a5874bf4"
      "8eaf70eea3a62a250a8b6bd8d9b08b08d64e32d1817777fb544d49cd49720e219dbf8bbed33904e1fd40a41d370a1f65745095687d47ba1d"
      "36d2349e23f644392c8ea9c49d40c13271aff264d0f24841d6465f0996ff84e65fc517c53efc3363c38492a8";
  const char* ciphertext_cstr =
      "919c8c33d66789703d05a0d7ce82a2aeac4ee76c0f4da050335e8a84e7897ba5df2f36bd513e3d0c8578c7a0fcf043e03aa3a39fbaad7d15"
      "be074faa5d9029f71fb457b647834714b0e18f117fca10677945096c8c5f326ba8d6095eb29c3e36cf245d1622aafe921f7566c4f5d644f2"
      "f1fc0ec684ddb21349747622e209295d27ff3f95623371d49b147c0af486171f22cd04b1cbeb2658223e6938";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

/// 128-NEA2 Test Set 6
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST_F(ciphering_engine_test, security_nea2_testset6)
{
  // Testdata in plain format
  const char* key_cstr  = "54f4e2e04c83786eec8fb5abe8e36566";
  uint32_t    count     = 0xaca4f50f;
  uint8_t     bearer    = 0x0b;
  uint8_t     direction = 0;
  // uint32_t    length    = 3861;
  const char* plaintext_cstr =
      "40981ba6824c1bfb4286b299783daf442c099f7ab0f58d5c8e46b104f08f01b41ab485472029b71d36bd1a3d90dc3a41b46d51672ac4c966"
      "3a2be063da4bc8d2808ce33e2cccbfc634e1b259060876a0fbb5a437ebcc8d31c19e4454318745e3fa16bb11adae248879fe52db2543e53c"
      "f445d3d828ce0bf5c560593d97278a59762dd0c2c9cd68d4496a792508614014b13b6aa51128c18cd6a90b87978c2ff1cabe7d9f898a411b"
      "fdb84f68f6727b1499cdd30df0443ab4a66653330bcba1105e4cec034c73e605b4310eaaadcfd5b0ca27ffd89d144df4792759427c9cc1f8"
      "cd8c87202364b8a687954cb05a8d4e2d99e73db160deb180ad0841e96741a5d59fe4189f15420026fe4cd12104932fb38f735340438aaf7e"
      "ca6fd5cfd3a195ce5abe65272af607ada1be65a6b4c9c0693234092c4d018f1756c6db9dc8a6d80b888138616b681262f954d0e771174878"
      "0d92291d86299972db741cfa4f37b8b56cdb18a7ca8218e86e4b4b716a4d04371fbec262fc5ad0b3819b187b97e55b1a4d7c19ee24c8b4d7"
      "723cfedf045b8acae4869517d80e50615d9035d5d9c5a40af602280b542597b0cb18619eeb35925759d195e100e8e4aa0c38a3c2abe0f3d8"
      "ff04f3c33c295069c23694b5bbeacdd542e28e8a94edb9119f412d054be1fa7200b090";
  const char* ciphertext_cstr =
      "5cb72c6edc878f1566e10253afc364c9fa540d914db94cbee275d0917ca6af0d77acb4ef3bbe1a722b2ef5bd1d4b8e2aa5024ec1388a201e"
      "7bce7920aec615895f763a5564dcc4c482a2ee1d8bfecc4498eca83fbb75f9ab530e0dafbede2fa5895b82991b6277c529e0f2529d7f7960"
      "6be96706296dedfa9d7412b616958cb563c678c02825c30d0aee77c4c146d2765412421a808d13cec819694c75ad572e9b973d948b81a933"
      "7c3b2a17192e22c2069f7ed1162af44cdea817603665e807ce40c8e0dd9d6394dc6e31153fe1955c47afb51f2617ee0c5e3b8ef1ad7574ed"
      "343edc2743cc94c990e1f1fd264253c178dea739c0befeebcd9f9b76d49c1015c9fecf50e53b8b5204dbcd3eed863855dabcdcc94b31e318"
      "021568855c8b9e52a981957a112827f978ba960f1447911b317b5511fbcc7fb13ac153db74251117e4861eb9e83bffffc4eb7755579038e5"
      "7924b1f78b3e1ad90bab2a07871b72db5eef96c334044966db0c37cafd1a89e5646a3580eb6465f121dce9cb88d85b96cf23ccccd4280767"
      "bee8eeb23d8652461db6493103003baf89f5e18261ea43c84a92ebffffe4909dc46c5192f825f770600b9602c557b5f8b431a79d45977dd9"
      "c41b863da9e142e90020cfd074d6927b7ab3b6725d1a6f3f98b9c9daa8982aff067828";

  // Pack hex strings into srsran types
  sec_128_key        key        = make_sec_128_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr).value();
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, count);
  ASSERT_TRUE(result.buf.has_value());
  EXPECT_EQ(result.buf.value(), ciphertext);
}

int main(int argc, char** argv)
{
  srslog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
