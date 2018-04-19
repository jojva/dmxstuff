#include "channel.h"
#include "synesthesizer.h"

using namespace std::chrono;

CChannel::CChannel(void) :
    m_max_velocity(0),
    m_trigger_time(0)
{
}

void CChannel::NoteOn(const SADSR &adsr, int max_velocity)
{
    ms rollback_time = ms(0);
    int current_velocity = ComputeVelocity();
    if(current_velocity > 0)
    {
        rollback_time = ms((current_velocity * m_adsr.attack) / m_max_velocity);
    }
    m_trigger_time = duration_cast<ms>(system_clock::now().time_since_epoch() - rollback_time);
    m_max_velocity = max_velocity;
    m_adsr = adsr;
}

void CChannel::NoteOff(void)
{
}

int CChannel::ComputeVelocity(void)
{
    if(m_max_velocity == 0)
    {
        return 0;
    }
    EPhase phase;
    int progress_percentage;
    ComputePhase(phase, progress_percentage);
    int sustain_level = (m_max_velocity * m_adsr.sustain) / 100;
    switch(phase)
    {
    case BEFORE:
    case AFTER:
        return 0;
    case ATTACK:
        return (m_max_velocity * progress_percentage) / 100;
    case DECAY:
        return sustain_level + (((m_max_velocity - sustain_level) * (100 - progress_percentage)) / 100);
    case SUSTAIN:
        // TODO
        return 0;
    case RELEASE:
        return (sustain_level * (100 - progress_percentage)) / 100;
    default:
        return 0;
    }
}

void CChannel::ComputePhase(EPhase& phase, int& progress_percentage)
{
    ms now = duration_cast<ms>(system_clock::now().time_since_epoch());
    ms delay_from_start = duration_cast<ms>(now - m_trigger_time);
    if(delay_from_start < ms(0))
    {
        // This should never happen
        phase = BEFORE;
        progress_percentage = 0;
    }
    else if(delay_from_start >= ms(0) && delay_from_start < m_adsr.attack)
    {
        phase = ATTACK;
        if(m_adsr.attack == ms(0))
        {
            progress_percentage = 100;
        }
        else
        {
            progress_percentage = (100 * delay_from_start) / m_adsr.attack;
        }
    }
    else if(delay_from_start >= m_adsr.attack && delay_from_start < (m_adsr.attack + m_adsr.decay))
    {
        phase = DECAY;
        if(m_adsr.decay == ms(0))
        {
            progress_percentage = 100;
        }
        else
        {
            progress_percentage = (100 * (delay_from_start - m_adsr.attack)) / m_adsr.decay;
        }
    }
//    else if(TODO)
//    {
//        m_phase = SUSTAIN;
//        m_progress_percentage = 0;
//    }
    else if(delay_from_start >= (m_adsr.attack + m_adsr.decay) && delay_from_start < (m_adsr.attack + m_adsr.decay + m_adsr.release))
    {
        phase = RELEASE;
        if(m_adsr.release == ms(0))
        {
            progress_percentage = 100;
        }
        else
        {
            progress_percentage = (100 * (delay_from_start - m_adsr.attack - m_adsr.decay)) / m_adsr.release;
        }
    }
    else
    {
        phase = AFTER;
        progress_percentage = 0;
    }
}
