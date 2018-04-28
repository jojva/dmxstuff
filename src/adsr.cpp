#include "adsr.h"

#include <algorithm>

CADSR::CADSR(int attack, int decay, int sustain, int sustain_time_max, int release) :
    m_attack(ms(attack)),
    m_decay(ms(decay)),
    m_sustain_percentage(sustain),
    m_sustain_time_max(sustain_time_max),
    m_release(ms(release))
{
}

CADSR::CADSR(int attack, int decay, int sustain, int sustain_time_max, int release, int max_velocity) :
    m_attack(ms(attack)),
    m_decay(ms(decay)),
    m_sustain_percentage(sustain),
    m_sustain_time_max(sustain_time_max),
    m_release(ms(release))
{
    ApplyMaxVelocity(max_velocity);
}

void CADSR::ApplyMaxVelocity(int max_velocity)
{
    m_sustain_absolute = (max_velocity * m_sustain_percentage) / 100;
}

ms CADSR::A(void) const
{
    return m_attack;
}

ms CADSR::D(void) const
{
    return m_decay;
}

int CADSR::S(void) const
{
    return m_sustain_absolute;
}

int CADSR::SRelative(void) const
{
    return m_sustain_percentage;
}

ms CADSR::STimeMax(void) const
{
    return m_sustain_time_max;
}

ms CADSR::R(void) const
{
    return m_release;
}

ms CADSR::AD(void) const
{
    return A() + D();
}

ms CADSR::DR(void) const
{
    return D() + R();
}

ms CADSR::ADS(void) const
{
    return AD() + std::max(ms(0), (m_sustain_time_max - AD()));
}

ms CADSR::ADS(ms gate_time) const
{
    if(gate_time == ms(0))
    {
        return AD() + std::max(ms(0), (m_sustain_time_max - AD()));
    }
    else
    {
        return AD() + std::max(ms(0), (std::min(gate_time, m_sustain_time_max) - AD()));
    }
}

ms CADSR::ADR(void) const
{
    return A() + D() + R();
}

ms CADSR::ADSR(void) const
{
    return ADR() + std::max(ms(0), (m_sustain_time_max - AD()));
}

ms CADSR::ADSR(ms gate_time) const
{
    if(gate_time == ms(0))
    {
        return ADR() + std::max(ms(0), (m_sustain_time_max - AD()));
    }
    else
    {
        return ADR() + std::max(ms(0), (std::min(gate_time, m_sustain_time_max) - AD()));
    }
}
