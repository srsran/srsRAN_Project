Amarisoft support
-----------------

Amarisoft's UE can be interfaced with `srsran` applications using a custom TRX driver implementation without the need of
any RF device.

In order to enable the TRX driver compilation, the TRX driver header path must be provided:
```bash
cmake .. -DENABLE_EXPORT=TRUE -DENABLE_ZEROMQ=TRUE -DENABLE_TRX_DRIVER=TRUE -DTRX_DRIVER_DIR=<path to trx_uhd-linux-...>
make trx_srsran_test
ctest -R trx_srsran_test
```

Make sure CMake finds the file `trx_driver.h` in the specified folder. CMake must print a similar line:
```
-- Found trx_driver.h in TRX_DRIVER_DIR=/home/user/amarisoft/2021-03-15/trx_uhd-linux-2021-03-15/trx_driver.h
```

A symbolic link must be done for the UE application to load the driver:
```
cd <path to lteue-linux-...>
ln -s <path to srsran build directory>/utils/trx_srsran/libtrx_srsran.so trx_srsran.so
```

And finally, the UE's RF configuration shall be:
```
  rf_driver: {
    // Select srsran TRX driver.
    name: "srsran",

    // TCP transmit and receive ports.
    tx_port0:  "tcp://*:6000",
    rx_port0:  "tcp://localhost:5000",
    tx_port1:  "tcp://*:6001",
    rx_port1:  "tcp://localhost:5001",
    tx_port2:  "tcp://*:6002",
    rx_port2:  "tcp://localhost:5002",
    tx_port3:  "tcp://*:6003",
    rx_port3:  "tcp://localhost:5003",

    // srsran logging level.
    log_level: "info",

    // Enables a noise generator in the uplink path. Determines the noise power spectral density in dBm/Hz for each gNb 
    // receive channel.
    noise_spd: -174,
  },
```