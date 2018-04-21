#pragma once

#include "adsr.h"

#include <chrono>

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

    void NoteOn(CADSR *adsr, int max_velocity);
    void NoteOff(void);
    int ComputeVelocity(void);
    void ComputePhase(EPhase& phase, double &progress_percentage);

private:
    CADSR*  m_adsr;
    int     m_max_velocity;
    ms      m_trigger_time;
};
