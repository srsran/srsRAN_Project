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

/// 128-NEA1 Test Set
/// Ref: TS 33.501 Sec. D.4.2, TS 35.217 Sec. 4,
/// 3GPP Confidentiality and Integrity Algorithms UEA2&UIA2
/// Document 4: Design Conformance Test Data - Sec. 3.3.
std::vector<nea_test_set> nea1_test_set = {
    nea_test_set{
        .name            = "128_NEA1_Test_Set_1",
        .key_cstr        = "D3C5D592327FB11C4035C6680AF8C6D1",
        .count           = 0x398a59b4,
        .bearer          = 0x15,
        .direction       = 1,
        .length          = 253,
        .plaintext_cstr  = "981BA6824C1BFB1AB485472029B71D808CE33E2CC3C0B5FC1F3DE8A6DC66B1F0",
        .ciphertext_cstr = "5D5BFE75EB04F68CE0A12377EA00B37D47C6A0BA06309155086A859C4341B378",
    },
    nea_test_set{
        .name           = "128_NEA1_Test_Set_2",
        .key_cstr       = "2BD6459F82C440E0952C49104805FF48",
        .count          = 0xc675a64b,
        .bearer         = 0x0c,
        .direction      = 1,
        .length         = 798,
        .plaintext_cstr = "7EC61272743BF1614726446A6C38CED166F6CA76EB5430044286346CEF130F92922B03450D3A9975E5BD2EA0EB55"
                          "AD8E1B199E3EC4316020E9A1B285E762795359B7BDFD39BEF4B2484583D5AFE082AEE638BF5FD5A606193901A08F"
                          "4AB41AAB9B134880",
        .ciphertext_cstr = "3F67850714B8DA69EFB727ED7A6C0C50714AD736C4F5600006E3525BE807C467C677FF864AF45FBA09C27CDE38F"
                           "87A1F84D59AB255408F2C7B82F9EAD41A1FE65EABEBFBC1F3A4C56C9A26FCF7B3D66D0220EE4775BC58170A2B12"
                           "F3431D11B344D6E36C",
    },
    nea_test_set{
        .name            = "128_NEA1_Test_Set_3",
        .key_cstr        = "0A8B6BD8D9B08B08D64E32D1817777FB",
        .count           = 0x544d49cd,
        .bearer          = 0x04,
        .direction       = 0,
        .length          = 310,
        .plaintext_cstr  = "FD40A41D370A1F65745095687D47BA1D36D2349E23F644392C8EA9C49D40C13271AFF264D0F248",
        .ciphertext_cstr = "48148E5452A210C05F46BC80DC6F73495B02048C1B958B026102CA97280279A4C18D2EE308921C",
    },
    nea_test_set{
        .name           = "128_NEA1_Test_Set_4",
        .key_cstr       = "AA1F95AEA533BCB32EB63BF52D8F831A",
        .count          = 0x72d8c671,
        .bearer         = 0x10,
        .direction      = 1,
        .length         = 1022,
        .plaintext_cstr = "FB1B96C5C8BADFB2E8E8EDFDE78E57F2AD81E74103FC430A534DCC37AFCEC70E1517BB06F27219DAE49022DDC47A"
                          "068DE4C9496A951A6B09EDBDC864C7ADBD740AC50C022F3082BAFD22D78197C5D508B977BCA13F32E652E74BA728"
                          "576077CE628C535E87DC6077BA07D29068590C8CB5F1088E082CFA0EC961302D69CF3D44",
        .ciphertext_cstr = "FFCFC2FEAD6C094E96C589D0F6779B6784246C3C4D1CEA203DB3901F40AD4FD7138BC6D77E8320CB102F497FDD4"
                           "4A269A96ECB28617700E332EB2F736B34F4F2693094E22FF94F9BE4723DA40C40DFD3931CC1AC9723F6B4A9913E"
                           "96B6DB7ABCACE415177C1D0115C5F09B5FDEA0B3ADB8F9DA6E9F9A04C543397B9D43F87330",
    },
    nea_test_set{
        .name      = "128_NEA1_Test_Set_5",
        .key_cstr  = "9618AE46891F86578EEBE90EF7A1202E",
        .count     = 0xc675a64b,
        .bearer    = 0x0c,
        .direction = 1,
        .length    = 1245,
        .plaintext_cstr =
            "8DAA17B1AE050529C6827F28C0EF6A1242E93F8B314FB18A77F790AE049FEDD612267FECAEFC450174D76D9F9AA7755A30CD90A9A5"
            "874BF48EAF70EEA3A62A250A8B6BD8D9B08B08D64E32D1817777FB544D49CD49720E219DBF8BBED33904E1FD40A41D370A1F657450"
            "95687D47BA1D36D2349E23F644392C8EA9C49D40C13271AFF264D0F24841D6465F0996FF84E65FC517C53EFC3363C38492A8",
        .ciphertext_cstr =
            "6CDB18A7CA8218E86E4B4B716A4D04371FBEC262FC5AD0B3819B187B97E55B1A4D7C19EE24C8B4D7723CFEDF045B8ACAE4869517D8"
            "0E50615D9035D5D9C5A40AF602280B542597B0CB18619EEB35925759D195E100E8E4AA0C38A3C2ABE0F3D8FF04F3C33C295069C236"
            "94B5BBEACDD542E28E8A94EDB9119F412D054BE1FA7272B5FFB2B2570F4F7CEAF383A8A9D93572F04D6E3A6E293726EC62C8",
    },
    nea_test_set{
        .name      = "128_NEA1_Test_Set_6",
        .key_cstr  = "54F4E2E04C83786EEC8FB5ABE8E36566",
        .count     = 0xaca4f50f,
        .bearer    = 0x0b,
        .direction = 0,
        .length    = 3861,
        .plaintext_cstr =
            "40981BA6824C1BFB4286B299783DAF442C099F7AB0F58D5C8E46B104F08F01B41AB485472029B71D36BD1A3D90DC3A41B46D51672A"
            "C4C9663A2BE063DA4BC8D2808CE33E2CCCBFC634E1B259060876A0FBB5A437EBCC8D31C19E4454318745E3FA16BB11ADAE248879FE"
            "52DB2543E53CF445D3D828CE0BF5C560593D97278A59762DD0C2C9CD68D4496A792508614014B13B6AA51128C18CD6A90B87978C2F"
            "F1CABE7D9F898A411BFDB84F68F6727B1499CDD30DF0443AB4A66653330BCBA1105E4CEC034C73E605B4310EAAADCFD5B0CA27FFD8"
            "9D144DF4792759427C9CC1F8CD8C87202364B8A687954CB05A8D4E2D99E73DB160DEB180AD0841E96741A5D59FE4189F15420026FE"
            "4CD12104932FB38F735340438AAF7ECA6FD5CFD3A195CE5ABE65272AF607ADA1BE65A6B4C9C0693234092C4D018F1756C6DB9DC8A6"
            "D80B888138616B681262F954D0E7711748780D92291D86299972DB741CFA4F37B8B56CDB18A7CA8218E86E4B4B716A4D04371FBEC2"
            "62FC5AD0B3819B187B97E55B1A4D7C19EE24C8B4D7723CFEDF045B8ACAE4869517D80E50615D9035D5D9C5A40AF602280B542597B0"
            "CB18619EEB35925759D195E100E8E4AA0C38A3C2ABE0F3D8FF04F3C33C295069C23694B5BBEACDD542E28E8A94EDB9119F412D054B"
            "E1FA72B09550",
        .ciphertext_cstr =
            "351E30D4D910C5DD5AD7834C426E6C0CAB6486DA7B0FDA4CD83AF1B9647137F1AC43B434223B19BE07BD89D1CC306944D3361EA1A2"
            "F8CDBD321655976350D00B80DD838120A7755C6DEA2AB2B0C99A913F47DAE2B8DEB9A829E5469FF2E187776F6FD081E3871D119A76"
            "E24C917EA62648E02E90367564DE72AE7E4F0A4249A9A5B0E465A2D6D9DC87843B1B875CC9A3BE93D8DA8F56ECAF5981FE93C28431"
            "8B0DEC7A3BA108E2CB1A61E966FA7AFA7AC7F67F65BC4A2DF070D4E434845F109AB2B68ADE3DC316CA6332A62893E0A7EC0B4FC251"
            "91BF2FF1B9F9815E4BA8A99C643B521804F7D5850DDE3952206EC6CCF340F9B3220B3023BDD063956EA8333920FDE99E0675410E49"
            "EF3B4D3FB3DF5192F99CA83D3B0032DE08C220776A5865B0E4B3B0C75DEFE7762DFF018EA7F5BE2B2F972B2A8BA5970E43BD6FDD63"
            "DAE629784EC48D610054EE4E4B5DBBF1FC2FA0B830E94DCBB7014E8AB429AB100FC48F83171D99FC258B7C2BA7C176EAEAAD37F860"
            "D597A31CE79B594733C7141DF79151FCA90C08478A5C6C2CC481D51FFECE3CD7D2581348827A71F091428EBE38C95A3F5C63E056DF"
            "B7CC45A9B7C07D834E7B20B99ED202429C14BB85FFA43B7CB68495CD75AB66D964D4CAFE64DD9404DAE2DC5110617F194FC3C184F5"
            "83CD0DEF6D00",
    },
};

