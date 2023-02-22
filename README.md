srsRAN Project
==============

The srsRAN Project is a complete 5G RAN solution, featuring an ORAN-native CU/DU developed by [SRS](http://www.srs.io).

The solution includes a complete L1/2/3 implementation with minimal external dependencies. Portable across processor architectures, the software has been optimized for x86 and ARM. srsRAN follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU) and centralized unit (CU). The CU is further disaggregated into control plane (CU-CP) and user-plane (CU-UP).

See the [srsRAN Project](https://www.srsran.com/) for information, guides and project news.

For application features, build instructions and user guides see the [srsRAN Project documentation](https://docs.srsran.com/projects/project).

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

Build Instructions
------------------

* Mandatory requirements:
  * cmake:               <https://cmake.org/>
  * libfftw:             <https://www.fftw.org/>
  * UHD:                 <https://github.com/EttusResearch/uhd>
  * libsctp:             <https://github.com/sctp/lksctp-tools>
  * yaml-cpp:            <https://github.com/jbeder/yaml-cpp>
  * PolarSSL/mbedTLS:    <https://www.trustedfirmware.org/projects/mbed-tls/>
  * googletest:          <https://github.com/google/googletest/>
    * You can skip test building by using the cmake option `-DBUILD_TESTS=False`. GoogleTest is not mandatory when building without tests.

You can install the required libraries for some example distributions with the commands below:

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

* Optional requirements
  * ZeroMQ:              <https://github.com/zeromq>

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
