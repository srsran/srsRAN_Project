srsRAN Project
==============

[![Build Status](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml/badge.svg?branch=main)](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml)
[![CodeQL](https://github.com/srsran/srsRAN_Project/actions/workflows/codeql.yml/badge.svg?branch=main)](https://github.com/srsran/srsRAN_Project/actions/workflows/codeql.yml)

The srsRAN Project is a complete 5G RAN solution, featuring an ORAN-native CU/DU developed by [SRS](http://www.srs.io).

The solution includes a complete L1/2/3 implementation with minimal external dependencies. Portable across processor architectures, the software has been optimized for x86 and ARM. srsRAN follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU) and centralized unit (CU). The CU is further disaggregated into control plane (CU-CP) and user-plane (CU-UP).

See the [srsRAN Project](https://www.srsran.com/) for information, guides and project news.

For application features, build instructions and user guides see the [srsRAN Project documentation](https://docs.srsran.com/projects/project).

For community announcements and support, join our [discussion board](https://www.github.com/srsran/srsran_project/discussions).

Current Features
----------------

* FDD/TDD supported, all FR1 bands
* 15/30 KHz subcarrier spacing
* All physical channels including PUCCH Format 1 and 2, excluding Sounding-RS 
* Highly optimized LDPC and Polar encoder/decoder for ARM Neon and x86 AVX2/AVX512
* All RRC procedures excluding Mobility, Paging and Reestablishment
* All MAC procedures excluding power control
* Tested Performance:
  * 64 simultaneous users
  * Simultaneous 140 Mbps DL and 120 Mbps UL in TDD-100 MHz or FDD-50 MHz configuration (running in AMD Ryzen5 6-core CPU)

License
-------

For license details, see [LICENSE](LICENSE) file.

Build Preparation
-----------------

* Build tools:
  * cmake:               <https://cmake.org/>
  
* Mandatory requirements:
  * libfftw:             <https://www.fftw.org/>
  * libsctp:             <https://github.com/sctp/lksctp-tools>
  * yaml-cpp:            <https://github.com/jbeder/yaml-cpp>
  * PolarSSL/mbedTLS:    <https://www.trustedfirmware.org/projects/mbed-tls/>
  * googletest:          <https://github.com/google/googletest/>
    * You can skip test building by using the cmake option `-DBUILD_TESTS=OFF`. GoogleTest is not mandatory when building without tests.

You can install the build tools and mandatory requirements for some example distributions with the commands below:

<details open>
<summary>Ubuntu 22.04</summary>


```bash
sudo apt-get install cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
```
</details>
<details>
<summary>Fedora</summary>


```bash
sudo yum install cmake make gcc gcc-c++ fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gtest-devel
```
</details>
<details>
<summary>Arch Linux</summary>


```bash
sudo pacman -S cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest
```
</details>

The srsRAN Project uses RF drivers to support different radio types.
Currently, only UHD is supported however additional drivers are under development:

* RF driver:
  * UHD:                 <https://github.com/EttusResearch/uhd>
    * See UHD documentation for installation instructions.
    
Build Instructions
------------------

Download and build srsRAN:

```bash
git clone https://github.com/srsran/srsRAN_Project.git
cd srsRAN_Project
mkdir build
cd build
cmake ..
make
make test
```

Run PHY testvector tests:

A number of PHY tests are based on MATLAB generated testvectors. By default, those tests are disabled.
The following steps are required to enable them:

1. Download the [PHY testvector set](https://github.com/srsran/srsRAN_Project/releases/download/release_23_5/phy_testvectors.tar.gz).
2. Copy the PHY testvectors to its location within srsRAN:
```
tar -xzf phy_testvectors.tar.gz -C /path_to_your_local_repository/srsgnb/
```
3. Enable the use of PHY testvectors by modifying the root [CMakeLists](CMakeLists.txt) as shown below:
```
option(USE_PHY_TESTVECTORS   "Enable testvector PHY tests"              ON)
```
4. Do a fresh srsRAN build.
