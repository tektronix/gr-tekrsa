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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "iq_blk_cmplx_impl.h"

namespace gr {
  namespace TekRSA {

static bool search_and_connect(int, bool);
static bool connect_to_hw_and_setup(int, bool);
static bool return_check(const char* tag, RSA_API::ReturnStatus rs);
static bool g_return_check_verbosity = false;
static bool stop_and_disconnect();
static bool run_alignment();
static int get_iq_block(gr_complex *, int);
static bool can_run = true;

//dyn param change
static int set_cf(float in_ctr_freq);
static int set_rl(float in_ref_lvl);
static int set_bw(float in_bw);
static int set_bl(float in_rec_len);

#ifdef _IS_WINDOWS_OS_
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif
// Translate Rtn status and print string
static bool return_check(const char* tag, RSA_API::ReturnStatus rs)
{
	bool pass = (rs == RSA_API::noError);
	//RSA_API_DLL const char* GetErrorString(ReturnStatus status);
	//if (!pass || g_return_check_verbosity)
	//{
	//	printf("%s: %s: %i:\"%s\"\n", tag, (pass ? "" : "FAILURE"), 
	//		(int)rs, RSA_API::DEVICE_GetErrorString(rs));
	//}
	return pass;
}

static bool search_and_connect(int dev_sel, bool do_reset)
{
	RSA_API::ReturnStatus rs;

	printf("\nSearching for Devices... ");
	int num_dev;
	int* dev_id;
	const char** dev_sn; //device serial number
	const char** dev_type; //rsa model type
	rs = RSA_API::DEVICE_SearchInt(&num_dev,&dev_id,&dev_sn,&dev_type);
	if (!return_check("DEVICE_Search", rs))
		return false;

	printf("Number of Devices found: %i\n", num_dev);
	if (num_dev == 0)
		return false;

	int use_device = -1;
	if (dev_type[dev_sel] != NULL)
	{
		use_device=dev_sel;
		printf("Found user specified device: %s, will use this.\n",
			dev_type[dev_sel]);
	}
	else
	{
		printf("Invalid device ID was entered..Finding all valid devices.\n");
		for (int n = 0; n<num_dev; n++)
		{
			printf("Dev: %i, ID: %i, S/N: \"%s\", Type: \"%s\"\n", n, dev_id[n],
				dev_sn[n], dev_type[n]);     // display connected devices
			std::string this_string = dev_type[n];
			{
				printf("Found first valid device: %s, will use this.\n",dev_type[n]);
				use_device=n;
				break;
			}

		}
	}

	//select device
	int id = use_device;

	//for manual dev# usage
	//int devid = dev_id[dev_sel];

	if (do_reset)    // if requested, reset the device and reconnect
	{
		printf("Resetting device: %d... ", id);
		rs = RSA_API::DEVICE_Reset(id);
		if (!return_check("DEVICE_Reset", rs))
			return false;
		printf("Successful\n");
	}

	rs = RSA_API::DEVICE_Connect(id);
	if (!return_check("DEVICE_Connect", rs))
	{
		printf("Failed on DEVICE_Connect(), please exit the program.");
		return false;   // failed to connect
	}
	printf("Successful connection to %s!\n",dev_type[id]);

	rs = RSA_API::CONFIG_Preset();  // preset device to initial state
	if (!return_check("CONFIG_Preset", rs))
		return false;

	// Print the device SW/FW/HW info

	RSA_API::DEVICE_INFO dev_info;
	rs = RSA_API::DEVICE_GetInfo(&dev_info);
	if (!return_check("DEVICE_GetInfo", rs))
		return false;
	printf("Prod:%s  SN:%s  HW:%s  FX3:%s  FPGA:%s  API:%s\n", 
		dev_info.nomenclature, dev_info.serialNum, dev_info.hwVersion,
		dev_info.fwVersion, dev_info.fpgaVersion, dev_info.apiVersion);

	//enables auto-attentuation and RF Preamp for 5xx-6xx series
	if (dev_info.nomenclature[3] == '5' || dev_info.nomenclature[3] == '6') //5 and 6 for 5xx and 6xx series
	{
		printf("Enabling 5xx-6xx Unique Settings Master.");
		
		bool enable_rsa;
		rs = RSA_API::CONFIG_SetAutoAttenuationEnable(&enable_rsa);
		rs = RSA_API::CONFIG_SetRFPreampEnable(&enable_rsa);

		if(!return_check("CONFIG_SetAutoAttentuationEnable", rs))
			return false;

		if(!return_check("CONFIG_SetRFPreampEnable", rs))
			return false;
	}

	return true;
}

static bool connect_to_hw_and_setup(int dev, bool align)
{
	//search and connect to device via usb, dev is solved in the function
	if (!search_and_connect(dev, align))
		return false;

	//alignment
	/*bool needs_alignment;
	RSA_API::ALIGN_GetAlignmentNeeded(&needs_alignment);
	printf("\nAlignment Needed: %s\n", needs_alignment ? "Yes" : "No");


	if (needs_alignment)
	{
		if (!run_alignment())
		{
			printf("Alignment failed\n");
			return false;
		}
	}
	printf("\nAlignment complete!\n");*/
	return true;
}

/*
 * run_alignment
 * Runs alignment for RSA via device API
 */
static bool run_alignment()
{
	printf("Alignment: Running, please wait...");
	RSA_API::ReturnStatus rs = RSA_API::ALIGN_RunAlignment();
	if (!return_check("ALIGN_RunAlignment", rs))
		return false;
	return true;
}

/*
 * stop_and_disconnect
 * Stops running mode on RSA, disconnects it
 * Reports run statistics if flagged
 */
static bool stop_and_disconnect()
{
	RSA_API::ReturnStatus rs; 
#ifdef RSA_API_V2_OLD
	RSA_API::RunMode run_mode;
	RSA_API::GetRunState(&run_mode);
	if (run_mode == RSA_API::running)     // if running, stop
#else
	bool is_running;
	RSA_API::DEVICE_GetEnable(&is_running);
	if (is_running)     // if running, stop
#endif
	{
		printf("Stopping Device...\n");
		rs = RSA_API::DEVICE_Stop();
		if (!return_check("DEVICE_Stop", rs))
			return false;
	}

	printf("Disconnecting from Device...\n");
	rs = RSA_API::DEVICE_Disconnect();      // disconnect from device
	if (!return_check("DEVICE_Disconnect", rs))
		return false;

	return true;
}

static int get_iq_block(gr_complex *iq_block, int block_len_req)
{
	RSA_API::ReturnStatus rs;
	//inits data acquire sequence

	if (!can_run)
	{
		printf("No device found\n");
		return -1;
	}

	int wait_timeout_ms = 10000;  //time to wait for next block in ms
	int block_len_rtn = 0;
	int block_count = 0;
	bool block_ready;

	//for error checking in acq info
	unsigned int over_range   = 0x1;
	unsigned int freq_ref   = 0x2;
	unsigned int unlocked   = 0x4;
	unsigned int transfer   = 0x8;

	//where we put the iq data from the device for this run
	RSA_API::Cplx32* iq_data = new RSA_API::Cplx32[block_len_req];

	//inits data acquire sequence
	rs = RSA_API::IQBLK_AcquireIQData();
	if (!return_check("IQBLK_AcquireIQData", rs))
	{
		printf("failed at IQBLK_AcquireIQData\n");
		return -1;
	}

	// Get IQ record
	rs = RSA_API::IQBLK_WaitForIQDataReady(wait_timeout_ms, &block_ready);
	if (!return_check("IQBLK_WaitForIQDataReady", rs))
	{
		printf("failed at IQBLK_WaitForIQDataReady\n");
		return(block_len_rtn);
	}

	if (!block_ready)
	{
		printf("failure at IQBLK_WaitForIQDataReady()\n");
	}

	//when the block is ready to be retrieved...
	if (block_ready)
	{
		//get iq cmlpx data from the device and put it in the 'iq_data' object
		rs = RSA_API::IQBLK_GetIQDataCplx(iq_data, &block_len_rtn,
							block_len_req);
		if (!return_check("IQBLK_GetIQDataCplx", rs))
		{
			printf("failed at IQBLK_GetIQDataCplx\n");
			return(block_len_rtn);
		}

		for (int k = 0; k < block_len_rtn; k++)
		{
			iq_block[k].real() = iq_data[k].i;
			iq_block[k].imag() = iq_data[k].q;
		}

		//Queries the IQ acquisition status information for 
		//the most recently retrieved IQ Block record
		RSA_API::IQBLK_ACQINFO acq_info;
		rs = RSA_API::IQBLK_GetIQAcqInfo(&acq_info);     // Get IQ acq info
		if (!return_check("IQBLK_GetIQAcqInfo", rs))
		{
			printf("failed at IQBLK_GetIQAcqInfo\n");
			return(block_len_rtn);
		}

		//if the block is not over_range, use it
		if (acq_info.acqStatus != 0)
		{
			if ((acq_info.acqStatus & over_range) == over_range)
			{
				//printf("Warning! ADC input over_range. (0x1)\n");
				//printf("0x1 (Overrange)\n");
				printf("o");
			}
			if ((acq_info.acqStatus & freq_ref) == freq_ref)
			{
				//printf("Warning! Frequency Reference unlocked during acquisition. (0x2)\n");
				//printf("0x2 (Unlocked)\n");
				printf("u");
			}
			if ((acq_info.acqStatus & unlocked) == unlocked)
			{
				//printf("Warning! Internal oscillator unlocked or power failure during acquisition. (0x4)\n");
				//printf("0x4 (OscFail)\n");
				printf("p");
			}
			if ((acq_info.acqStatus & transfer) == transfer)
			{
				//printf("Warning! USB frame transfer error detected during acquisition. (0x8)\n");
				//printf("0x8 (TransError)\n");
				printf("t");
			}
		}
	}
	return(block_len_rtn);
}

void iq_blk_cmplx_impl::set_cf(float in_ctr_freq)
{
	RSA_API::ReturnStatus rs;
	double ctr_freq = (double)in_ctr_freq;

	rs = RSA_API::CONFIG_SetCenterFreq(ctr_freq);
		if (!return_check("CONFIG_SetCenterFreq", rs))
		{	
			return;
		}
	return;	
}

void iq_blk_cmplx_impl::set_rl(float in_ref_lvl)
{
	RSA_API::ReturnStatus rs;
	double ref_lvl = (double)in_ref_lvl;

	rs = RSA_API::CONFIG_SetReferenceLevel(ref_lvl);
		if (!return_check("CONFIG_SetReferenceLevl", rs))
		{	
			return;
		}
	return;	
}

void iq_blk_cmplx_impl::set_bw(float in_bw)
{
	RSA_API::ReturnStatus rs;
	double bw = (double)in_bw;

	rs = RSA_API::IQBLK_SetIQBandwidth(bw);
	if (!return_check("IQBLK_SetIQBandwidth", rs))
	{
		return;
	}
	double iq_SR; //sample rate
	RSA_API::IQBLK_GetIQSampleRate(&iq_SR);
	printf("New IQ Sample Rate: %.6f MHz\n", iq_SR);
	return;	
}

void iq_blk_cmplx_impl::set_bl(float in_rec_len)
{
	RSA_API::ReturnStatus rs;
	double bl = (double)in_rec_len;

		rs = RSA_API::IQBLK_SetIQRecordLength(bl);
		if (!return_check("IQBLK_SetIQRecordLength", rs))
		{
			return;
		}
	return;	
}

//GNU radio funcs
    iq_blk_cmplx::sptr
    iq_blk_cmplx::make(float in_ctr_freq, float in_ref_lvl, float in_bw,
			int in_rec_len, int in_devid)
    {
      return gnuradio::get_initial_sptr
        (new iq_blk_cmplx_impl(in_ctr_freq,in_ref_lvl,in_bw,
				in_rec_len, in_devid));
    }

