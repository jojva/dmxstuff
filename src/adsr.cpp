#include "adsr.h"

CADSR::CADSR(int attack, int decay, int sustain, int release) :
    m_attack(ms(attack)),
    m_decay(ms(decay)),
    m_sustain_percentage(sustain),
    m_release(ms(release))
{
}

CADSR::CADSR(int attack, int decay, int sustain, int release, int max_velocity) :
    m_attack(ms(attack)),
    m_decay(ms(decay)),
    m_sustain_percentage(sustain),
    m_release(ms(release))
{
    ApplyMaxVelocity(max_velocity);
}

void CADSR::ApplyMaxVelocity(int max_velocity)
{
    m_sustain_absolute = (max_velocity * m_sustain_percentage) / 100;
}

ms CADSR::A(void)
{
    return m_attack;
}

ms CADSR::D(void)
{
    return m_decay;
}

int CADSR::S(void)
{
    return m_sustain_absolute;
}

ms CADSR::R(void)
{
    return m_release;
}

ms CADSR::AD(void)
{
    return m_attack + m_decay;
}

ms CADSR::DR(void)
{
    return m_decay + m_release;
}

ms CADSR::ADR(void)
{
    return m_attack + m_decay + m_release;
}
