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
#include "iq_stream_impl.h"

namespace gr {
  namespace TekRSA {

static bool search_and_connect(int, bool);
static bool connect_to_hw_and_setup(int, bool);
static bool return_check(const char* tag, RSA_API::ReturnStatus rs);
static bool g_return_check_verbosity = false;
static bool stop_and_disconnect();
static int get_iq_data(void *);
static int get_iq_output(gr_complex *, int);
static bool can_run = true;
void *m_iq_stream_client_buf;
static std::queue<RSA_API::Cplx32> queue_iq;
static int queue_size;
static boost::mutex cb_mutex;

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
	
	RSA_API_DLL const char* GetErrorString(RSA_API::ReturnStatus status);
	if (!pass || g_return_check_verbosity)
	{
		printf("%s: %s: %i:\"%s\"\n", tag, 
			(pass ? "" : "FAILURE"), 
			(int)rs, RSA_API::DEVICE_GetErrorString(rs));
	}
	return pass;
}

static bool search_and_connect(int dev_sel, bool do_reset)
{
	RSA_API::ReturnStatus rs;

	printf("\nSearching for Devices... ");
	int num_dev;
	int* dev_id;
	const char** dev_sn;
	const char** dev_type;
	rs = RSA_API::DEVICE_SearchInt(&num_dev,&dev_id, &dev_sn,&dev_type);
	if (!return_check("DEVICE_Search", rs))
		return false;

	printf("Number of Devices found: %i\n", num_dev);
	if (num_dev == 0)
		return false;

	int use_dev = -1;
	if (dev_type[dev_sel] != NULL)
	{
		use_dev=dev_sel;
		printf("Found user specified device: %s.\n",
			dev_type[dev_sel]);
	}
	else
	{
		printf("Invalid device ID was entered..Finding all valid devices.\n");
		for (int n = 0; n<num_dev; n++)
		{
			// display connected devices
			printf("Dev: %i, ID: %i, S/N: \"%s\", ", n, dev_id[n], dev_sn[n]);
			printf("Type: \"%s\"\n", dev_type[n]);
			std::string this_string = dev_type[n];
			//should allow us to see any RSA's, not working
			//if (this_string.substr(0,3) == "RSA") 
			{
				printf("Found first valid device: %s, will use this.\n",dev_type[n]);
				use_dev=n;
				break;
			}
		}
	}

	//select device
	int id = use_dev;

	//for manual dev# usage
	//int dev_id = dev_id[dev_sel];

	if (do_reset)    // if requested, reset the device and reconnect
	{
		printf("Resetting device: %d... ", id);
		rs = RSA_API::DEVICE_Reset(id);
		if (!return_check("DEVICE_Reset", rs))
			return false;
		printf("Successful\n");
	}

	printf("Connecting to device\n");
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
		
		bool enable_rsa = true;
		rs = RSA_API::CONFIG_SetAutoAttenuationEnable(&enable_rsa);
		if(!return_check("CONFIG_SetAutoAttentuationEnable", rs))
			return false;
		bool enable_preamp = false;
		rs = RSA_API::CONFIG_SetRFPreampEnable(&enable_preamp);
		if(!return_check("CONFIG_SetRFPreampEnable", rs))
			return false;
	}

	return true;
}

static bool connect_to_hw_and_setup(int dev, bool align)
{
	//search and connect to device via usb
	if (!search_and_connect(dev, align))
		return false;
	/*
	//alignment
	bool needsAlignment;
	RSA_API::ALIGN_GetAlignmentNeeded(&needsAlignment);
	printf("\nAlignment Needed: %s\n", needsAlignment ? "Yes" : "No");
	

	if (needsAlignment)
	{
		if (!RunAlignment())
		{
			printf("Alignment failed\n");
			return false;
		}
	}
	printf("\nAlignment complete!\n");
	*/
	return true;
}

/*
 * RunAlignment
 * Runs alignment for RSA via device API
 */
static bool RunAlignment()
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
	RSA_API::run_mode run_mode;
	RSA_API::GetRunState(&run_mode);
	RSA_API::IQSTREAM_Stop(); //stop streaming/writing to output file
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

