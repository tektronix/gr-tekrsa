#!/usr/bin/env python2
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Wed Apr  5 16:33:03 2017
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from gnuradio import analog
from gnuradio import audio
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import TekRSA
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.sr = sr = 7e6
        self.tw = tw = 1.7e3
        self.samp_rate = samp_rate = sr
        self.rl = rl = -20
        self.co = co = 100e3
        self.cf = cf = 91.5e6
        self.bw = bw = 2e6

        ##################################################
        # Blocks
        ##################################################
        _tw_sizer = wx.BoxSizer(wx.VERTICAL)
        self._tw_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_tw_sizer,
        	value=self.tw,
        	callback=self.set_tw,
        	label='tw',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._tw_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_tw_sizer,
        	value=self.tw,
        	callback=self.set_tw,
        	minimum=0,
        	maximum=1e8,
        	num_steps=10,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_tw_sizer)
        _rl_sizer = wx.BoxSizer(wx.VERTICAL)
        self._rl_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_rl_sizer,
        	value=self.rl,
        	callback=self.set_rl,
        	label='rl',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._rl_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_rl_sizer,
        	value=self.rl,
        	callback=self.set_rl,
        	minimum=-40,
        	maximum=40,
        	num_steps=10,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_rl_sizer)
        _co_sizer = wx.BoxSizer(wx.VERTICAL)
        self._co_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_co_sizer,
        	value=self.co,
        	callback=self.set_co,
        	label='co',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._co_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_co_sizer,
        	value=self.co,
        	callback=self.set_co,
        	minimum=0,
        	maximum=1e8,
        	num_steps=10,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_co_sizer)
        _cf_sizer = wx.BoxSizer(wx.VERTICAL)
        self._cf_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_cf_sizer,
        	value=self.cf,
        	callback=self.set_cf,
        	label='cf',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._cf_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_cf_sizer,
        	value=self.cf,
        	callback=self.set_cf,
        	minimum=3,
        	maximum=1e10,
        	num_steps=100,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_cf_sizer)
        _bw_sizer = wx.BoxSizer(wx.VERTICAL)
        self._bw_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_bw_sizer,
        	value=self.bw,
        	callback=self.set_bw,
        	label='bw',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._bw_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_bw_sizer,
        	value=self.bw,
        	callback=self.set_bw,
        	minimum=2e6,
        	maximum=40e6,
        	num_steps=10,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_bw_sizer)
        _sr_sizer = wx.BoxSizer(wx.VERTICAL)
        self._sr_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_sr_sizer,
        	value=self.sr,
        	callback=self.set_sr,
        	label='sr',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._sr_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_sr_sizer,
        	value=self.sr,
        	callback=self.set_sr,
        	minimum=0,
        	maximum=20e8,
        	num_steps=10,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_sr_sizer)
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=1000,
                decimation=500,
                taps=None,
                fractional_bw=None,
        )
        self.low_pass_filter_0 = filter.fir_filter_ccf(20, firdes.low_pass(
        	1, samp_rate, co, tw, firdes.WIN_HAMMING, 6.76))
        self.blocks_throttle_1 = blocks.throttle(gr.sizeof_float*1, 44.1e3,True)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((1, ))
        self.audio_sink_0 = audio.sink(44100, "", True)
        self.analog_wfm_rcv_0 = analog.wfm_rcv(
        	quad_rate=705e3,
        	audio_decimation=15,
        )
        self.TekRSA_iq_stream_0_0 = TekRSA.iq_stream(cf, rl, bw, 1000000, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.TekRSA_iq_stream_0_0, 0), (self.blocks_throttle_0, 0))    
        self.connect((self.analog_wfm_rcv_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_throttle_1, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.low_pass_filter_0, 0))    
        self.connect((self.blocks_throttle_1, 0), (self.audio_sink_0, 0))    
        self.connect((self.low_pass_filter_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.analog_wfm_rcv_0, 0))    


    def get_sr(self):
        return self.sr

    def set_sr(self, sr):
        self.sr = sr
        self.set_samp_rate(self.sr)
        self._sr_slider.set_value(self.sr)
        self._sr_text_box.set_value(self.sr)

    def get_tw(self):
        return self.tw

    def set_tw(self, tw):
        self.tw = tw
        self._tw_slider.set_value(self.tw)
        self._tw_text_box.set_value(self.tw)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.co, self.tw, firdes.WIN_HAMMING, 6.76))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.co, self.tw, firdes.WIN_HAMMING, 6.76))

    def get_rl(self):
        return self.rl

    def set_rl(self, rl):
        self.rl = rl
        self._rl_slider.set_value(self.rl)
        self._rl_text_box.set_value(self.rl)
        self.TekRSA_iq_stream_0_0.set_rl(self.rl)

    def get_co(self):
        return self.co

    def set_co(self, co):
        self.co = co
        self._co_slider.set_value(self.co)
        self._co_text_box.set_value(self.co)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.co, self.tw, firdes.WIN_HAMMING, 6.76))

    def get_cf(self):
        return self.cf

    def set_cf(self, cf):
        self.cf = cf
        self._cf_slider.set_value(self.cf)
        self._cf_text_box.set_value(self.cf)
        self.TekRSA_iq_stream_0_0.set_cf(self.cf)

    def get_bw(self):
        return self.bw

    def set_bw(self, bw):
        self.bw = bw
        self._bw_slider.set_value(self.bw)
        self._bw_text_box.set_value(self.bw)
        self.TekRSA_iq_stream_0_0.set_bw(self.bw)


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = top_block()
    tb.Start(True)
    tb.Wait()
