# Configuration Examples

This folder contains a number of selected example configuration files that represent some
of the most common use-cases and configuration options. Their main goal is to simplify
the bring up and, therefore, they only expose the most important as well as the most likely
to change configuration options. For the sake of simplicity and readability they do not
list all available configuration sections, subsection and fields.

## Usage

The srsRAN Project gNB can be configured via one or multiple configuration files and/or via 
the command line. When multiple configuration files are specified, their content gets combined 
by the gNB application. In case there are repeated parameters across configuration files, the 
parameter values of the last file are selected. Similarly, the parameters passed via command 
line overwrite the parameters specified via configuration files. If the value of a parameter is 
not specified by neither the configuration files nor through the command line, a default value 
is selected by the application.

The capability to overlay parameters via multiple configuration files and through the command line 
is useful to minimize config duplication. For example users can share the same QoS configuration 
across many experiments in different bands as follows: 

  sudo ./gnb -c qos.yml -c gnb_ru_ran550_tdd_n78_20mhz.yml


## Full list of configuration options

Look [here](https://docs.srsran.com/projects/project/en/latest/user_manuals/source/config_ref.html) for
the main configuration reference. We update this list regularly and aim to always keep it up-to-date
with the actual code.

This process, however, is done manually and might not always be 100% accurate.
To obtain a full list of configuration options available on the command line and in configuration files
it is best to invoke the respective binary with the `-h` parameter and navigate through the subsections.
Note that the each section might contain multiple subsections. To obtain the list of available config
options for a subsection it is again required to call the binary with the `subsectioname -h` parameter.

For example, to obtain the list of all main gNB configuration options run:

```
$ ./apps/gnb/gnb -h
srsGNB application
Usage: ./apps/gnb/gnb [OPTIONS] [SUBCOMMAND]

Options:
  -h,--help                   Print this help message and exit
  -v,--version                Display program version information and exit
  -c                          Read config from file
  --gnb_id UINT [411]         gNodeB identifier
  --gnb_id_bit_length UINT:INT in [22 - 32] [ ]
                              gNodeB identifier length in bits
  --ran_node_name TEXT [srsgnb01]
                              RAN node name
  --cells TEXT ...            Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg
  --qos TEXT ...              Configures RLC and PDCP radio bearers on a per 5QI basis.

Subcommands:
  log                         Logging configuration
  pcap                        PCAP configuration
  amf                         AMF parameters
  cu_cp                       CU-CP parameters
  ru_sdr                      SDR Radio Unit configuration
  ru_ofh                      Open Fronthaul Radio Unit configuration
  cell_cfg                    Default cell configuration
  expert_phy                  Expert physical layer configuration
  test_mode                   Test mode configuration
```

To then list all options for the `cell_cfg` subcommand, run the binary again with:

```
$ ./apps/gnb/gnb cell_cfg -h
Default cell configuration
Usage: ./apps/gnb/gnb cell_cfg [OPTIONS] [SUBCOMMAND]

Options:
  -h,--help                   Print this help message and exit
  --pci UINT:INT in [0 - 1007] [1]
                              PCI
  --dl_arfcn UINT [536020]    Downlink ARFCN
  --band TEXT [auto]          NR band
  --common_scs ENUM [15kHz]   Cell common subcarrier spacing
  --channel_bandwidth_MHz ENUM [20]
                              Channel bandwidth in MHz
  --nof_antennas_ul UINT [1]  Number of antennas in uplink
  --nof_antennas_dl UINT [1]  Number of antennas in downlink
  --plmn TEXT [00101]         PLMN
  --tac UINT [7]              TAC
  --ssb_period UINT:{5,10,20} [10]
                              Period of SSB scheduling in milliseconds

Subcommands:
  pdcch                       PDCCH parameters
  pdsch                       PDSCH parameters
  pusch                       PUSCH parameters
  pucch                       PUCCH parameters
  prach                       PRACH parameters
  tdd_ul_dl_cfg               TDD UL DL configuration parameters
  paging                      Paging parameters
```

Note that the `cell_cfg` subcommand has multiple configuration options but also exposes more subcommands.