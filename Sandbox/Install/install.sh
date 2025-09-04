#!/bin/bash

# Usage Instructions:
# ------------------------
# The hdf5 and libtelemetry RPMs should be in the directory where the script
# will be run.
#
# Additionally, many of these packages come from non-standard repositories. The
# following repositories must be enabled/installed before executing this script:
#   - CRB
#   - EPEL
#
# Then, execute this script as root to install the following packages:
#   - hdf5, hdf5-devel
#   - lbto-libtelemetry, lbto-libtelemetry-devel, lbto-libtelemetry-leapseconds
#   - qt6-qtbase-devel, qt6-designer
#   - doxygen
#   - libmodbus, libmodbus-devel
#   - readline
#
# The installation results will be output to a file with the name ${OUTPUT_FILE}.

OUTPUT_FILE="results.txt"
FLAGS="-y"

{

# Install hdf5 library from local RPM files.

dnf localinstall ${FLAGS} hdf5-1.12.1-11_lbto.el9.x86_64.rpm;
dnf localinstall ${FLAGS} hdf5-devel-1.12.1-11_lbto.el9.x86_64.rpm;

# Install lbto-libtelemetry from local RPM files.

dnf localinstall ${FLAGS} lbto-libtelemetry-leapseconds-5-0.el9.x86_64.rpm;
dnf localinstall ${FLAGS} lbto-libtelemetry-5-0.el9.x86_64.rpm;
dnf localinstall ${FLAGS} lbto-libtelemetry-devel-5-0.el9.x86_64.rpm;

# Install Qt6 graphics library.

dnf install ${FLAGS} qt6-qtbase-devel qt6-designer;

# Install libmodsbus.

dnf install ${FLAGS} libmodbus libmodbus-devel ;

# Install GNU Readline library.

dnf install ${FLAGS} readline readline-devel ;

# Other stuff: doxygen, tk, tcsh, etc.

dnf install ${FLAGS} doxygen tk tcsh emacs dnstools wget ;

# Zero-C Ice install

sudo dnf -y install https://download.zeroc.com/ice/3.7/el9/ice-repo-3.7.el9.noarch.rpm ;
sudo dnf -y install libice-c++-devel libice3.7-c++ python3-ice ;
sudo dnf -y install icebox ;

} &> ${OUTPUT_FILE}
