#pragma once

#include "k8062.h"
#include "channel.h"

#include <alsa/asoundlib.h>

#define MAX_CHANNELS    12

class MainWindow;

class CSynesthesizer
{
public:
    CSynesthesizer(const MainWindow& mainwindow);
    ~CSynesthesizer(void);

    void Init(int argc, char *argv[]);
    void Run(void);

private:
    void InitDMX(void);
    void InitASeqDump(int argc, char *argv[]);
    void ExitASeqDump(void);

    void UpdateChannels(void);
    void HandleMidiEvent(const snd_seq_event_t *ev);
    void SendDmx(int channel);

private:
    const MainWindow&   m_mainwindow;
    k8062_client        m_dmx;
    CChannel            m_channels[MAX_CHANNELS];
    struct pollfd*      m_pfds;
    int                 m_npfds;
    bool                m_sustain_pedal_on;
};