/// 128-NEA1 Test Set Extra
/// This set contains additional custom test cases
std::vector<nea_test_set> nea1_test_set_extra = {
    nea_test_set{
        .name            = "128_NEA1_Test_Set_Extra_Len_0",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 0,
        .plaintext_cstr  = "",
        .ciphertext_cstr = "",
    },
    nea_test_set{
        .name            = "128_NEA1_Test_Set_Extra_Len_1",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 8,
        .plaintext_cstr  = "AA",
        .ciphertext_cstr = "16",
    },
    nea_test_set{
        .name            = "128_NEA1_Test_Set_Extra_Len_2",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 16,
        .plaintext_cstr  = "AABB",
        .ciphertext_cstr = "1674",
    },
    nea_test_set{
        .name            = "128_NEA1_Test_Set_Extra_Len_4",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 32,
        .plaintext_cstr  = "AABBCCDD",
        .ciphertext_cstr = "167451BC",
    },
    nea_test_set{
        .name            = "128_NEA1_Test_Set_Extra_Len_8",
        .key_cstr        = "00112233445566778899AABBCCDDEEFF",
        .count           = 0x55555555,
        .bearer          = 0x11,
        .direction       = 1,
        .length          = 64,
        .plaintext_cstr  = "AABBCCDDEEFF0011",
        .ciphertext_cstr = "167451BC2F3F1923",
    },
};
