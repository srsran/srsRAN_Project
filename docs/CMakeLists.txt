find_package(Doxygen OPTIONAL_COMPONENTS dot)

if (DOXYGEN_FOUND)
  # Configure doxygen
  set(DOXYGEN_PROJECT_NUMBER ${VERSION})
  set(DOXYGEN_PROJECT_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/srsRan_logo.png)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set(DOXYGEN_WARN_LOGFILE ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log)
  set(DOXYGEN_USE_MATHJAX "YES")
  set(DOXYGEN_MATHJAX_EXTENSIONS "TeX/AMSmath")
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
  find_program(PLANT_UML_PATH plantuml.jar PATH_SUFFIXES PlantUML plantuml Plantuml
          PATHS /usr/share /usr/local/share/ /usr/local/bin /opt/local/share/java/)
  if (PLANT_UML_PATH)
    message(STATUS "PLANTUML found.")
  endif ()
  set(DOXYGEN_PLANTUML_JAR_PATH "/home/xico/tmp/plantuml.jar")

  # directories and files to process
  set(docs_inputs ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_HOME_DIRECTORY}/include/srsgnb/
                  ${CMAKE_HOME_DIRECTORY}/lib/
                  ${CMAKE_HOME_DIRECTORY}/unittests/phy/upper/channel_coding/ldpc_enc_dec_test.cpp
  )
  set(DOXYGEN_EXCLUDE ${CMAKE_HOME_DIRECTORY}/include/srsgnb/asn1)

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