static int get_iq_output(gr_complex *iq_block, int block_len_req)
{
	if (queue_iq.size() < block_len_req)
	{
		//printf("~");
	}
	else
	{
		boost::mutex::scoped_lock lock(cb_mutex);
		RSA_API::Cplx32 data_val;
		for (int i=0; i < block_len_req; i++)
		{
			data_val = queue_iq.front();
			iq_block[i].real() = data_val.i;
			iq_block[i].imag() = data_val.q;
			queue_iq.pop();
		}
	}
	return block_len_req;
}

static int get_iq_data(void *p_client_buffer)
{
	uint32_t  acq_status = 0;
	int ret_len;
	RSA_API::IQSTRMIQINFO iq_info;
	RSA_API::ReturnStatus rs;
	int out_size = 0;

	//for RSA error returning
	unsigned int over_range   = 0x1;
	unsigned int data_disc   = 0x2;
	unsigned int heavy_load   = 0x4;
	unsigned int overload   = 0x8;
	unsigned int behind   = 0x10;
	unsigned int data_loss   = 0x20;

	rs = RSA_API::DEVICE_Run();         // Start device running
	if (!return_check("DEVICE_Run", rs))
	{
		printf("fail@DEVICE_Run\n");
	}

	rs = RSA_API::IQSTREAM_Start();
	if (!return_check("IQSTREAM_Start", rs))
	{
		printf("fail@IQSTREAM_Start\n");
	}

	while(true)
	{
		rs = RSA_API::IQSTREAM_GetIQData(p_client_buffer,
						   &ret_len, &iq_info);
		if (!return_check("IQSTREAM_getIQData", rs))
		{
			printf("Failed@GetIQData::IQSTREAM_GetIQData\n");
		}
		boost::mutex::scoped_lock lock(cb_mutex);
		if (ret_len != 0)
		{
			//RSA buffer -> our buffer
			//printf("Getting data in GetIQData\n");
			RSA_API::Cplx32 *cp = (RSA_API::Cplx32 *)p_client_buffer;
			for (int i = 0; i < ret_len; i++)
			{
				if (queue_iq.size() >= queue_size)
				{
					//printf("+");
					//printf("FULL! (%d remain)\n", out_size);
					break;
				}
				out_size++;
				queue_iq.push(cp[i]);				
			}
		}
		if (iq_info.acqStatus != 0)
		{
			if ((iq_info.acqStatus & over_range) == over_range)
			{
				//printf("Warning! ADC input over_range.\n");
				printf("o");
			}
			if ((iq_info.acqStatus & data_disc) == data_disc)
			{
				//printf("Warning! USB data stream discontinuity.\n");
				printf("d");
			}
			if ((iq_info.acqStatus & heavy_load) == heavy_load)
			{
				//printf("Warning! Input buffer overflow
				//(IQStream processing heavily loaded).\n");
				printf("h");
			}
			if ((iq_info.acqStatus & overload) == overload)
			{
				//printf("Warning! USB frame transfer error detected 
				//during acquisition.\n");
				printf("v");
			}
			if ((iq_info.acqStatus & behind) == behind)
			{
				//printf("Warning! Output buffer>75% full (Client falling
				// behind unloading data)\n");
				printf("b");
			}
			if ((iq_info.acqStatus & data_loss) == data_loss)
			{
				//printf("Warning! Output buffer overflow (Client unloading
				// data too slow, data loss has occurred)\n");
				printf("l");
			}
		}
	}
	//printf("Got out of thread loop!\n");
	return -1;
}

void iq_stream_impl::set_cf(float in_ctr_freq)
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

void iq_stream_impl::set_rl(float in_ref_lvl)
{
	RSA_API::ReturnStatus rs;
	double ref_lvl = (double)in_ref_lvl;
	
	rs = RSA_API::CONFIG_SetReferenceLevel(ref_lvl);
	if (!return_check("CONFIG_SetReferenceLevel", rs))
	{	
		return;
	}
	return;	
}

