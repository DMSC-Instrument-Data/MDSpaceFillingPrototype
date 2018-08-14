# Q-space conversion

A simple conversion to Q-space for the elastic case is implemented in
`EventToMDEventConversion.[h,cpp]`.

## Usage

To simplify implementation a Python script is used to convert instrument
definition files to a basic HDF5 format containing the instrument geometry. This
conversion is performed using:
```
./get_geometry.py WISH_Definition_10Panels.xml wish.nxs
```

The `QConversionDemo` is used to perform the conversion, currently it simply
performs the conversion and saves the Q-space coordinates obtained from a
conversion back from the interleaved coordinates. It can be run using:
```
./QConversionDemo -data WISH00034509.nxs -frames all -instrument wish.h5
```

## Benchmark

A Q conversion benchmark has been implemented for the following instruments and
datasets:

- WISH, 34509 (V/Nb rod)
- WISH, 37828 (hexaferrite 300K 0T)
- WISH, 37868 (hexaferrite 20K 0.4T)
- WISH, 38423 (NaCl)
- TOPAZ, 3132 (triphylite)
- SXD, 23767 (NaCl sphere)

The times taken for each stage of the workflow (Q conversion, MD event sorting,
box structure construction) are reported individually in seconds. The number of
MD events created is also reported.

### Prototype

Command:
```bash
./src/benchmark/QConversionBenchmark --benchmark_counters_tabular=true --benchmark_min_time=120
```

Output:
```
2018-08-14 10:48:42
Running ./src/benchmark/QConversionBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
---------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                               Time           CPU Iterations box_structure  md_events q_conversion       sort
---------------------------------------------------------------------------------------------------------------------------------------
BM_QConversion_WISH_34509<uint16_t, uint64_t>       41631 ms       6576 ms         26       11.7064   469.388M      25.1058    4.14236
BM_QConversion_WISH_34509<uint32_t, uint128_t>      75082 ms      12734 ms         10       26.2229   469.388M      38.0001    9.92662
BM_QConversion_WISH_38423<uint16_t, uint64_t>        1078 ms        585 ms        283      0.314958   15.1266M      0.62719  0.0984566
BM_QConversion_WISH_38423<uint32_t, uint128_t>       2008 ms        826 ms        208      0.833816   15.1266M     0.940869   0.183804
BM_QConversion_WISH_37828<uint16_t, uint64_t>        1400 ms        685 ms        245      0.423877   21.5551M     0.792605   0.139132
BM_QConversion_WISH_37828<uint32_t, uint128_t>       2688 ms        917 ms        185       1.10895   21.5551M      1.24123   0.277144
BM_QConversion_WISH_37868<uint16_t, uint64_t>         611 ms        417 ms        396      0.131891   7.12883M     0.410433  0.0493872
BM_QConversion_WISH_37868<uint32_t, uint128_t>       1007 ms        506 ms        332       0.33555   7.12883M     0.557575  0.0874628
BM_QConversion_TOPAZ_3132<uint16_t, uint64_t>        1400 ms       1072 ms        157      0.232032    9.0473M      1.07019  0.0655026
BM_QConversion_TOPAZ_3132<uint32_t, uint128_t>       1930 ms       1257 ms        139      0.476999    9.0473M      1.29776   0.117478
BM_QConversion_SXD_23767<uint16_t, uint64_t>          824 ms        394 ms        431      0.276481   14.3206M     0.425418  0.0891088
BM_QConversion_SXD_23767<uint32_t, uint128_t>        1677 ms        580 ms        311      0.728931   14.3206M     0.727463     0.1754
```

### Mantid

A benchmark for the equivalent operations using the `ConvertToMD` algorithm in
Mantid was implemented in `mantid_benchmark.py`, essentially this just times the
execution of `ConvertToMD`.

Command:
```bash
mantidpython mantid_benchmark.py
```

Output:
```
Data file: /home/dan/WISH00034509.nxs
Iteration count:  2
Total time:  435.449289083
Average time:  217.724644542

Data file: /home/dan/WISH00037828_event.nxs
Iteration count:  46
Total time:  301.932240963
Average time:  6.56374436876

Data file: /home/dan/WISH00037868_event.nxs
Iteration count:  131
Total time:  300.174309492
Average time:  2.29140694269

Data file: /home/dan/WISH00038423_event.nxs
Iteration count:  57
Total time:  302.14209199
Average time:  5.30073845596

Data file: /home/dan/TOPAZ_3132_event.nxs
Iteration count:  96
Total time:  302.360197783
Average time:  3.14958539357

Data file: /home/dan/SXD23767_event.nxs
Iteration count:  174
Total time:  301.50667119
Average time:  1.73279696086
```
