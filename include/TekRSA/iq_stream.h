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


#ifndef INCLUDED_TEKRSA_IQ_stream_H
#define INCLUDED_TEKRSA_IQ_stream_H

#include <TekRSA/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace TekRSA {

    /*!
     * \brief <+description of block+>
     * \ingroup TekRSA
     *
     */
    class TEKRSA_API iq_stream : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<iq_stream> sptr;
      virtual void set_cf(float in_ctr_freq) = 0;
      virtual void set_rl(float in_ref_lvl) = 0;
	virtual void set_bw(float in_bw) = 0;

      /*!
       * \brief Return a shared_ptr to a new instance of TekRSA::iq_stream.
       *
       * To avoid accidental use of raw pointers, TekRSA::iq_stream's
       * constructor is in a private implementation
       * class. TekRSA::iq_stream::make is the public interface for
       * creating new instances.
       */
      static sptr make(float in_ctrfreq, float in_reflvl, float in_bw, int in_recLen, int in_devid);
    };

  } // namespace TekRSA
} // namespace gr

#endif /* INCLUDED_TEKRSA_IQ_stream_H */

