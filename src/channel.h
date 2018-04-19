#pragma once

#include <chrono>

typedef std::chrono::milliseconds ms;

class CChannel
{
public:
    struct SADSR
    {
        ms  attack;
        ms  decay;
        int sustain;
        ms  release;
    };

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

    void NoteOn(const SADSR &adsr, int max_velocity);
    void NoteOff(void);
    int ComputeVelocity(void);
    void ComputePhase(EPhase& phase, int& progress_percentage);

private:
    SADSR   m_adsr;
    int     m_max_velocity;
    ms      m_trigger_time;
};
