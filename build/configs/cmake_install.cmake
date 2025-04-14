# Install script for directory: /home/shovon/srsRAN_Project/configs

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/srsran" TYPE FILE FILES
    "/home/shovon/srsRAN_Project/configs/amf.yml"
    "/home/shovon/srsRAN_Project/configs/cell_cfg_max_128_ues.yml"
    "/home/shovon/srsRAN_Project/configs/cell_cfg_max_256_ues.yml"
    "/home/shovon/srsRAN_Project/configs/cell_cfg_max_32_ues.yml"
    "/home/shovon/srsRAN_Project/configs/cell_cfg_max_512_ues.yml"
    "/home/shovon/srsRAN_Project/configs/cell_cfg_max_64_ues.yml"
    "/home/shovon/srsRAN_Project/configs/cu.yml"
    "/home/shovon/srsRAN_Project/configs/cu_up_f1u_multiple_sockets.yml"
    "/home/shovon/srsRAN_Project/configs/du_f1u_multiple_sockets.yml"
    "/home/shovon/srsRAN_Project/configs/du_rf_b200_tdd_n78_20mhz.yml"
    "/home/shovon/srsRAN_Project/configs/geo_ntn.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_custom_cell_properties.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_rf_b200_tdd_n78_20mhz.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_rf_b210_fdd_srsUE.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_rf_n310_fdd_n3_20mhz.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_ru_picocom_scb_tdd_n78_20mhz.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_ru_ran550_tdd_n78_100mhz_4x2.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_ru_ran550_tdd_n78_20mhz.yml"
    "/home/shovon/srsRAN_Project/configs/gnb_ru_rpqn4800e_tdd_n78_20mhz_2x2.yml"
    "/home/shovon/srsRAN_Project/configs/low_latency.yml"
    "/home/shovon/srsRAN_Project/configs/mimo_usrp.yml"
    "/home/shovon/srsRAN_Project/configs/mobility.yml"
    "/home/shovon/srsRAN_Project/configs/ngu_multiple_sockets.yml"
    "/home/shovon/srsRAN_Project/configs/qam256.yml"
    "/home/shovon/srsRAN_Project/configs/qos.yml"
    "/home/shovon/srsRAN_Project/configs/slicing.yml"
    "/home/shovon/srsRAN_Project/configs/srb.yml"
    "/home/shovon/srsRAN_Project/configs/testmode.yml"
    )
endif()

