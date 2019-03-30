#!/bin/sh
# Run this to fix up environment before running gnuradio-companion.
export VOLK_GENERIC=1
#export GR_DONT_LOAD_PREFS=1
export srcdir=/home/tekdev/oot/gr-TekRSA/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/tekdev/oot/gr-TekRSA/build/python:$PATH
export LD_LIBRARY_PATH=/home/tekdev/oot/gr-TekRSA/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/tekdev/oot/gr-TekRSA/build/swig:$PYTHONPATH