void iq_stream_impl::set_bw(float in_bw)
{
	RSA_API::ReturnStatus rs;
	double bw = (double)in_bw;

	rs = RSA_API::IQSTREAM_Stop();
	if(!return_check("IQSTREAM_Stop", rs))
	{
		printf("Fail Stream Stop\n");
	}

	rs = RSA_API::DEVICE_Stop();
	if(!return_check("DEVICE_Stop", rs))
	{
		printf("Fail Device Stop\n");
	}

	rs = RSA_API::IQSTREAM_SetAcqBandwidth(bw);
	if (!return_check("IQSTREAM_SetAcqBandwidth", rs))
	{
		double bw_Act, sr_samples_per_second;
		// THIS FUNCTION MUST BE CALLED to make output work!!
		rs = RSA_API::IQSTREAM_GetAcqParameters(&bw_Act,
							&sr_samples_per_second);
		if (!return_check("IQSTREAM_GetAcqParameters", rs))
		{
			//printf("IQSTREAM_GetAcqParameters\n");
		}

		size_t bytes_per_sample = sizeof(RSA_API::CplxInt32); //.. for now
		double bytes_per_second = sr_samples_per_second * bytes_per_sample;		
		printf("New IQ Stream Settings: Acq BW:%.6f MHz | FS:", bw_Act / 1e6);
		printf("%.6f Msps | Data:", sr_samples_per_second / 1e6);
		printf("%.6fe6 Bytes/sec\n", bytes_per_second / 1e6);

		rs = RSA_API::DEVICE_Run();
		if(!return_check("DEVICE_Run", rs))
		{
			printf("Fail Device Restart\n");
		}
		rs = RSA_API::IQSTREAM_Start();
		if (!return_check("IQSTREAM_Start", rs))
		{
			printf("Fail Stream Restart\n");
		}
		double iq_SR;
		RSA_API::IQBLK_GetIQSampleRate(&iq_SR);
		printf("New IQ Sample Rate: %.6f MHz\n", iq_SR);
		return;
	}
	
	
	double bw_Act, sr_samples_per_second;
	// THIS FUNCTION MUST BE CALLED to make output work!!
	rs = RSA_API::IQSTREAM_GetAcqParameters(&bw_Act,
						&sr_samples_per_second);
	if (!return_check("IQSTREAM_GetAcqParameters", rs))
	{
		//printf("IQSTREAM_GetAcqParameters\n");
	}

	size_t bytes_per_sample = sizeof(RSA_API::CplxInt32); //.. for now
	double bytes_per_second = sr_samples_per_second * bytes_per_sample;		
	printf("New IQ Stream Settings: Acq BW:%.6f MHz | FS:", bw_Act / 1e6);
	printf("%.6f Msps | Data:", sr_samples_per_second / 1e6);
	printf("%.6fe6 Bytes/sec\n", bytes_per_second / 1e6);

	rs = RSA_API::DEVICE_Run();
	if(!return_check("DEVICE_Run", rs))
	{
		printf("failDeviceRestart2\n");
	}

	rs = RSA_API::IQSTREAM_Start();
	if (!return_check("IQSTREAM_Start", rs))
	{
		printf("failStreamRestart2\n");
	}
	return;
}