    /*
     * The private constructor,gets and sets device settings
     */
    iq_blk_cmplx_impl::iq_blk_cmplx_impl(float in_ctr_freq,
					float in_ref_lvl, float in_bw,
					int in_rec_len, int in_devid)
      : gr::sync_block("iq_blk_cmplx",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
	RSA_API::ReturnStatus rs;
	double ctr_freq = (double)in_ctr_freq;
	double ref_lvl = (double)in_ref_lvl;
	double bw = (double)in_bw;
	int rec_len = in_rec_len;
	int dev_id_ctor = in_devid;
	
	ippInit();

	//Collect and set all RSA configs
	bool reboot = false;
	if (!connect_to_hw_and_setup(dev_id_ctor, reboot))
	{
		printf("Failed on ConnectToHWandSetup...\n");
		can_run = false;
	}

	if (can_run)
	{
		// Print app build date/time and API version
		char api_version[200];
		RSA_API::DEVICE_GetAPIVersion(api_version);

		// Query system RL and CF settings get ref level and center freq
	    	rs = RSA_API::CONFIG_SetReferenceLevel(ref_lvl);
		if (!return_check("CONFIG_SetReferenceLevel", rs))
		{
			//printf("failed on CONFIG_SetReferenceLevel\n");
		}

		rs = RSA_API::CONFIG_SetCenterFreq(ctr_freq);
		if (!return_check("CONFIG_SetCenterFreq", rs))
		{	
			//printf("failed on CONFIG_SetCenterFreq\n");
		}

		rs = RSA_API::CONFIG_GetReferenceLevel(&ref_lvl);
		if (!return_check("CONFIG_GetReferenceLevel", rs))
		{
			//printf("failed on CONFIG_GetReferenceLevel\n");
		}

		rs = RSA_API::CONFIG_GetCenterFreq(&ctr_freq);
		if (!return_check("CONFIG_GetCenterFreq", rs))
		{
			//printf("failed on CONFIG_GetCenterFreq\n");
		}

		// Check device limits
		double max_bw, min_bw;
		int max_samples;
		RSA_API::IQBLK_GetMinIQBandwidth(&min_bw);
		RSA_API::IQBLK_GetMaxIQBandwidth(&max_bw);
		RSA_API::IQBLK_GetMaxIQRecordLength(&max_samples);
		printf("IQ Block Limits: Min/MaxBW: %.6f - %.6f MaxBlockSize:%u\n",
			min_bw / 1e6, max_bw / 1e6, (unsigned int)max_samples);

		//set bw if it inits above or below limits
		if (bw < min_bw)
		{
			bw = min_bw;
		}
		else if (bw > max_bw)
		{
			bw = max_bw;
		}

		if (rec_len > max_samples)
		{
			rec_len = max_samples;
		}

		//set bandwidth length, now that it is safe
		rs = RSA_API::IQBLK_SetIQBandwidth(bw);
		if (!return_check("IQBLK_SetIQBandwidth", rs))
		{
			//printf("failed on IQBLK_SetIQBandwidth\n");
		}

		//set iq block length
		rs = RSA_API::IQBLK_SetIQRecordLength(rec_len);
		if (!return_check("IQBLK_SetIQRecordLength", rs))
		{
			//printf("failed on IQBLK_SetIQRecordLength\n");
		}

		//get settings as stated be device
		double iq_bw, iq_sr;
		RSA_API::IQBLK_GetIQBandwidth(&iq_bw);
		RSA_API::IQBLK_GetIQSampleRate(&iq_sr);
		double out_cf, out_rl;
		RSA_API::CONFIG_GetCenterFreq(&out_cf);
		RSA_API::CONFIG_GetReferenceLevel(&out_rl);

		// Display user settings
		printf("\nIQ Block Settings: \ 
		\nCenter Frequency: %.6f MHz \ 
		\nReference Level: %.6f dB\nBlock Size: %d points \
		\n", out_cf / 1e6, out_rl, rec_len);

		printf("Acq BW: %.6f MHz\nIQ Sample Rate: %.6f MHz\n \ 
		\n", iq_bw / 1e6, iq_sr / 1e6);
	}
    }

    /*
     * Our virtual destructor.
     */
    iq_blk_cmplx_impl::~iq_blk_cmplx_impl()
    {
	stop_and_disconnect();
    }

    int
    iq_blk_cmplx_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
	gr_complex *out = (gr_complex *) output_items[0];
        int num_items = get_iq_block(out, noutput_items);
        return num_items;
    }

  } /* namespace rsa306 */
} /* namespace gr */

