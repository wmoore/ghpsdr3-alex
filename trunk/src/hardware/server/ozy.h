/**
* @file ozy.h
* @brief Ozy protocol implementation
* @author John Melton, G0ORX/N6LYT
* @version 0.1
* @date 2009-10-13
*/

/* Copyright (C)
* 2009 - John Melton, G0ORX/N6LYT
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#ifndef __OZY_H__
#define __OZY_H__

int ozy_init(void);
void ozy_set_fpga_image(const char *);
void ozy_set_hex_image(const char *);
void ozy_prime();
void ozy_set_buffers(int buffers, int hermes);
void ozy_set_class(int c);
int create_ozy_thread();
void ozy_set_receivers(int r);
int ozy_get_receivers();
void ozy_set_sample_rate(int r);
int ozy_get_sample_rate();
void ozy_set_preamp(int p);
void ozy_set_dither(int dither);
void ozy_set_random(int random);
void ozy_set_10mhzsource(int source);
void ozy_set_122_88mhzsource(int source);
void ozy_set_micsource(int source);
int  ozy_get_adc_overflow (void);

void ozy_set_timing(int t);

void ozy_set_record(char* filename);
void ozy_set_playback(char* filename);
int ozy_set_playback_sleep(int sleep);
void ozy_stop_record();


void process_ozy_input_buffer(unsigned char* buffer);

void write_ozy_output_buffer_metis(void);
void write_ozy_output_buffer_hermes(void);

void process_bandscope_buffer(unsigned char* buffer);
void process_ozy_output_buffer(float *left_output_buffer,float *right_output_buffer,
                               float *left_tx_buffer,float *right_tx_buffer,int mox_state);

void ozy_set_metis(int state);
void ozy_set_hermes(int state);

void ozy_set_open_collector_outputs(int oc);
void ozy_set_hermes_power(unsigned char p);
void ozy_set_hermes_lineingain(unsigned char p);
void ozy_set_hermes_linein(unsigned char p);
void ozy_set_hermes_att(int p);
void ozy_set_hermes_mic_boost(int p);

int ozy_get_hermes_sw_ver(void);
void ozy_set_alex_rx_att (unsigned int a);
void ozy_set_alex_rx_antenna (unsigned int a);
void ozy_set_alex_tx_relay (unsigned int t);

#endif
