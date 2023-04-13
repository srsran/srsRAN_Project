#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# There is a compilation defect for CLang 15.0.0 which makes the pseudo-random generator to create invalid sequences.
if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    include(CheckCSourceRuns)

    # Check if the compiler supports the PRN generator with -O2 flag.
    set(CMAKE_REQUIRED_FLAGS "-O2")
    check_c_source_runs("
int prn(unsigned in) { return in * 123 + 1234; }

void apply_xor(char* out, const char* in, unsigned x1, unsigned x2,
               unsigned length) {
    unsigned bytes_per_word = 24;

    unsigned i = 0;

    for (unsigned i_end = (length / bytes_per_word) * bytes_per_word;
         i != i_end; i += bytes_per_word) {
            unsigned mask = x1 ^ x2;

        for (unsigned j = 0; j != bytes_per_word; ++j) {
            out[i + j] = in[i + j] ^ ((mask >> j) & 1U);
        }

        x1 = prn(x1);
        x2 = prn(x2);
    }

    unsigned mask = x1 ^ x2;
   for (; i != length; ++i) {
        out[i] = in[i] ^ (mask & 1U);
        mask = mask >> 1U;
    }
}

int main() {
    char in[256] = {};
    char out[256] = {};
    apply_xor(out, in, 0xdeadbeef, 0xbeeffeed, 256);

    unsigned sum = 0;
    for (unsigned i = 0; i != 256; ++i) {
        sum += out[i];
    }
    return (sum == 118) ? 0 : -1;
}"
            PRN_SUPPORTED_COMPILER)

    # If the compiler with -O2 is not supported, warn the user about the defect and replace the -O2 flag by a -O3 flag
    # for this directory.
    if (NOT PRN_SUPPORTED_COMPILER)
        message(WARNING "Unsupported compiler version.")
        string(REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    endif ()
endif ()

set(SOURCES
        low_papr_sequence_collection_impl.cpp
        low_papr_sequence_generator_impl.cpp
        pseudo_random_generator_impl.cpp
        sequence_generator_factories.cpp
        )

add_library(srsran_sequence_generators STATIC ${SOURCES})
target_link_libraries(srsran_sequence_generators srsran_support srsvec)

install(TARGETS srsran_sequence_generators
        EXPORT srsran_export)