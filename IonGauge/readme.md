# IonGuage
Version: 1.0.0

Author: X. Carroll

## Overview
This project contains C and Python code for interacting with the Micro-Ion Plus Vacuum Guage Module. The `python` folder contains a Jupyter notebook with instructions for interacting with the ion-gauge module. The `src` folder contains the C code.

Both the Jupyter notebook and C libraries provide error handling if the ion module is disconnected or powered off.

The C code is designed to be used as a library, providing two helper functions needed to interact with the ion module. In practice, these two functions would be included in another project, or compiled into a library that is used in another project. However, a main function is included here so that the code can be tested when it is built.

## Build Instructions
Run the `make` command from the IonGuage directory.

## Usage Instructions
Run the generated executable.
- `./test`