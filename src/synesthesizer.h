#pragma once

#include "k8062.h"
#include "channel.h"

#include <alsa/asoundlib.h>

#define MAX_CHANNELS    12

class MainWindow;

class CSynesthesizer
{
public:
    CSynesthesizer(void);
    ~CSynesthesizer(void);

    void Init(int argc, char *argv[]);
    void Run(void);

private:
    void ReadSettings(void);
    void InitDMX(void);
    void InitASeqDump(int argc, char *argv[]);
    void ExitASeqDump(void);

    void UpdateChannels(void);
    void HandleMidiEvent(const snd_seq_event_t *ev);
    void SendDmx(int channel);

private:
    CADSR               m_adsr;
    k8062_client        m_dmx;
    CChannel            m_channels[MAX_CHANNELS];
    struct pollfd*      m_pfds;
    int                 m_npfds;
    bool                m_sustain_pedal_on;
};
