#pragma once

#include <chrono>

struct SADSR;

typedef std::chrono::milliseconds ms;

class CChannel
{
public:
    enum EPhase {
        BEFORE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE,
        AFTER
    };

public:
    CChannel(void);

    void NoteOn(int velocity);
    void NoteOff(void);
    int ComputeVelocity(const SADSR &adsr);
    void ComputePhase(const SADSR& adsr);

private:
    int     m_max_velocity;
    ms      m_trigger_time;
    EPhase  m_phase;
    int     m_progress_percentage;
};