//GNU radio funcs
    iq_stream::sptr
    iq_stream::make(float in_ctr_freq, float in_ref_lvl, float in_bw,
		    int in_rec_len, int in_dev_id)
    {
      return gnuradio::get_initial_sptr
        (new iq_stream_impl(in_ctr_freq,in_ref_lvl,in_bw,in_rec_len,
			    in_dev_id));
    }

    /*
     * The private constructor,gets and sets device settings
     */
    iq_stream_impl::iq_stream_impl(float in_ctr_freq, float in_ref_lvl,
				   float in_bw, int in_rec_len, int in_dev_id)
      : gr::sync_block("iq_stream",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
	RSA_API::ReturnStatus rs;
	double ctr_freq = (double)in_ctr_freq;
	double ref_lvl = (double)in_ref_lvl;
	double bw = (double)in_bw;
	int rec_len = in_rec_len;
	int dev_id = in_dev_id;
	
	ippInit();

	//Collect and set all RSA configs
	bool reboot = false;
	if (!connect_to_hw_and_setup(dev_id, reboot))
	{
		printf("Failed on connect_to_hw_and_setup...\n");
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
		double max_BW, min_BW;
		RSA_API::IQSTREAM_GetMinAcqBandwidth(&min_BW);
		RSA_API::IQSTREAM_GetMaxAcqBandwidth(&max_BW);

		//set bw if it inits above or below limits
		if (bw < min_BW)
		{
			bw = min_BW;
			printf("Input bandwidth too low, setting to RSA minimum\n");
		}
		else if (bw > max_BW)
		{
			bw = max_BW;
			printf("Input bandwidth too high, setting to RSA maximum\n");
		}

		//set bandwidth length, now that it is safe
		rs = RSA_API::IQSTREAM_SetAcqBandwidth(bw);
		if (!return_check("IQSTREAM_SetAcqBandwidth", rs))
		{
			//printf("failed on IQBLK_SetIQBandwidth\n");
		}

		double bw_Act, sr_samples_per_second;
		// THIS FUNCTION MUST BE CALLED to make output work!!
		rs = RSA_API::IQSTREAM_GetAcqParameters(&bw_Act,
							&sr_samples_per_second);
		if (!return_check("IQSTREAM_GetAcqParameters", rs))
		{
			//printf("IQSTREAM_GetAcqParameters\n");
		}

		//set iq block length
		rs = RSA_API::IQSTREAM_SetIQDataBufferSize(rec_len);
		if (!return_check("IQSTREAM_SetIQDataBufferSize", rs))
		{
			//printf("failed on IQBLK_SetIQRecordLength\n");
		}
		queue_size = rec_len;
		//get settings as stated be device
		double out_CF, out_RL;
		RSA_API::CONFIG_GetCenterFreq(&out_CF);
		RSA_API::CONFIG_GetReferenceLevel(&out_RL);

		// Display user settings
		printf("\nIQ General Settings:\nCenter Frequency: %.6f", out_CF/1e6);
		printf(" MHz\nReference Level: %.6f", out_RL);
		printf(" dB\nBuffer Size: %d\n", rec_len);
		
		size_t bytes_per_sample = sizeof(RSA_API::CplxInt32);

		double bytes_per_second = sr_samples_per_second * bytes_per_sample;		
		printf("\nIQ Stream Settings:\nAcq BW:%.6f MHz\nFS:", bw_Act / 1e6);
		printf("%.6f Msps\nData:", sr_samples_per_second / 1e6);
		printf("%.6fe6 Bytes/sec\n", bytes_per_second / 1e6);

		//set output dest
		rs = RSA_API::IQSTREAM_SetOutputConfiguration(RSA_API::IQSOD_CLIENT,
							      RSA_API::IQSODT_SINGLE);
		if (!return_check("IQSTREAM_SetOutputConfiguration", rs))
		{
			//printf("IQSTREAM_SetOutputConfiguration\n");
		}

		void* p_client_buffer = NULL;
		int req_buffer_size = 1; // 1=request minimum size, 0=set to default
		int get_buffer_size;
		// request size for API to return
		rs = RSA_API::IQSTREAM_SetIQDataBufferSize(req_buffer_size);
		if (!return_check("IQSTREAM_SetIQDataBufferSize", rs))
		{
			//printf("IQSTREAM_SetIQDataBufferSize\n");
		}

		// size API will return
		rs = RSA_API::IQSTREAM_GetIQDataBufferSize(&get_buffer_size);
		if (!return_check("IQSTREAM_GetIQDataBufferSize", rs))
		{
			//printf("IQSTREAM_GetIQDataBufferSize\n");
		}

		size_t malloc_size = (size_t)(get_buffer_size * bytes_per_sample);
		p_client_buffer = malloc(malloc_size);

		printf("\nStartIQStream Settings:\nIQBuffer: Req:%i  Act:%i\n",
			req_buffer_size, get_buffer_size);
		m_iq_stream_client_buf = p_client_buffer;

		//boost::thread getLoop(get_iq_data, m_iq_stream_client_buf);
		boost::thread thrd1(get_iq_data, m_iq_stream_client_buf);
	}
    }

    /*
     * Our virtual destructor.
     */
    iq_stream_impl::~iq_stream_impl()
    {
	stop_and_disconnect();
    }

    int
    iq_stream_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
	gr_complex *out = (gr_complex *) output_items[0];
	//int tStat = get_iq_data(m_iq_stream_client_buf);
	int num_items = get_iq_output(out,noutput_items);
	//printf("Queue Size: %d\n",queue_iq.size());
        return num_items;
    }

  } /* namespace rsa306 */
} /* namespace gr */

