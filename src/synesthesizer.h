#include "k8062.h"

#include <alsa/asoundlib.h>
#include <SDL2/SDL.h>

#define MAX_CHANNELS    12

class CSynesthesizer
{
public:
    CSynesthesizer(void);
    ~CSynesthesizer(void);

    void Init(int argc, char *argv[]);
    void Run(void);

private:
    void InitDMX(void);
    void InitASeqDump(int argc, char *argv[]);
    void InitSDL(void);
    void CloseASeqDump(void);

    void UpdateChannels(void);
    void HandleEvent(const snd_seq_event_t *ev);
    void SendDmx(BYTE channel);

private:
    SDL_Window*     m_window;
    SDL_Surface*    m_screenSurface;
    k8062_client    m_dmx;
    struct pollfd*  m_pfds;
    int             m_npfds;
    BYTE            m_velocity[MAX_CHANNELS];
    bool            m_release[MAX_CHANNELS];
};
