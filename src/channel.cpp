#include "channel.h"
#include "synesthesizer.h"

using namespace std::chrono;

CChannel::CChannel(void) :
    m_adsr(nullptr),
    m_max_velocity(0),
    m_trigger_time(0),
    m_gate_time(0),
    m_is_note_sustained(false),
    m_is_pedal_sustained(false)
{
}

void CChannel::NoteOn(CADSR* adsr, int max_velocity, bool is_pedal_sustained)
{
    ms rollback_time = ms(0);
    int current_velocity = ComputeVelocity();
    if(current_velocity > 0)
    {
        rollback_time = ms((current_velocity * m_adsr->A()) / m_max_velocity);
    }
    m_trigger_time = duration_cast<ms>(system_clock::now().time_since_epoch() - rollback_time);
    m_max_velocity = max_velocity;
    delete m_adsr;
    m_adsr = adsr;
    adsr->ApplyMaxVelocity(max_velocity);
    m_is_note_sustained = true;
    m_is_pedal_sustained = is_pedal_sustained;
}

void CChannel::NoteOff(void)
{
    m_is_note_sustained = false;
    CheckGateClosed();
}

void CChannel::ReleaseSustainPedal(void)
{
    m_is_pedal_sustained = false;
    CheckGateClosed();
}

void CChannel::CheckGateClosed(void)
{
    if(!m_is_note_sustained && !m_is_pedal_sustained)
    {
        m_gate_time = duration_cast<ms>(system_clock::now().time_since_epoch() - m_trigger_time);
    }
}

int CChannel::ComputeVelocity(void)
{
    if(m_max_velocity == 0)
    {
        return 0;
    }
    EPhase phase;
    double progress_percentage;
    ComputePhase(phase, progress_percentage);
    switch(phase)
    {
    case BEFORE:
    case AFTER:
        return 0;
    case ATTACK:
        return static_cast<int>(m_max_velocity * progress_percentage);
    case DECAY:
        return static_cast<int>(m_adsr->S() + ((m_max_velocity - m_adsr->S()) * (1.0 - progress_percentage)));
    case SUSTAIN:
        return m_adsr->S();
    case RELEASE:
        return static_cast<int>(m_adsr->S() * (1.0 - progress_percentage));
    default:
        return 0;
    }
}

void CChannel::ComputePhase(EPhase& phase, double& progress_percentage)
{
    ms now = duration_cast<ms>(system_clock::now().time_since_epoch());
    ms delay_from_start = duration_cast<ms>(now - m_trigger_time);
    if(delay_from_start < ms(0))
    {
        // This should never happen
        phase = BEFORE;
        progress_percentage = 0.0;
    }
    else if(delay_from_start < m_adsr->A())
    {
        phase = ATTACK;
        if(m_adsr->A() == ms(0))
        {
            progress_percentage = 1.0;
        }
        else
        {
            progress_percentage = static_cast<double>(delay_from_start.count()) / static_cast<double>(m_adsr->A().count());
        }
    }
    else if(delay_from_start < (m_adsr->AD()))
    {
        phase = DECAY;
        if(m_adsr->D() == ms(0))
        {
            progress_percentage = 1.0;
        }
        else
        {
            progress_percentage = static_cast<double>(delay_from_start.count() - m_adsr->A().count()) / static_cast<double>(m_adsr->D().count());
        }
    }
    else if(m_is_note_sustained || m_is_pedal_sustained)
    {
        phase = SUSTAIN;
        progress_percentage = 0;
    }
    else if(delay_from_start < (m_gate_time + m_adsr->R()))
    {
        phase = RELEASE;
        if(m_adsr->R() == ms(0))
        {
            progress_percentage = 1.0;
        }
        else
        {
            progress_percentage = static_cast<double>(delay_from_start.count() - m_gate_time.count()) / static_cast<double>(m_adsr->R().count());
        }
    }
    else
    {
        phase = AFTER;
        progress_percentage = 0.0;
    }
}
