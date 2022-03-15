find_package(Doxygen OPTIONAL_COMPONENTS dot)

if (DOXYGEN_FOUND)
  # Configure doxygen
  set(DOXYGEN_PROJECT_NUMBER ${VERSION})
  set(DOXYGEN_PROJECT_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/srsRan_logo.png)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set(DOXYGEN_WARN_LOGFILE ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log)
  set(DOXYGEN_USE_MATHJAX "YES")
  set(DOXYGEN_MATHJAX_EXTENSIONS "TeX/AMSmath")
  if (${CMAKE_VERSION} VERSION_LESS 3.20)
    get_filename_component(strip_path ${CMAKE_HOME_DIRECTORY} DIRECTORY)
  else()
    cmake_path(GET CMAKE_HOME_DIRECTORY PARENT_PATH strip_path)
  endif()
  set(DOXYGEN_STRIP_FROM_PATH ${strip_path})
  set(DOXYGEN_MACRO_EXPANSION "YES")
  set(DOXYGEN_EXPAND_ONLY_PREDEF "YES")
  set(DOXYGEN_EXPAND_AS_DEFINED "SRSRAN_API")
  set(DOXYGEN_PREDEFINED "SRSRAN_API=''")
  set(DOXYGEN_EXTRACT_STATIC "YES")
  set(DOXYGEN_EXTRACT_PRIVATE "YES")

  # directories and files to process
  set(docs_inputs ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_HOME_DIRECTORY}/include/srsgnb/phy
                  ${CMAKE_HOME_DIRECTORY}/lib/phy
                  ${CMAKE_HOME_DIRECTORY}/unittests/phy/upper/channel_coding/ldpc_enc_dec_test.cpp
  )

  doxygen_add_docs(doxygen
    ${docs_inputs}
    COMMENT "Generating documentation"
  )

  add_custom_target(doxygen-clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/html
                                       ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log
    COMMENT "Cleaning documentation"
    VERBATIM
  )
endif(DOXYGEN_FOUND)
