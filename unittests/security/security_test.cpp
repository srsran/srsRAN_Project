/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/security/security.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
sec_128_as_key make_sec_128_as_key(std::string hex_str)
{
  byte_buffer    key_buf = make_byte_buffer(hex_str);
  sec_128_as_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// 128-NEA1 Test Set 1
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset1)
{
  // Testdata in plain format
  const char* key_cstr        = "D3C5D592327FB11C4035C6680AF8C6D1";
  uint32_t    count           = 0x398a59b4;
  uint8_t     bearer          = 0x15;
  uint8_t     direction       = 1;
  uint32_t    length          = 253;
  const char* plaintext_cstr  = "981BA6824C1BFB1AB485472029B71D808CE33E2CC3C0B5FC1F3DE8A6DC66B1F0";
  const char* ciphertext_cstr = "5D5BFE75EB04F68CE0A12377EA00B37D47C6A0BA06309155086A859C4341B378";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA1 Test Set 2
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset2)
{
  // Testdata in plain format
  const char* key_cstr  = "2BD6459F82C440E0952C49104805FF48";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  uint32_t    length    = 798;
  const char* plaintext_cstr =
      "7EC61272743BF1614726446A6C38CED166F6CA76EB5430044286346CEF130F92922B03450D3A9975E5BD2EA0EB55AD8E1B199E3EC4316020"
      "E9A1B285E762795359B7BDFD39BEF4B2484583D5AFE082AEE638BF5FD5A606193901A08F4AB41AAB9B134880";
  const char* ciphertext_cstr =
      "3F67850714B8DA69EFB727ED7A6C0C50714AD736C4F5600006E3525BE807C467C677FF864AF45FBA09C27CDE38F87A1F84D59AB255408F2C"
      "7B82F9EAD41A1FE65EABEBFBC1F3A4C56C9A26FCF7B3D66D0220EE4775BC58170A2B12F3431D11B344D6E36C";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA1 Test Set 3
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset3)
{
  // Testdata in plain format
  const char* key_cstr        = "0A8B6BD8D9B08B08D64E32D1817777FB";
  uint32_t    count           = 0x544d49cd;
  uint8_t     bearer          = 0x04;
  uint8_t     direction       = 0;
  uint32_t    length          = 310;
  const char* plaintext_cstr  = "FD40A41D370A1F65745095687D47BA1D36D2349E23F644392C8EA9C49D40C13271AFF264D0F248";
  const char* ciphertext_cstr = "48148E5452A210C05F46BC80DC6F73495B02048C1B958B026102CA97280279A4C18D2EE308921C";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA1 Test Set 4
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset4)
{
  // Testdata in plain format
  const char* key_cstr       = "AA1F95AEA533BCB32EB63BF52D8F831A";
  uint32_t    count          = 0x72d8c671;
  uint8_t     bearer         = 0x10;
  uint8_t     direction      = 1;
  uint32_t    length         = 1022;
  const char* plaintext_cstr = "FB1B96C5C8BADFB2E8E8EDFDE78E57F2AD81E74103FC430A534DCC37AFCEC70E1517BB06F27219DAE49022D"
                               "DC47A068DE4C9496A951A6B09EDBDC864C7ADBD740AC50C022F3082BAFD22D78197C5D508B977BCA13F32E6"
                               "52E74BA728576077CE628C535E87DC6077BA07D29068590C8CB5F1088E082CFA0EC961302D69CF3D44";
  const char* ciphertext_cstr = "FFCFC2FEAD6C094E96C589D0F6779B6784246C3C4D1CEA203DB3901F40AD4FD7138BC6D77E8320CB102F49"
                                "7FDD44A269A96ECB28617700E332EB2F736B34F4F2693094E22FF94F9BE4723DA40C40DFD3931CC1AC9723"
                                "F6B4A9913E96B6DB7ABCACE415177C1D0115C5F09B5FDEA0B3ADB8F9DA6E9F9A04C543397B9D43F87330";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA1 Test Set 5
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset5)
{
  // Testdata in plain format
  const char* key_cstr  = "9618AE46891F86578EEBE90EF7A1202E";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  uint32_t    length    = 1245;
  const char* plaintext_cstr =
      "8DAA17B1AE050529C6827F28C0EF6A1242E93F8B314FB18A77F790AE049FEDD612267FECAEFC450174D76D9F9AA7755A30CD90A9A5874BF4"
      "8EAF70EEA3A62A250A8B6BD8D9B08B08D64E32D1817777FB544D49CD49720E219DBF8BBED33904E1FD40A41D370A1F65745095687D47BA1D"
      "36D2349E23F644392C8EA9C49D40C13271AFF264D0F24841D6465F0996FF84E65FC517C53EFC3363C38492A8";
  const char* ciphertext_cstr =
      "6CDB18A7CA8218E86E4B4B716A4D04371FBEC262FC5AD0B3819B187B97E55B1A4D7C19EE24C8B4D7723CFEDF045B8ACAE4869517D80E5061"
      "5D9035D5D9C5A40AF602280B542597B0CB18619EEB35925759D195E100E8E4AA0C38A3C2ABE0F3D8FF04F3C33C295069C23694B5BBEACDD5"
      "42E28E8A94EDB9119F412D054BE1FA7272B5FFB2B2570F4F7CEAF383A8A9D93572F04D6E3A6E293726EC62C8";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA1 Test Set 6
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
TEST(security_nea1_test, testset6)
{
  // Testdata in plain format
  const char* key_cstr  = "54F4E2E04C83786EEC8FB5ABE8E36566";
  uint32_t    count     = 0xaca4f50f;
  uint8_t     bearer    = 0x0b;
  uint8_t     direction = 0;
  uint32_t    length    = 3861;
  const char* plaintext_cstr =
      "40981BA6824C1BFB4286B299783DAF442C099F7AB0F58D5C8E46B104F08F01B41AB485472029B71D36BD1A3D90DC3A41B46D51672AC4C966"
      "3A2BE063DA4BC8D2808CE33E2CCCBFC634E1B259060876A0FBB5A437EBCC8D31C19E4454318745E3FA16BB11ADAE248879FE52DB2543E53C"
      "F445D3D828CE0BF5C560593D97278A59762DD0C2C9CD68D4496A792508614014B13B6AA51128C18CD6A90B87978C2FF1CABE7D9F898A411B"
      "FDB84F68F6727B1499CDD30DF0443AB4A66653330BCBA1105E4CEC034C73E605B4310EAAADCFD5B0CA27FFD89D144DF4792759427C9CC1F8"
      "CD8C87202364B8A687954CB05A8D4E2D99E73DB160DEB180AD0841E96741A5D59FE4189F15420026FE4CD12104932FB38F735340438AAF7E"
      "CA6FD5CFD3A195CE5ABE65272AF607ADA1BE65A6B4C9C0693234092C4D018F1756C6DB9DC8A6D80B888138616B681262F954D0E771174878"
      "0D92291D86299972DB741CFA4F37B8B56CDB18A7CA8218E86E4B4B716A4D04371FBEC262FC5AD0B3819B187B97E55B1A4D7C19EE24C8B4D7"
      "723CFEDF045B8ACAE4869517D80E50615D9035D5D9C5A40AF602280B542597B0CB18619EEB35925759D195E100E8E4AA0C38A3C2ABE0F3D8"
      "FF04F3C33C295069C23694B5BBEACDD542E28E8A94EDB9119F412D054BE1FA72B09550";
  const char* ciphertext_cstr =
      "351E30D4D910C5DD5AD7834C426E6C0CAB6486DA7B0FDA4CD83AF1B9647137F1AC43B434223B19BE07BD89D1CC306944D3361EA1A2F8CDBD"
      "321655976350D00B80DD838120A7755C6DEA2AB2B0C99A913F47DAE2B8DEB9A829E5469FF2E187776F6FD081E3871D119A76E24C917EA626"
      "48E02E90367564DE72AE7E4F0A4249A9A5B0E465A2D6D9DC87843B1B875CC9A3BE93D8DA8F56ECAF5981FE93C284318B0DEC7A3BA108E2CB"
      "1A61E966FA7AFA7AC7F67F65BC4A2DF070D4E434845F109AB2B68ADE3DC316CA6332A62893E0A7EC0B4FC25191BF2FF1B9F9815E4BA8A99C"
      "643B521804F7D5850DDE3952206EC6CCF340F9B3220B3023BDD063956EA8333920FDE99E0675410E49EF3B4D3FB3DF5192F99CA83D3B0032"
      "DE08C220776A5865B0E4B3B0C75DEFE7762DFF018EA7F5BE2B2F972B2A8BA5970E43BD6FDD63DAE629784EC48D610054EE4E4B5DBBF1FC2F"
      "A0B830E94DCBB7014E8AB429AB100FC48F83171D99FC258B7C2BA7C176EAEAAD37F860D597A31CE79B594733C7141DF79151FCA90C08478A"
      "5C6C2CC481D51FFECE3CD7D2581348827A71F091428EBE38C95A3F5C63E056DFB7CC45A9B7C07D834E7B20B99ED202429C14BB85FFA43B7C"
      "B68495CD75AB66D964D4CAFE64DD9404DAE2DC5110617F194FC3C184F583CD0DEF6D00";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea1(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 1
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset1)
{
  // Testdata in plain format
  const char* key_cstr        = "d3c5d592327fb11c4035c6680af8c6d1";
  uint32_t    count           = 0x398a59b4;
  uint8_t     bearer          = 0x15;
  uint8_t     direction       = 1;
  uint32_t    length          = 253;
  const char* plaintext_cstr  = "981ba6824c1bfb1ab485472029b71d808ce33e2cc3c0b5fc1f3de8a6dc66b1f0";
  const char* ciphertext_cstr = "e9fed8a63d155304d71df20bf3e82214b20ed7dad2f233dc3c22d7bdeeed8e78";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 2
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset2)
{
  // Testdata in plain format
  const char* key_cstr  = "2bd6459f82c440e0952c49104805ff48";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  uint32_t    length    = 798;
  const char* plaintext_cstr =
      "7ec61272743bf1614726446a6c38ced166f6ca76eb5430044286346cef130f92922b03450d3a9975e5bd2ea0eb55ad8e1b199e3ec4316020"
      "e9a1b285e762795359b7bdfd39bef4b2484583d5afe082aee638bf5fd5a606193901a08f4ab41aab9b134880";
  const char* ciphertext_cstr =
      "5961605353c64bdca15b195e288553a910632506d6200aa790c4c806c99904cf2445cc50bb1cf168a49673734e081b57e324ce5259c0e78d"
      "4cd97b870976503c0943f2cb5ae8f052c7b7d392239587b8956086bcab18836042e2e6ce42432a17105c53d0";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 3
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset3)
{
  // Testdata in plain format
  const char* key_cstr        = "0a8b6bd8d9b08b08d64e32d1817777fb";
  uint32_t    count           = 0x544d49cd;
  uint8_t     bearer          = 0x04;
  uint8_t     direction       = 0;
  uint32_t    length          = 310;
  const char* plaintext_cstr  = "fd40a41d370a1f65745095687d47ba1d36d2349e23f644392c8ea9c49d40c13271aff264d0f248";
  const char* ciphertext_cstr = "75750d37b4bba2a4dedb34235bd68c6645acdaaca48138a3b0c471e2a7041a576423d2927287f0";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out.length(), ciphertext.length());
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 4
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset4)
{
  // Testdata in plain format
  const char* key_cstr       = "aa1f95aea533bcb32eb63bf52d8f831a";
  uint32_t    count          = 0x72d8c671;
  uint8_t     bearer         = 0x10;
  uint8_t     direction      = 1;
  uint32_t    length         = 1022;
  const char* plaintext_cstr = "fb1b96c5c8badfb2e8e8edfde78e57f2ad81e74103fc430a534dcc37afcec70e1517bb06f27219dae49022d"
                               "dc47a068de4c9496a951a6b09edbdc864c7adbd740ac50c022f3082bafd22d78197c5d508b977bca13f32e6"
                               "52e74ba728576077ce628c535e87dc6077ba07d29068590c8cb5f1088e082cfa0ec961302d69cf3d44";
  const char* ciphertext_cstr = "dfb440acb3773549efc04628aeb8d8156275230bdc690d94b00d8d95f28c4b56307f60f4ca55eba661ebba"
                                "72ac808fa8c49e26788ed04a5d606cb418de74878b9a22f8ef29590bc4eb57c9faf7c41524a885b8979c42"
                                "3f2f8f8e0592a9879201be7ff9777a162ab810feb324ba74c4c156e04d39097209653ac33e5a5f2d8864";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 5
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset5)
{
  // Testdata in plain format
  const char* key_cstr  = "9618ae46891f86578eebe90ef7a1202e";
  uint32_t    count     = 0xc675a64b;
  uint8_t     bearer    = 0x0c;
  uint8_t     direction = 1;
  uint32_t    length    = 1245;
  const char* plaintext_cstr =
      "8daa17b1ae050529c6827f28c0ef6a1242e93f8b314fb18a77f790ae049fedd612267fecaefc450174d76d9f9aa7755a30cd90a9a5874bf4"
      "8eaf70eea3a62a250a8b6bd8d9b08b08d64e32d1817777fb544d49cd49720e219dbf8bbed33904e1fd40a41d370a1f65745095687d47ba1d"
      "36d2349e23f644392c8ea9c49d40c13271aff264d0f24841d6465f0996ff84e65fc517c53efc3363c38492a8";
  const char* ciphertext_cstr =
      "919c8c33d66789703d05a0d7ce82a2aeac4ee76c0f4da050335e8a84e7897ba5df2f36bd513e3d0c8578c7a0fcf043e03aa3a39fbaad7d15"
      "be074faa5d9029f71fb457b647834714b0e18f117fca10677945096c8c5f326ba8d6095eb29c3e36cf245d1622aafe921f7566c4f5d644f2"
      "f1fc0ec684ddb21349747622e209295d27ff3f95623371d49b147c0af486171f22cd04b1cbeb2658223e6938";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA2 Test Set 6
/// Ref: TS 33.501 Sec. D.4.4, TS 33.401 Sec. C.1 128-EEA2
TEST(security_nea2_test, testset6)
{
  // Testdata in plain format
  const char* key_cstr  = "54f4e2e04c83786eec8fb5abe8e36566";
  uint32_t    count     = 0xaca4f50f;
  uint8_t     bearer    = 0x0b;
  uint8_t     direction = 0;
  uint32_t    length    = 3861;
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

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea2(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA3 Test Set 1
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
TEST(security_nea3_test, testset1)
{
  // Testdata in plain format
  const char* key_cstr        = "173d14ba5003731d7a60049470f00a29";
  uint32_t    count           = 0x66035492;
  uint8_t     bearer          = 0xf;
  uint8_t     direction       = 0;
  uint32_t    length          = 193;
  const char* plaintext_cstr  = "6cf65340735552ab0c9752fa6f9025fe0bd675d9005875b200";
  const char* ciphertext_cstr = "a6c85fc66afb8533aafc2518dfe784940ee1e4b030238cc800";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea3(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA3 Test Set 2
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
TEST(security_nea3_test, testset2)
{
  // Testdata in plain format
  const char* key_cstr  = "e5bd3ea0eb55ade866c6ac58bd54302a";
  uint32_t    count     = 0x56823;
  uint8_t     bearer    = 0x18;
  uint8_t     direction = 1;
  uint32_t    length    = 800;
  const char* plaintext_cstr =
      "14a8ef693d678507bbe7270a7f67ff5006c3525b9807e467c4e56000ba338f5d429559036751822246c80d3b38f07f4be2d8ff5805f51322"
      "29bde93bbbdcaf382bf1ee972fbf9977bada8945847a2a6c9ad34a667554e04d1f7fa2c33241bd8f01ba220d";
  const char* ciphertext_cstr =
      "131d43e0dea1be5c5a1bfd971d852cbf712d7b4f57961fea3208afa8bca433f456ad09c7417e58bc69cf8866d1353f74865e80781d202dfb"
      "3ecff7fcbc3b190fe82a204ed0e350fc0f6f2613b2f2bca6df5a473a57a4a00d985ebad880d6f23864a07b01";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea3(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA3 Test Set 3
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
TEST(security_nea3_test, testset3)
{
  // Testdata in plain format
  const char* key_cstr  = "d4552a8fd6e61cc81a2009141a29c10b";
  uint32_t    count     = 0x76452ec1;
  uint8_t     bearer    = 0x2;
  uint8_t     direction = 1;
  uint32_t    length    = 1570;
  const char* plaintext_cstr =
      "38f07f4be2d8ff5805f5132229bde93bbbdcaf382bf1ee972fbf9977bada8945847a2a6c9ad34a667554e04d1f7fa2c33241bd8f01ba220d"
      "3ca4ec41e074595f54ae2b454fd971432043601965cca85c2417ed6cbec3bada84fc8a579aea7837b0271177242a64dc0a9de71a8edee86c"
      "a3d47d033d6bf539804eca86c584a9052de46ad3fced65543bd90207372b27afb79234f5ff43ea870820e2c2b78a8aae61cce52a0515e348"
      "d196664a3456b182a07c406e4a20791271cfeda165d535ec5ea2d4df40";
  const char* ciphertext_cstr =
      "8383b0229fcc0b9d2295ec41c977e9c2bb72e220378141f9c8318f3a270dfbcdee6411c2b3044f176dc6e00f8960f97afacd131ad6a3b49b"
      "16b7babcf2a509ebb16a75dcab14ff275dbeeea1a2b155f9d52c26452d0187c310a4ee55beaa78ab4024615ba9f5d5adc7728f73560671f0"
      "13e5e550085d3291df7d5fecedded559641b6c2f585233bc71e9602bd2305855bbd25ffa7f17ecbc042daae38c1f57ad8e8ebd37346f71be"
      "fdbb7432e0e0bb2cfc09bcd96570cb0c0c39df5e29294e82703a637f80";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea3(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA3 Test Set 4
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
TEST(security_nea3_test, testset4)
{
  // Testdata in plain format
  const char* key_cstr  = "db84b4fbccda563b66227bfe456f0f77";
  uint32_t    count     = 0xe4850fe1;
  uint8_t     bearer    = 0x10;
  uint8_t     direction = 1;
  uint32_t    length    = 2798;
  const char* plaintext_cstr =
      "e539f3b8973240da03f2b8aa05ee0a00dbafc0e182055dfe3d7383d92cef40e92928605d52d05f4f9018a1f189ae3997ce19155fb1221db8"
      "bb0951a853ad852ce16cff07382c93a157de00ddb125c7539fd85045e4ee07e0c43f9e9d6f414fc4d1c62917813f74c00fc83f3e2ed7c45b"
      "a5835264b43e0b20afda6b3053bfb6423b7fce25479ff5f139dd9b5b995558e2a56be18dd581cd017c735e6f0d0d97c4ddc1d1da70c6db4a"
      "12cc92778e2fbbd6f3ba52af91c9c6b64e8da4f7a2c266d02d001753df08960393c5d56888bf49eb5c16d9a80427a416bcb597df5bfe6f13"
      "890a07ee1340e6476b0d9aa8f822ab0fd1ab0d204f40b7ce6f2e136eb67485e507804d504588ad37ffd816568b2dc40311dfb654cdead47e"
      "2385c3436203dd836f9c64d97462ad5dfa63b5cfe08acb9532866f5ca787566fca93e6b1693ee15cf6f7a2d689d9741798dc1c238e1be650"
      "733b18fb34ff880e16bbd21b47ac";
  const char* ciphertext_cstr =
      "4bbfa91ba25d47db9a9f190d962a19ab323926b351fbd39e351e05da8b8925e30b1cce0d1221101095815cc7cb6319509ec0d67940491987"
      "e13f0affac332aa6aa64626d3e9a1917519e0b97b655c6a165e44ca9feac0790d2a321ad3d86b79c5138739fa38d887ec7def449ce8abdd3"
      "e7f8dc4ca9e7b73314ad310f9025e61946b3a56dc649ec0da0d63943dff592cf962a7efb2c8524e35a2a6e7879d62604ef268695fa400302"
      "7e22e6083077522064bd4a5b906b5f531274f235ed506cff0154c754928a0ce5476f2cb1020a1222d32c1455ecaef1e368fb344d1735bfbe"
      "deb71d0a33a2a54b1da5a294e679144ddf11eb1a3de8cf0cc061917974f35c1d9ca0ac81807f8fcce6199a6c7712da865021b04ce0439516"
      "f1a526ccda9fd9abbd53c3a684f9ae1e7ee6b11da138ea826c5516b5aadf1abbe36fa7fff92e3a1176064e8d95f2e4882b5500b93228b219"
      "4a475c1a27f63f9ffd264989a1bc";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea3(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}

/// 128-NEA3 Test Set 5
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
TEST(security_nea3_test, testset5)
{
  // Testdata in plain format
  const char* key_cstr  = "e13fed21b46e4e7ec31253b2bb17b3e0";
  uint32_t    count     = 0x2738cdaa;
  uint8_t     bearer    = 0x1a;
  uint8_t     direction = 0;
  uint32_t    length    = 4019;
  const char* plaintext_cstr =
      "8d74e20d54894e06d3cb13cb3933065e8674be62adb1c72b3a646965ab63cb7b7854dfdc27e84929f49c64b872a490b13f957b64827e71f4"
      "1fbd4269a42c97f824537027f86e9f4ad82d1df451690fdd98b6d03f3a0ebe3a312d6b840ba5a1820b2a2c9709c090d245ed267cf845ae41"
      "fa975d3333ac3009fd40eba9eb5b885714b768b697138baf21380eca49f644d48689e4215760b906739f0d2b3f091133ca15d981cbe401ba"
      "f72d05ace05cccb2d297f4ef6a5f58d91246cfa77215b892ab441d5278452795ccb7f5d79057a1c4f77f80d46db2033cb79bedf8e60551ce"
      "10c667f62a97abafabbcd6772018df96a282ea737ce2cb331211f60d5354ce78f9918d9c206ca042c9b62387dd709604a50af16d8d35a890"
      "6be484cf2e74a9289940364353249b27b4c9ae29eddfc7da6418791a4e7baa0660fa64511f2d685cc3a5ff70e0d2b74292e3b8a0cd6b04b1"
      "c790b8ead2703708540dea2fc09c3da770f65449e84d817a4f551055e19ab85018a0028b71a144d96791e9a3577933504eee0060340c69d2"
      "74e1bf9d805dcbcc1a6faa976800b6ff2b671dc463652fa8a33ee50974c1c21be01eabb2167430269d72ee511c9dde30797c9a25d86ce74f"
      "5b961be5fdfb6807814039e7137636bd1d7fa9e09efd2007505906a5ac45dfdeed7757bbee745749c29633350bee0ea6f409df45801600";
  const char* ciphertext_cstr =
      "94eaa4aa30a57137ddf09b97b25618a20a13e2f10fa5bf8161a879cc2ae797a6b4cf2d9df31debb9905ccfec97de605d21c61ab8531b7f3c"
      "9da5f03931f8a0642de48211f5f52ffea10f392a047669985da454a28f080961a6c2b62daa17f33cd60a4971f48d2d909394a55f48117ace"
      "43d708e6b77d3dc46d8bc017d4d1abb77b7428c042b06f2f99d8d07c9879d99600127a31985f1099bbd7d6c1519ede8f5eeb4a610b349ac0"
      "1ea2350691756bd105c974a53eddb35d1d4100b012e522ab41f4c5f2fde76b59cb8b96d885cfe4080d1328a0d636cc0edc05800b76acca8f"
      "ef672084d1f52a8bbd8e0993320992c7ffbae17c408441e0ee883fc8a8b05e22f5ff7f8d1b48c74c468c467a028f09fd7ce91109a570a2d5"
      "c4d5f4fa18c5dd3e4562afe24ef771901f59af645898acef088abae07e92d52eb2de55045bb1b7c4164ef2d7a6cac15eeb926d7ea2f08b66"
      "e1f759f3aee44614725aa3c7482b30844c143ff85b53f1e583c501257dddd096b81268daa303f17234c2333541f0bb8e190648c5807c866d"
      "7193228609adb948686f7de294a802cc38f7fe5208f5ea3196d0167b9bdd02f0d2a5221ca508f893af5c4b4bb9f4f520fd84289b3dbe7e61"
      "497a7e2a584037ea637b6981127174af57b471df4b2768fd79c1540fb3edf2ea22cb69bec0cf8d933d9c6fdd645e850591cca3d62c0cc0";

  // Pack hex strings into srsgnb types
  sec_128_as_key     key        = make_sec_128_as_key(key_cstr);
  security_direction dir        = static_cast<security_direction>(direction);
  byte_buffer        plaintext  = make_byte_buffer(plaintext_cstr);
  byte_buffer        ciphertext = make_byte_buffer(ciphertext_cstr);

  // Apply ciphering and compare results
  byte_buffer ciphertext_out = security_nea3(key, count, bearer, dir, plaintext, length);
  EXPECT_EQ(ciphertext_out, ciphertext);
}
