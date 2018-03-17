#include <stdio.h>
#include <stdlib.h>

#include "k8062.h"

extern "C"
{
#include "aseqdump.h"
}

#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>

using namespace std::chrono;

#define MAX_CHANNELS            12
#define DELAY_SUSTAIN_RELEASE   80

static struct pollfd *pfds;
static int npfds;
static BYTE dmx_channels[MAX_CHANNELS];
static bool dmx_channels_release[MAX_CHANNELS];

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

    // By default, read port 20
    parse_ports("20");

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

void update_channels(k8062_client& dmx)
{
    static milliseconds ms_last = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    milliseconds ms_now = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    auto diff = duration_cast<milliseconds>(ms_now - ms_last);
    milliseconds update_delay(50);
    if(diff > update_delay)
    {
        for(int channel = 0; channel < MAX_CHANNELS; channel++)
        {
            if(dmx_channels_release[channel])
            {
                dmx_channels[channel] = std::max(0, dmx_channels[channel] - 10);
                dmx.set_channel(channel, dmx_channels[channel]);
            }
        }
        ms_last += milliseconds(update_delay);
    }
}

void handle_event(const snd_seq_event_t *ev, k8062_client& dmx)
{
    printf("%3d:%-3d ", ev->source.client, ev->source.port);
    switch (ev->type) {
    case SND_SEQ_EVENT_NOTEON:
    {
        // Assign each note to a channel number. Since there are 12 midi notes, we compute the channel by modulo 12.
        int channel = ev->data.note.note % 12;
        dmx_channels_release[channel] = false;
        if (ev->data.note.velocity)
        {
            printf("Note on                %2d, note %d, velocity %d\n",
                   ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
            printf("Let there be lights! With velocity!\n");
            // MIDI velocity is in the range 0-127, we multiply it by 2 to get it in the range 0-254 of DMX
            dmx_channels[channel] = (BYTE)(ev->data.note.velocity * 2);
            dmx.set_channel(channel, dmx_channels[channel]);
        }
        else
        {
            printf("Note off               %2d, note %d\n",
                   ev->data.note.channel, ev->data.note.note);
            printf("Let there be lights!\n");
            dmx.set_channel(channel, 254);
        }
        break;
    }
    case SND_SEQ_EVENT_NOTEOFF:
    {
        printf("Note off               %2d, note %d, velocity %d\n",
               ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
        // Assign each note to a channel number. Since there are 12 midi notes, we compute the channel by modulo 12.
        int channel = ev->data.note.note % 12;
        dmx_channels_release[channel] = true;
        break;
    }
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

void run(k8062_client& dmx)
{
    for (;;) {
        snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
        if (poll(pfds, npfds, -1) < 0)
            break;
        int err;
        do {
            update_channels(dmx);
            snd_seq_event_t *event;
            err = snd_seq_event_input(seq, &event);
            if (err < 0)
                break;
            if (event)
                handle_event(event, dmx);
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
    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        dmx_channels[i] = 0;
        dmx_channels_release[i] = false;
    }
    // Init MIDI interface
    init_aseqdump(argc, argv);
    // Run software
    run(dmx);
    // Destroy MIDI interface
    close_aseqdump();
    return 0;
}
