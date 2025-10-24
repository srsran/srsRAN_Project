/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "nea_test_set.h"
#include <vector>

/// 128-NEA3 Test Set
/// Ref: TS 33.501 Sec. D.4.6, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 4
std::vector<nea_test_set> nea3_test_set = {
    nea_test_set{
        .name            = "128_NEA3_Test_Set_1",
        .key_cstr        = "173d14ba5003731d7a60049470f00a29",
        .count           = 0x66035492,
        .bearer          = 0xf,
        .direction       = 0,
        .length          = 193,
        .plaintext_cstr  = "6cf65340735552ab0c9752fa6f9025fe0bd675d9005875b200",
        .ciphertext_cstr = "a6c85fc66afb8533aafc2518dfe784940ee1e4b030238cc800",
    },
    nea_test_set{
        .name      = "128_NEA3_Test_Set_2",
        .key_cstr  = "e5bd3ea0eb55ade866c6ac58bd54302a",
        .count     = 0x56823,
        .bearer    = 0x18,
        .direction = 1,
        .length    = 800,
        .plaintext_cstr =
            "14a8ef693d678507bbe7270a7f67ff5006c3525b9807e467c4e56000ba338f5d429559036751822246c80d3b38f07f4be2d8ff5805"
            "f5132229bde93bbbdcaf382bf1ee972fbf9977bada8945847a2a6c9ad34a667554e04d1f7fa2c33241bd8f01ba220d",
        .ciphertext_cstr =
            "131d43e0dea1be5c5a1bfd971d852cbf712d7b4f57961fea3208afa8bca433f456ad09c7417e58bc69cf8866d1353f74865e80781d"
            "202dfb3ecff7fcbc3b190fe82a204ed0e350fc0f6f2613b2f2bca6df5a473a57a4a00d985ebad880d6f23864a07b01",
    },
    nea_test_set{
        .name      = "128_NEA3_Test_Set_3",
        .key_cstr  = "d4552a8fd6e61cc81a2009141a29c10b",
        .count     = 0x76452ec1,
        .bearer    = 0x2,
        .direction = 1,
        .length    = 1570,
        .plaintext_cstr =
            "38f07f4be2d8ff5805f5132229bde93bbbdcaf382bf1ee972fbf9977bada8945847a2a6c9ad34a667554e04d1f7fa2c33241bd8f01"
            "ba220d3ca4ec41e074595f54ae2b454fd971432043601965cca85c2417ed6cbec3bada84fc8a579aea7837b0271177242a64dc0a9d"
            "e71a8edee86ca3d47d033d6bf539804eca86c584a9052de46ad3fced65543bd90207372b27afb79234f5ff43ea870820e2c2b78a8a"
            "ae61cce52a0515e348d196664a3456b182a07c406e4a20791271cfeda165d535ec5ea2d4df40",
        .ciphertext_cstr =
            "8383b0229fcc0b9d2295ec41c977e9c2bb72e220378141f9c8318f3a270dfbcdee6411c2b3044f176dc6e00f8960f97afacd131ad6"
            "a3b49b16b7babcf2a509ebb16a75dcab14ff275dbeeea1a2b155f9d52c26452d0187c310a4ee55beaa78ab4024615ba9f5d5adc772"
            "8f73560671f013e5e550085d3291df7d5fecedded559641b6c2f585233bc71e9602bd2305855bbd25ffa7f17ecbc042daae38c1f57"
            "ad8e8ebd37346f71befdbb7432e0e0bb2cfc09bcd96570cb0c0c39df5e29294e82703a637f80",
    },
    nea_test_set{
        .name      = "128_NEA3_Test_Set_4",
        .key_cstr  = "db84b4fbccda563b66227bfe456f0f77",
        .count     = 0xe4850fe1,
        .bearer    = 0x10,
        .direction = 1,
        .length    = 2798,
        .plaintext_cstr =
            "e539f3b8973240da03f2b8aa05ee0a00dbafc0e182055dfe3d7383d92cef40e92928605d52d05f4f9018a1f189ae3997ce19155fb1"
            "221db8bb0951a853ad852ce16cff07382c93a157de00ddb125c7539fd85045e4ee07e0c43f9e9d6f414fc4d1c62917813f74c00fc8"
            "3f3e2ed7c45ba5835264b43e0b20afda6b3053bfb6423b7fce25479ff5f139dd9b5b995558e2a56be18dd581cd017c735e6f0d0d97"
            "c4ddc1d1da70c6db4a12cc92778e2fbbd6f3ba52af91c9c6b64e8da4f7a2c266d02d001753df08960393c5d56888bf49eb5c16d9a8"
            "0427a416bcb597df5bfe6f13890a07ee1340e6476b0d9aa8f822ab0fd1ab0d204f40b7ce6f2e136eb67485e507804d504588ad37ff"
            "d816568b2dc40311dfb654cdead47e2385c3436203dd836f9c64d97462ad5dfa63b5cfe08acb9532866f5ca787566fca93e6b1693e"
            "e15cf6f7a2d689d9741798dc1c238e1be650733b18fb34ff880e16bbd21b47ac",
        .ciphertext_cstr =
            "4bbfa91ba25d47db9a9f190d962a19ab323926b351fbd39e351e05da8b8925e30b1cce0d1221101095815cc7cb6319509ec0d67940"
            "491987e13f0affac332aa6aa64626d3e9a1917519e0b97b655c6a165e44ca9feac0790d2a321ad3d86b79c5138739fa38d887ec7de"
            "f449ce8abdd3e7f8dc4ca9e7b73314ad310f9025e61946b3a56dc649ec0da0d63943dff592cf962a7efb2c8524e35a2a6e7879d626"
            "04ef268695fa4003027e22e6083077522064bd4a5b906b5f531274f235ed506cff0154c754928a0ce5476f2cb1020a1222d32c1455"
            "ecaef1e368fb344d1735bfbedeb71d0a33a2a54b1da5a294e679144ddf11eb1a3de8cf0cc061917974f35c1d9ca0ac81807f8fcce6"
            "199a6c7712da865021b04ce0439516f1a526ccda9fd9abbd53c3a684f9ae1e7ee6b11da138ea826c5516b5aadf1abbe36fa7fff92e"
            "3a1176064e8d95f2e4882b5500b93228b2194a475c1a27f63f9ffd264989a1bc",
    },
    nea_test_set{
        .name      = "128_NEA3_Test_Set_5",
        .key_cstr  = "e13fed21b46e4e7ec31253b2bb17b3e0",
        .count     = 0x2738cdaa,
        .bearer    = 0x1a,
        .direction = 0,
        .length    = 4019,
        .plaintext_cstr =
            "8d74e20d54894e06d3cb13cb3933065e8674be62adb1c72b3a646965ab63cb7b7854dfdc27e84929f49c64b872a490b13f957b6482"
            "7e71f41fbd4269a42c97f824537027f86e9f4ad82d1df451690fdd98b6d03f3a0ebe3a312d6b840ba5a1820b2a2c9709c090d245ed"
            "267cf845ae41fa975d3333ac3009fd40eba9eb5b885714b768b697138baf21380eca49f644d48689e4215760b906739f0d2b3f0911"
            "33ca15d981cbe401baf72d05ace05cccb2d297f4ef6a5f58d91246cfa77215b892ab441d5278452795ccb7f5d79057a1c4f77f80d4"
            "6db2033cb79bedf8e60551ce10c667f62a97abafabbcd6772018df96a282ea737ce2cb331211f60d5354ce78f9918d9c206ca042c9"
            "b62387dd709604a50af16d8d35a8906be484cf2e74a9289940364353249b27b4c9ae29eddfc7da6418791a4e7baa0660fa64511f2d"
            "685cc3a5ff70e0d2b74292e3b8a0cd6b04b1c790b8ead2703708540dea2fc09c3da770f65449e84d817a4f551055e19ab85018a002"
            "8b71a144d96791e9a3577933504eee0060340c69d274e1bf9d805dcbcc1a6faa976800b6ff2b671dc463652fa8a33ee50974c1c21b"
            "e01eabb2167430269d72ee511c9dde30797c9a25d86ce74f5b961be5fdfb6807814039e7137636bd1d7fa9e09efd2007505906a5ac"
            "45dfdeed7757bbee745749c29633350bee0ea6f409df45801600",
        .ciphertext_cstr =
            "94eaa4aa30a57137ddf09b97b25618a20a13e2f10fa5bf8161a879cc2ae797a6b4cf2d9df31debb9905ccfec97de605d21c61ab853"
            "1b7f3c9da5f03931f8a0642de48211f5f52ffea10f392a047669985da454a28f080961a6c2b62daa17f33cd60a4971f48d2d909394"
            "a55f48117ace43d708e6b77d3dc46d8bc017d4d1abb77b7428c042b06f2f99d8d07c9879d99600127a31985f1099bbd7d6c1519ede"
            "8f5eeb4a610b349ac01ea2350691756bd105c974a53eddb35d1d4100b012e522ab41f4c5f2fde76b59cb8b96d885cfe4080d1328a0"
            "d636cc0edc05800b76acca8fef672084d1f52a8bbd8e0993320992c7ffbae17c408441e0ee883fc8a8b05e22f5ff7f8d1b48c74c46"
            "8c467a028f09fd7ce91109a570a2d5c4d5f4fa18c5dd3e4562afe24ef771901f59af645898acef088abae07e92d52eb2de55045bb1"
            "b7c4164ef2d7a6cac15eeb926d7ea2f08b66e1f759f3aee44614725aa3c7482b30844c143ff85b53f1e583c501257dddd096b81268"
            "daa303f17234c2333541f0bb8e190648c5807c866d7193228609adb948686f7de294a802cc38f7fe5208f5ea3196d0167b9bdd02f0"
            "d2a5221ca508f893af5c4b4bb9f4f520fd84289b3dbe7e61497a7e2a584037ea637b6981127174af57b471df4b2768fd79c1540fb3"
            "edf2ea22cb69bec0cf8d933d9c6fdd645e850591cca3d62c0cc0",
    },
};

/// 128-NEA3 Test Set Extra
/// This set contains additional custom test cases
std::vector<nea_test_set> nea3_test_set_extra = {
    nea_test_set{
        .name            = "128_NEA3_Test_Set_Extra_Len_0",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 0,
        .plaintext_cstr  = "",
        .ciphertext_cstr = "",
    },
    nea_test_set{
        .name            = "128_NEA3_Test_Set_Extra_Len_1",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 8,
        .plaintext_cstr  = "AA",
        .ciphertext_cstr = "47",
    },
    nea_test_set{
        .name            = "128_NEA3_Test_Set_Extra_Len_2",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 16,
        .plaintext_cstr  = "AABB",
        .ciphertext_cstr = "47E0",
    },
    nea_test_set{
        .name            = "128_NEA3_Test_Set_Extra_Len_4",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 32,
        .plaintext_cstr  = "AABBCCDD",
        .ciphertext_cstr = "47E02465",
    },
    nea_test_set{
        .name            = "128_NEA3_Test_Set_Extra_Len_8",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 64,
        .plaintext_cstr  = "AABBCCDDEEFF0011",
        .ciphertext_cstr = "47E02465BD84ED8A",
    },
};
