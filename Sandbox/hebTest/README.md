# HEB lab testing utilities

**Updated**: 2025 Aug 26 [rwp/osu]

Programs and Jupyter notebooks for testing Archon head electronics box (HEB)
functions separate from the instrument control and data-taking system.

## Contents

 * `HEB_WAGO.ipynb` - Test the WAGO unit in an HEB
 * `testHEB.ipynb` - step-by-step function test of a flight HEB (subset of `HEB_WAGO.ipynb`)
 * `measQC.py` - "stripchart" plot of a quad cell readout, used for verification and bias adjustment, must be edited to change the WAGO address
 * `readQC.py` - ascii-text readout with simple stats, useful for checkout over slow networks.
 * `qcTest.py` - read and record QC data for long time series.  Must be edited to change the WAGO address.
 * `igTest.ipynb` - Notebook to test the vacuum ionization gauges on MODS (connect via Comtrol but powered on via HEB WAGO)

### Dependencies

needs `pymodbus` module installed
```shell
pip install pymodbus
```

#### Notes

In the OSU Lab we were using pymodbus v3.6.9, on the mountain we installed it on the new MODS
machines from pip and are running v3.11.0. It looks like in version 3.8.4 they added count= keyword
to the read_xyz() methods, which is what was broken in a non-obvious way when we first fired
it up.  Beware!
