/**
* @file server.c
* @brief HPSDR server application
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
* This file contains the main program and the command line processing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

#include <usb.h>

#include "client.h"
#include "listener.h"
#include "softrock.h"
#include "receiver.h"
#include "operations.h"

void usage(void);

static struct option long_options[] = {
    {"samplerate",required_argument, 0, 's'},
    {"device",required_argument, 0, 'd'},
    {"input",required_argument, 0, 'i'},
    {"output",required_argument, 0, 'o'},
    {"iq",no_argument, 0, 4},
    {"qi",no_argument, 0, 5},
    {"si570",no_argument, 0, 6},
    {"verbose",no_argument, 0, 'v'},
    {"startupfreq",required_argument, 0, 'f'},
    {"multiplier",required_argument, 0, 'm'},
    {"correctedfreq",required_argument, 0, 10},
    {"serialid",required_argument, 0, 11},
    {"record",required_argument, 0, 'r'},
    {"playback",required_argument, 0, 'p'},
	{"receivers",required_argument, 0, 14},
	{"jack",no_argument, 0, 'j'},

    {"peaberry",no_argument, 0, 16},
	{0, 0, 0, 0}
};

static char* short_options="s:d:i:o:vf:m:r:p:j";
static int option_index;

int si570=0;
double startupFreq=56.32;
double multiplier=4;
int i2cAddress = 0x55;
double fXtall = 114.285;
char* usbSerialID=NULL;
char* unitName = "DG8SAQ-I2C";
int setByValue = 1;

usb_dev_handle* handle = NULL;

void process_args(int argc,char* argv[]);

int main(int argc,char* argv[]) {

    int rc;
#ifdef DIRECTAUDIO
    softrock_set_device("/dev/dsp");
#endif

    process_args(argc,argv);

    if(si570) {
        usb_init();
        rc=usbOpenDevice(&handle,0x16C0,"www.obdev.at",0x05DC,unitName,usbSerialID);
        if (rc!=0) rc=usbOpenDevice(&handle,0x16C0,"SDR-Widget",0x05DC,"Yoyodyne SDR-Widget",usbSerialID);
        if(rc!=0) {
            fprintf(stderr,"Cannot open USB device\n");
            exit(1);
        }
    }

    init_receivers();

    create_listener_thread();

    create_softrock_thread();

    while(1) {
        sleep(10);
    }
}

void process_args(int argc,char* argv[]) {
    int i;

    // set defaults
    softrock_set_receivers(1);
    softrock_set_sample_rate(96000);
	softrock_set_jack(0);
    //softrock_set_device("/dev/dsp");

    while((i=getopt_long(argc,argv,short_options,long_options,&option_index))!=EOF) {
        switch(i) {
            case 's': // sample rate
				if (long_options[option_index].flag != 0){
					fprintf(stderr,"invalid argument\n");
                	exit(1);
				}
				printf ("option %s", long_options[option_index].name);
               	if (optarg)
                	printf (" with arg %s", optarg);
               	printf ("\n");
				fprintf(stderr,"process_args: samplerate=%s\n",optarg);
                softrock_set_sample_rate(atoi(optarg));
                break;
            case 'd': // device
				fprintf(stderr,"process_args: device=%s\n",optarg);
                softrock_set_device(optarg);
                break;
            case 'i': // input
				fprintf(stderr,"process_args: input=%s\n",optarg);
                softrock_set_input(optarg);
                break;
            case 'o': // output
				fprintf(stderr,"process_args: output=%s\n",optarg);
                softrock_set_output(optarg);
                break;
            case 4: // iq
                softrock_set_iq(1);
                break;
            case 5: // qi
                softrock_set_iq(0);
                break;
            case 6: // si570
                si570=1;
                break;
            case 'v': // verbose
				fprintf(stderr,"Set verbose flag.\n");
                softrock_set_verbose (1);
                break;
            case 'f': // startupfreq
                startupFreq=atof(optarg);
                break;
            case 'm': // multiplier
                multiplier=atof(optarg);
                break;
            case 10: // corrected xtal freq
                fXtall=atof(optarg);
				setByValue = 0;
                break;
            case 11: // serial ID
                usbSerialID=optarg;
                break;
            case 'r': // record
                softrock_set_record(optarg);
                break;
            case 'p': // playback
                softrock_set_playback(optarg);
                break;
			case 14: // receivers
                softrock_set_receivers(atoi(optarg));
                break;
			case 'j': // jack
                softrock_set_jack(1);
                break;
	    case 16: // peaberry SDR
                unitName = "Peaberry SDR";
                break;
            default:
				fprintf(stderr,"invalid argument\n");
				usage(); 
                exit(1);
        }
    }
}

void usage( void )
{
	fprintf(stderr,"Usage options:\n");
	fprintf(stderr,"	-s= or --samplerate  \n");
	fprintf(stderr,"	-d= or --device (OSS audio device) \n");
	fprintf(stderr,"	-i= or --input (Port Audio device for input) \n");
	fprintf(stderr,"	-o= or --output (Port Audio device for output) \n");
	fprintf(stderr,"	--iq or --qi (to swap I and Q channels)  \n");
	fprintf(stderr,"	--si570 (to use a unit having a Si570 oscillater) \n");
	fprintf(stderr,"	-v or --verbose  \n");
	fprintf(stderr,"	-f= or --startupfreq (set Si570 startup frequency) \n");
	fprintf(stderr,"	-m= or --multiplier (set Si570 multiplier) \n");
	fprintf(stderr,"	--correctedfreq (set Si570 corrected frequency) \n");
	fprintf(stderr,"	--serialid  \n");
	fprintf(stderr,"	--record filename (record to this file)\n");
	fprintf(stderr,"	--playback filename (playback this file)  \n");
	fprintf(stderr,"	--receivers (number of receivers) \n");
	fprintf(stderr,"	-j or --jack (use Jack Audio Connection Kit for audio in/out)  \n");
	fprintf(stderr,"	--peaberry (to use a Peaberry SDR v2 unit) \n");
}

	        
