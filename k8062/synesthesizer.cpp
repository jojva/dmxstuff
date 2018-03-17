#include <stdio.h>
#include <stdlib.h>

#include "k8062.h"

extern "C"
{
#include "aseqdump.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>

struct pollfd *pfds;
int npfds;

void init_aseqdump(int argc, char *argv[])
{
    static const char short_options[] = "hVlp:";
    static const struct option long_options[] = {
    {"help", 0, NULL, 'h'},
    {"list", 0, NULL, 'l'},
    {"port", 1, NULL, 'p'},
    { }};

    int do_list = 0;
    int c, err;

    init_seq();

    while ((c = getopt_long(argc, argv, short_options,
                            long_options, NULL)) != -1) {
        switch (c) {
        case 'h':
            help(argv[0]);
            exit(0);
        case 'l':
            do_list = 1;
            break;
        case 'p':
            parse_ports(optarg);
            break;
        default:
            help(argv[0]);
            exit(1);
        }
    }
    if (optind < argc) {
        help(argv[0]);
        exit(1);
    }

    if (do_list) {
        list_ports();
        exit(0);
    }

    create_port();
    connect_ports();

    err = snd_seq_nonblock(seq, 1);
    check_snd("set nonblock mode", err);

    if (port_count > 0)
        printf("Waiting for data.");
    else
        printf("Waiting for data at port %d:0.",
               snd_seq_client_id(seq));
    printf(" Press Ctrl+C to end.\n");
    printf("Source  Event                  Ch  Data\n");

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    npfds = snd_seq_poll_descriptors_count(seq, POLLIN);
    pfds = (struct pollfd *)alloca(sizeof(*pfds) * npfds);
}

void close_aseqdump(void)
{
    snd_seq_close(seq);
}

void run(k8062_client& dmx)
{
    // Partie récupérée de main whiteout
    int max_channels = 16;
    for(int cur_channel = 0; cur_channel < max_channels; cur_channel++){
        dmx.set_channel((BYTE)cur_channel,254);
    }
    // Partie récupérée de main aseqdump
    for (;;) {
        snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
        if (poll(pfds, npfds, -1) < 0)
            break;
        int err;
        do {
            snd_seq_event_t *event;
            err = snd_seq_event_input(seq, &event);
            if (err < 0)
                break;
            if (event)
                dump_event(event);
        } while (err > 0);
        fflush(stdout);
        if (stop)
            break;
    }
}

int main(int argc,char *argv[])
{
    // Init DMX interface
    k8062_client dmx;
    if(!dmx.is_connected()){
        puts("Error: Unable to connect to dmx daemon.\n");
        exit(1);
    }
    // Init MIDI interface
    init_aseqdump(argc, argv);
    // Run software
    run(dmx);
    // Destroy MIDI interface
    close_aseqdump();
    return 0;
}
