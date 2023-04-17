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
  * git-lfs:             <https://git-lfs.github.com/>
  * googletest:          <https://github.com/google/googletest/>
    * You can skip test building by using the cmake option `-DBUILD_TESTS=False`. GoogleTest is not mandatory when building without tests.

You can install the required libraries for some example distributions with the commands below:

<details open>
<summary>Ubuntu 22.04</summary>


```bash
sudo apt-get install cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev git-lfs
git lfs install
```
</details>
<details>
<summary>Fedora</summary>


```bash
sudo yum install cmake make gcc gcc-c++ pkgconf fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gtest-devel git-lfs
git lfs install
```
</details>
<details>
<summary>Arch Linux</summary>


```bash
sudo pacman -S cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest git-lfs pkgconf
git lfs install
```
</details>

* Optional requirements
  * UHD:                 <https://github.com/EttusResearch/uhd>
  * ZeroMQ:              <https://github.com/zeromq>

Download and build srsRAN:

```bash
git clone https://gitlab.com/softwareradiosystems/srsran.git
cd srsran
git lfs pull
mkdir build
cd build
cmake ../
make
make test
```

Amarisoft support
-----------------

Amarisoft's UE can be interfaced with `srsran` applications using a custom TRX driver implementation without the need of 
any RF device. 

In order to enable the TRX driver compilation, the TRX driver header path must be provided:
```bash
cmake .. -DENABLE_TRX_DRIVER=On -DTRX_DRIVER_DIR=<path to trx_uhd-linux-...>
make trx_srsran_test
ctest -R trx_srsran_test
```

Make sure CMake finds the file `trx_driver.h` in the specified folder. CMake must print a similar line:
```
-- Found trx_driver.h in TRX_DRIVER_DIR=/home/amarisoft/amarisoft/2021-03-15/trx_uhd-linux-2021-03-15/trx_driver.h
```

A symbolic link must be done for the UE application to load the driver:
```
cd <path to lteue-linux-...>
ln -s <path to srsran build directory>/apps/trx_srsran/libtrx_srsran.so trx_srsran.so
```

And finally, the UE's RF configuration shall be:
```
  rf_driver: {
    name:      "srsran",
    args:      "",
    tx_port0:  "tcp://*:6000",
    rx_port0:  "tcp://localhost:5000",
    log_level: "info"
  },
```