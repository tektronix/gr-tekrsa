/* -*- c++ -*- */

#define TEKRSA_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "TekRSA_swig_doc.i"

%{
#include "TekRSA/iq_blk_cmplx.h"
#include "TekRSA/iq_stream.h"
%}


%include "TekRSA/iq_blk_cmplx.h"
GR_SWIG_BLOCK_MAGIC2(TekRSA, iq_blk_cmplx);
%include "TekRSA/iq_stream.h"
GR_SWIG_BLOCK_MAGIC2(TekRSA, iq_stream);
