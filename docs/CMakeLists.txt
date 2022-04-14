find_package(Doxygen OPTIONAL_COMPONENTS dot)

if (DOXYGEN_FOUND)
  # Configure doxygen
  set(DOXYGEN_PROJECT_NUMBER ${VERSION})
  set(DOXYGEN_PROJECT_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/srsRan_logo.png)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set(DOXYGEN_WARN_LOGFILE ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log)
  set(DOXYGEN_USE_MATHJAX "YES")
  set(DOXYGEN_MATHJAX_EXTENSIONS "TeX/AMSmath")
  set(DOXYGEN_FORMULA_MACROFILE ${CMAKE_CURRENT_SOURCE_DIR}/math_macros.tex)
  set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_HOME_DIRECTORY})
  set(DOXYGEN_EXTRACT_STATIC "YES")
  set(DOXYGEN_EXTRACT_PRIVATE "YES")
  set(DOXYGEN_GENERATE_TREEVIEW "YES")
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome.css)
    set(DOXYGEN_GENERATE_TREEVIEW "YES")
    set(DOXYGEN_HTML_EXTRA_STYLESHEET "doxygen-awesome.css")
  endif()
  set(DOXYGEN_UML_LOOK "YES")
  set(DOXYGEN_CALL_GRAPH "YES")
  set(DOXYGEN_CALLER_GRAPH "YES")

  # find plantuml
  find_program(PLANTUML_PATH plantuml.jar PATH_SUFFIXES PlantUML plantuml Plantuml
          PATHS /usr/share /usr/local/share /usr/local/bin /opt/local/share/java)
  if (PLANTUML_PATH)
    message(STATUS "PLANTUML found in ${PLANTUML_PATH}.")
  endif ()
  set(DOXYGEN_PLANTUML_JAR_PATH "${PLANTUML_PATH}")

  # directories and files to process for all the docs
  set(docs_inputs ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_HOME_DIRECTORY}/include/srsgnb
                  ${CMAKE_HOME_DIRECTORY}/lib
  )
  set(DOXYGEN_EXCLUDE ${CMAKE_HOME_DIRECTORY}/include/srsgnb/asn1)

  doxygen_add_docs(doxygen
    ${docs_inputs}
    COMMENT "Generating all documentation."
  )

  # directories and files to process for FAPI related docs
  set(docs_inputs_fapi ${CMAKE_CURRENT_SOURCE_DIR}
                       ${CMAKE_HOME_DIRECTORY}/include/srsgnb/fapi
                       ${CMAKE_HOME_DIRECTORY}/include/srsgnb/fapi_adaptor
                       ${CMAKE_HOME_DIRECTORY}/lib/fapi
                       ${CMAKE_HOME_DIRECTORY}/lib/fapi_adaptor
  )

  doxygen_add_docs(doxygen-fapi
    ${docs_inputs_fapi}
    COMMENT "Generating FAPI-related documentation."
  )

  # directories and files to process for MAC+RLC docs
  set(docs_inputs_mac ${CMAKE_CURRENT_SOURCE_DIR}
                      ${CMAKE_HOME_DIRECTORY}/include/srsgnb/mac
                      ${CMAKE_HOME_DIRECTORY}/include/srsgnb/rlc
                      ${CMAKE_HOME_DIRECTORY}/lib/mac
                      ${CMAKE_HOME_DIRECTORY}/lib/rlc
  )

  doxygen_add_docs(doxygen-mac
    ${docs_inputs_mac}
    COMMENT "Generating documentation for layers MAC and RLC."
  )

  # directories and files to process for PHY+RAN docs
  set(docs_inputs_phy ${CMAKE_CURRENT_SOURCE_DIR}
                      ${CMAKE_HOME_DIRECTORY}/include/srsgnb/phy
                      ${CMAKE_HOME_DIRECTORY}/include/srsgnb/ran
                      ${CMAKE_HOME_DIRECTORY}/include/srsgnb/radio
                      ${CMAKE_HOME_DIRECTORY}/lib/phy
                      ${CMAKE_HOME_DIRECTORY}/lib/ran
                      ${CMAKE_HOME_DIRECTORY}/lib/radio
  )

  doxygen_add_docs(doxygen-phy
    ${docs_inputs_phy}
    COMMENT "Generating documentation for layers PHY and radio."
  )

  add_custom_target(doxygen-clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/html
                                       ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log
    COMMENT "Cleaning documentation"
    VERBATIM
  )
endif(DOXYGEN_FOUND)
