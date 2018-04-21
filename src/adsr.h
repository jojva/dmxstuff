#pragma once

#include <chrono>

typedef std::chrono::milliseconds ms;

class CADSR
{
public:
    CADSR(int attack, int decay, int sustain, int release);
    CADSR(int attack, int decay, int sustain, int release, int max_velocity);

    void ApplyMaxVelocity(int max_velocity);

    ms A(void) const;
    ms D(void) const;
    int S(void) const;
    int SRelative(void) const;
    ms R(void) const;
    ms AD(void) const;
    ms DR(void) const;
    ms ADS(ms gate_time) const;
    ms ADR(void) const;
    ms ADSR(ms gate_time) const;

private:
    ms  m_attack;
    ms  m_decay;
    int m_sustain_percentage;
    int m_sustain_absolute;
    ms  m_release;
};
