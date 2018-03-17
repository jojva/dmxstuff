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

#include <alsa/asoundlib.h>
#include <signal.h>

extern snd_seq_t *seq;
extern int port_count;
extern snd_seq_addr_t *ports;
extern volatile sig_atomic_t stop;

void fatal(const char *msg, ...);
void check_mem(void *p);
void check_snd(const char *operation, int err);
void init_seq(void);
void parse_ports(const char *arg);
void create_port(void);
void connect_ports(void);
void dump_event(const snd_seq_event_t *ev);
void list_ports(void);
void help(const char *argv0);
void sighandler(int sig);
