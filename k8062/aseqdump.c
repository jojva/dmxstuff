/*
 * aseqdump.c - show the events received at an ALSA sequencer port
 *
 * Copyright (c) 2005 Clemens Ladisch <clemens@ladisch.de>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "aseqdump.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>

snd_seq_t *seq;
int port_count;
snd_seq_addr_t *ports;
volatile sig_atomic_t stop = 0;

/* prints an error message to stderr, and dies */
void fatal(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

/* memory allocation error handling */
void check_mem(void *p)
{
	if (!p)
		fatal("Out of memory");
}

/* error handling for ALSA functions */
void check_snd(const char *operation, int err)
{
	if (err < 0)
		fatal("Cannot %s - %s", operation, snd_strerror(err));
}

void init_seq(void)
{
	int err;

	/* open sequencer */
	err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
	check_snd("open sequencer", err);

	/* set our client's name */
	err = snd_seq_set_client_name(seq, "aseqdump");
	check_snd("set client name", err);
}

/* parses one or more port addresses from the string */
void parse_ports(const char *arg)
{
	char *buf, *s, *port_name;
	int err;

	/* make a copy of the string because we're going to modify it */
	buf = strdup(arg);
	check_mem(buf);

	for (port_name = s = buf; s; port_name = s + 1) {
		/* Assume that ports are separated by commas.  We don't use
		 * spaces because those are valid in client names. */
		s = strchr(port_name, ',');
		if (s)
			*s = '\0';

		++port_count;
		ports = realloc(ports, port_count * sizeof(snd_seq_addr_t));
		check_mem(ports);

		err = snd_seq_parse_address(seq, &ports[port_count - 1], port_name);
		if (err < 0)
			fatal("Invalid port %s - %s", port_name, snd_strerror(err));
	}

	free(buf);
}

void create_port(void)
{
	int err;

	err = snd_seq_create_simple_port(seq, "aseqdump",
					 SND_SEQ_PORT_CAP_WRITE |
					 SND_SEQ_PORT_CAP_SUBS_WRITE,
					 SND_SEQ_PORT_TYPE_MIDI_GENERIC |
					 SND_SEQ_PORT_TYPE_APPLICATION);
	check_snd("create port", err);
}

void connect_ports(void)
{
	int i, err;

	for (i = 0; i < port_count; ++i) {
		err = snd_seq_connect_from(seq, 0, ports[i].client, ports[i].port);
		if (err < 0)
			fatal("Cannot connect from port %d:%d - %s",
			      ports[i].client, ports[i].port, snd_strerror(err));
	}
}

void dump_event(const snd_seq_event_t *ev)
{
	printf("%3d:%-3d ", ev->source.client, ev->source.port);
	switch (ev->type) {
	case SND_SEQ_EVENT_NOTEON:
		if (ev->data.note.velocity)
			printf("Note on                %2d, note %d, velocity %d\n",
			       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		else
			printf("Note off               %2d, note %d\n",
			       ev->data.note.channel, ev->data.note.note);
		break;
	case SND_SEQ_EVENT_NOTEOFF:
		printf("Note off               %2d, note %d, velocity %d\n",
		       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		break;
	case SND_SEQ_EVENT_KEYPRESS:
		printf("Polyphonic aftertouch  %2d, note %d, value %d\n",
		       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		break;
	case SND_SEQ_EVENT_CONTROLLER:
		printf("Control change         %2d, controller %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_PGMCHANGE:
		printf("Program change         %2d, program %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_CHANPRESS:
		printf("Channel aftertouch     %2d, value %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_PITCHBEND:
		printf("Pitch bend             %2d, value %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_CONTROL14:
		printf("Control change         %2d, controller %d, value %5d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_NONREGPARAM:
		printf("Non-reg. parameter     %2d, parameter %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_REGPARAM:
		printf("Reg. parameter         %2d, parameter %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_SONGPOS:
		printf("Song position pointer      value %d\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_SONGSEL:
		printf("Song select                value %d\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_QFRAME:
		printf("MTC quarter frame          %02xh\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_TIMESIGN:
		// XXX how is this encoded?
		printf("SMF time signature         (%#010x)\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_KEYSIGN:
		// XXX how is this encoded?
		printf("SMF key signature          (%#010x)\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_START:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue start                queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Start\n");
		break;
	case SND_SEQ_EVENT_CONTINUE:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue continue             queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Continue\n");
		break;
	case SND_SEQ_EVENT_STOP:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue stop                 queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Stop\n");
		break;
	case SND_SEQ_EVENT_SETPOS_TICK:
		printf("Set tick queue pos.        queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_SETPOS_TIME:
		printf("Set rt queue pos.          queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_TEMPO:
		printf("Set queue tempo            queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_CLOCK:
		printf("Clock\n");
		break;
	case SND_SEQ_EVENT_TICK:
		printf("Tick\n");
		break;
	case SND_SEQ_EVENT_QUEUE_SKEW:
		printf("Queue timer skew           queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_TUNE_REQUEST:
		printf("Tune request\n");
		break;
	case SND_SEQ_EVENT_RESET:
		printf("Reset\n");
		break;
	case SND_SEQ_EVENT_SENSING:
		printf("Active Sensing\n");
		break;
	case SND_SEQ_EVENT_CLIENT_START:
		printf("Client start               client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_CLIENT_EXIT:
		printf("Client exit                client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_CLIENT_CHANGE:
		printf("Client changed             client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_PORT_START:
		printf("Port start                 %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_EXIT:
		printf("Port exit                  %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_CHANGE:
		printf("Port changed               %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_SUBSCRIBED:
		printf("Port subscribed            %d:%d -> %d:%d\n",
		       ev->data.connect.sender.client, ev->data.connect.sender.port,
		       ev->data.connect.dest.client, ev->data.connect.dest.port);
		break;
	case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
		printf("Port unsubscribed          %d:%d -> %d:%d\n",
		       ev->data.connect.sender.client, ev->data.connect.sender.port,
		       ev->data.connect.dest.client, ev->data.connect.dest.port);
		break;
	case SND_SEQ_EVENT_SYSEX:
		{
			unsigned int i;
			printf("System exclusive          ");
			for (i = 0; i < ev->data.ext.len; ++i)
				printf(" %02X", ((unsigned char*)ev->data.ext.ptr)[i]);
			printf("\n");
		}
		break;
	default:
		printf("Event type %d\n",  ev->type);
	}
}

void list_ports(void)
{
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;

	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);

	puts(" Port    Client name                      Port name");

	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(seq, cinfo) >= 0) {
		int client = snd_seq_client_info_get_client(cinfo);

		snd_seq_port_info_set_client(pinfo, client);
		snd_seq_port_info_set_port(pinfo, -1);
		while (snd_seq_query_next_port(seq, pinfo) >= 0) {
			/* we need both READ and SUBS_READ */
			if ((snd_seq_port_info_get_capability(pinfo)
			     & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
			    != (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
				continue;
			printf("%3d:%-3d  %-32.32s %s\n",
			       snd_seq_port_info_get_client(pinfo),
			       snd_seq_port_info_get_port(pinfo),
			       snd_seq_client_info_get_name(cinfo),
			       snd_seq_port_info_get_name(pinfo));
		}
	}
}

void help(const char *argv0)
{
	printf("Usage: %s [options]\n"
		"\nAvailable options:\n"
		"  -h,--help                  this help\n"
		"  -l,--list                  list input ports\n"
		"  -p,--port=client:port,...  source port(s)\n",
		argv0);
}

void sighandler(int sig)
{
	stop = 1;
}
