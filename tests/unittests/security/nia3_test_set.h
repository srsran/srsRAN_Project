/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "nia_test_set.h"
#include <vector>

/// 128-NIA3 Test Set
/// Test sets named with suffix "_mod" are modified custom versions of the test set expanded to next full byte.
/// Ref: TS 33.501 Sec. D.4.7, TS 35.223 (Document 3) Sec. 4,
/// ETSI TC SAGE "Specification of the 3GPP Confidentiality and Integrity Algorithms 128-EEA3 & 128-EIA3
/// Document 3: Implementor's Test Data - Sec. 5
std::vector<nia_test_set> nia3_test_set = {
    nia_test_set{
        .name         = "128_NIA3_Test_Set_1_mod",
        .count_i      = 0x0,
        .bearer       = 0x0,
        .direction    = 0,
        .ik_cstr      = "00000000000000000000000000000000",
        .length       = 8,
        .message_cstr = "00",
        .mact_cstr    = "390a91b7",
    },
    nia_test_set{
        .name         = "128_NIA3_Test_Set_2_mod",
        .count_i      = 0x561eb2dd,
        .bearer       = 0x14,
        .direction    = 0,
        .ik_cstr      = "47054125561eb2dda94059da05097850",
        .length       = 96,
        .message_cstr = "000000000000000000000000",
        .mact_cstr    = "89a58b47",
    },
    nia_test_set{
        .name         = "128_NIA3_Test_Set_3_mod",
        .count_i      = 0xa94059da,
        .bearer       = 0xa,
        .direction    = 1,
        .ik_cstr      = "c9e6cec4607c72db000aefa88385ab0a",
        .length       = 584,
        .message_cstr = "983b41d47d780c9e1ad11d7eb70391b1de0b35da2dc62f83e7b78d6306ca0ea07e941b7be91348f9fcb170e2217fec"
                        "d97f9f68adb16e5d7d21e569d280ed775cebde3f4093c5388100",
        .mact_cstr    = "24a842b3",
    },
    nia_test_set{
        .name      = "128_NIA3_Test_Set_4_mod",
        .count_i   = 0x05097850,
        .bearer    = 0x10,
        .direction = 1,
        .ik_cstr   = "c8a48262d0c2e2bac4b96ef77e80ca59",
        .length    = 2080,
        .message_cstr =
            "b546430bf87b4f1ee834704cd6951c36e26f108cf731788f48dc34f1678c05221c8fa7ff2f39f477e7e49ef60a4ec2c3de24312a96"
            "aa26e1cfba57563838b297f47e8510c779fd6654b143386fa639d31edbd6c06e47d159d94362f26aeeedee0e4f49d9bf8412995415"
            "bfad56ee82d1ca7463abf085b082b09904d6d990d43cf2e062f40839d93248b1eb92cdfed5300bc148280430b6d0caa094b6ec8911"
            "ab7dc36824b824dc0af6682b0935fde7b492a14dc2f43648038da2cf79170d2d50133fd49416cb6e33bea90b8bf4559b03732a01ea"
            "290e6d074f79bb83c10e580015cc1a85b36b5501046e9c4bdcae5135690b8666bd54b7a703ea7b6f220a5469a568027e",
        .mact_cstr = "039532e1",
    },
    nia_test_set{
        .name      = "128_NIA3_Test_Set_5_mod",
        .count_i   = 0x561eb2dd,
        .bearer    = 0x1c,
        .direction = 0,
        .ik_cstr   = "6b8b08ee79e0b5982d6d128ea9f220cb",
        .length    = 5672,
        .message_cstr =
            "5bad724710ba1c56d5a315f8d40f6e093780be8e8de07b6992432018e08ed96a5734af8bad8a575d3a1f162f85045cc770925571d9"
            "f5b94e454a77c16e72936bf016ae157499f0543b5d52caa6dbeab697d2bb73e41b8075dce79b4b86044f661d4485a543dd78606e04"
            "19e8059859d3cb2b67ce0977603f81ff839e331859544cfbc8d00fef1a4c8510fb547d6b06c611ef44f1bce107cfa45a06aab36015"
            "2b28dc1ebe6f7fe09b0516f9a5b02a1bd84bb0181e2e89e19bd8125930d178682f3862dc51b636f04e720c47c3ce51ad70d94b9b22"
            "55fbae906549f499f8c6d39947ed5e5df8e2def113253e7b08d0a76b6bfc68c812f375c79b8fe5fd85976aa6d46b4a2339d8ae5147"
            "f680fbe70f978b38effd7b2f7866a22554e193a94e98a68b74bd25bb2b3f5fb0a5fd59887f9ab68159b7178d5b7b677cb546bf41ea"
            "dca216fc10850128f8bdef5c8d89f96afa4fa8b54885565ed838a950fee5f1c3b0a4f6fb71e54dfd169e82cecc7266c850e67c5ef0"
            "ba960f5214060e71eb172a75fc1486835cbea6534465b055c96a72e4105224182325d830414b40214daa8091d2e0fb010ae15c6de9"
            "0850973bdf1e423be148a237b87a0c9f34d4b47605b803d743a86a90399a4af396d3a1200a62f3d9507962e8e5bee6d3da2bb3f723"
            "7664ac7a292823900bc63503b29e80d63f6067bf8e1716ac25beba350deb62a99fe03185eb4f69937ecd387941fda544ba67db0911"
            "774938b01827bcc69c92b3f772a9d2859ef003398b1f6bbad7b574f7989a1d10b2df798e0dbf30d6587464d24878cd00c0eaee8a1a"
            "0cc753a27979e11b41db1de3d5038afaf49f5c682c3748d8a3a9ec54e6a371275f1683510f8e4f90938f9ab6e134c2cfdf4841cba8"
            "8e0cff2b0bcc8e6adcb71109b5198fecf1bb7e5c531aca50a56a8a3b6de59862d41fa113d9cd957808f08571d9a4bb792af271f6cc"
            "6dbb8dc7ec36e36be1ed308164c31c7c0afc541c",
        .mact_cstr = "fb9ab74c",
    },
};
