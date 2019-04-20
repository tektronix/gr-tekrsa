/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3, or (at your
 * option) any later version.
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

#ifndef INCLUDED_TEKRSA_IQ_BLK_CMPLX_IMPL_H
#define INCLUDED_TEKRSA_IQ_BLK_CMPLX_IMPL_H

#include <string>

#include <TekRSA/iq_blk_cmplx.h>
#include "ipp.h"
#include "RSA_API.h"

#include <TekRSA/iq_blk_cmplx.h>

namespace gr {
  namespace TekRSA {

    class iq_blk_cmplx_impl : public iq_blk_cmplx
    {

     public:
      iq_blk_cmplx_impl(float in_ctr_freq, float in_ref_lvl, float in_bw,
			int in_rec_len, int in_dev_id);
      ~iq_blk_cmplx_impl();
	virtual void set_cf(float in_ctr_freq);
	virtual void set_rl(float in_ref_lvl);
	virtual void set_bw(float in_bw);
	virtual void set_bl(float in_rec_len);

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace TekRSA
} // namespace gr

#endif /* INCLUDED_TEKRSA_IQ_BLK_CMPLX_IMPL_H */

