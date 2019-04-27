/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_TEKRSA_IQ_stream_IMPL_H
#define INCLUDED_TEKRSA_IQ_stream_IMPL_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <TekRSA/iq_stream.h>
#include "RSA_API.h"
#include <queue>
#include <string>

#include <TekRSA/iq_stream.h>

namespace gr {
  namespace TekRSA {

    class iq_stream_impl : public iq_stream
    {

     public:
      iq_stream_impl(float in_ctrfreq, float in_reflvl, float in_bw, int in_recLen, int in_devid);
      ~iq_stream_impl();
	virtual void set_cf(float in_ctrfreq);
	virtual void set_rl(float in_reflvl);
	virtual void set_bw(float in_bw);

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace TekRSA
} // namespace gr

#endif /* INCLUDED_TEKRSA_IQ_stream_IMPL_H */

