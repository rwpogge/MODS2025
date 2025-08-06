# HEB lab testing

Programs and Jupyter notebooks for testing Archon head electronics box (HEB)
functions separate from the instrument control and data-taking system.

## Contents

 * `HEB_WAGO.ipynb` - Test the WAGO unit in an HEB
 * `testHEB.ipynb` - step-by-step function test of a flight HEB (subset of `HEB_WAGO.ipynb`)
 * `measQC.py` - "stripchart" plot of a quad cell readout, used for verification and bias adjustment, must be edited to change the WAGO address
 * `qcTest.py` - read and record QC data for long time series.  Must be edited to change the WAGO address.

### Dependencies

needs `pymodbus` module installed
```shell
pip install pymodbus
```
