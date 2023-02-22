srsRAN
======

srsRAN is a complete 5G RAN solution, featuring an ORAN-native CU/DU developed by [SRS](http://www.srs.io).

The solution includes a complete L1/2/3 implementation with minimal external dependencies. Portable across processor architectures, the software has been optimized for x86 and ARM. srsRAN follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU) and centralized unit (CU). The CU is further disaggregated into control plane (CU-CP) and user-plane (CU-UP).

srsRAN further follows O-RAN architecture principles, supports all external interfaces (F1, E1, E2, A1, O1, N2 and N3) and allows split 7.2a/b as well as split 8 deployments for the fronthaul connection.

License
-------

For license details, see [LICENSE](LICENSE) file.

Build Instructions
------------------

* Mandatory requirements:
  * cmake:               <https://cmake.org/>
  * libfftw:             <https://www.fftw.org/>
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
  * UHD:                 <https://github.com/EttusResearch/uhd>
  * ZeroMQ:              <https://github.com/zeromq>

Download and build srsRAN:

```bash
git clone https://github.com/srsran/srsRAN_Project.git
cd srsran
mkdir build
cd build
cmake ../
make
make test
```