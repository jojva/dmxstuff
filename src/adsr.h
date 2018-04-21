#pragma once

#include <chrono>

typedef std::chrono::milliseconds ms;

class CADSR
{
public:
    CADSR(int attack, int decay, int sustain, int release);
    CADSR(int attack, int decay, int sustain, int release, int max_velocity);

    void ApplyMaxVelocity(int max_velocity);

    ms A(void);
    ms D(void);
    int S(void);
    ms R(void);
    ms AD(void);
    ms DR(void);
    ms ADR(void);

private:
    ms  m_attack;
    ms  m_decay;
    int m_sustain_percentage;
    int m_sustain_absolute;
    ms  m_release;
};
