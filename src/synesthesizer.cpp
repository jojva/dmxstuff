#include "synesthesizer.h"

extern "C"
{
#include "aseqdump.h"
}

#include <SDL2/SDL.h>

#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <sys/poll.h>

using namespace std::chrono;

#define DELAY_SUSTAIN_RELEASE   80

CSynesthesizer::CSynesthesizer(void) :
    m_dmx(),
    m_pfds(NULL),
    m_npfds(0)
{
    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        m_velocity[i] = 0;
        m_release[i] = false;
    }
}

CSynesthesizer::~CSynesthesizer(void)
{
    CloseASeqDump();
}

void CSynesthesizer::Init(int argc, char *argv[])
{
    InitDMX();
    InitASeqDump(argc, argv);
}

void CSynesthesizer::InitDMX(void)
{
    if(!m_dmx.is_connected()){
        puts("Error: Unable to connect to dmx daemon.\n");
        exit(1);
    }
}

void CSynesthesizer::InitASeqDump(int argc, char *argv[])
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

    m_npfds = snd_seq_poll_descriptors_count(seq, POLLIN);
    m_pfds = (struct pollfd *)alloca(sizeof(*m_pfds) * m_npfds);
}

void CSynesthesizer::CloseASeqDump(void)
{
    snd_seq_close(seq);
}

void CSynesthesizer::Run(void)
{
    for (;;) {
        snd_seq_poll_descriptors(seq, m_pfds, m_npfds, POLLIN);
        if (poll(m_pfds, m_npfds, -1) < 0)
            break;
        int err;
        do {
            UpdateChannels();
            snd_seq_event_t *event;
            err = snd_seq_event_input(seq, &event);
            if (err < 0)
                break;
            if (event)
                HandleEvent(event);
        } while (err > 0);
        fflush(stdout);
        if (stop)
            break;
    }
}

void CSynesthesizer::UpdateChannels(void)
{
    static milliseconds ms_last = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    milliseconds ms_now = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    auto diff = duration_cast<milliseconds>(ms_now - ms_last);
    milliseconds update_delay(50);
    // Every 50 milliseconds...
    if(diff > update_delay)
    {
        // For each channel...
        for(int channel = 0; channel < MAX_CHANNELS; channel++)
        {
            // If it's being released (after a Note Off)...
            if(m_release[channel])
            {
                // Decrease the DMX velocity by 10, to a lower bound of 0
                m_velocity[channel] = std::max(0, m_velocity[channel] - 10);
                SendDmx((BYTE)channel);
            }
        }
        ms_last += milliseconds(update_delay);
    }
}

void CSynesthesizer::HandleEvent(const snd_seq_event_t *ev)
{
    printf("%3d:%-3d ", ev->source.client, ev->source.port);
    switch (ev->type) {
    case SND_SEQ_EVENT_NOTEON:
    {
        // Assign each note to a channel number. Since there are 12 midi notes, we compute the channel by modulo 12.
        int channel = ev->data.note.note % 12;
        m_release[channel] = false;
        if (ev->data.note.velocity)
        {
            printf("Note on                %2d, note %d, velocity %d\n",
                   ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
            // MIDI velocity is in the range 0-127, we multiply it by 2 to get it in the range 0-254 of DMX
//            m_velocity[channel] = (BYTE)(ev->data.note.velocity * 2);
            m_velocity[channel] = (BYTE)254;
            SendDmx((BYTE)channel);
        }
        else
        {
            printf("Note off               %2d, note %d\n",
                   ev->data.note.channel, ev->data.note.note);
            m_release[channel] = true;
        }
        break;
    }
    case SND_SEQ_EVENT_NOTEOFF:
    {
        printf("Note off               %2d, note %d, velocity %d\n",
               ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
        // Assign each note to a channel number. Since there are 12 midi notes, we compute the channel by modulo 12.
        int channel = ev->data.note.note % 12;
        m_release[channel] = true;
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

void CSynesthesizer::SendDmx(BYTE channel)
{
    printf("Sending velocity %d to channel %d\n", m_velocity[channel], channel);
    m_dmx.set_channel(channel, m_velocity[channel]);
}
