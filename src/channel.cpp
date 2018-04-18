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
    int current_velocity = ComputeVelocity();
    ms rollback_time = ms((current_velocity * m_adsr.attack) / m_max_velocity);
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
    ComputePhase();
    int sustain_level = (m_max_velocity * m_adsr.sustain) / 100;
    switch(m_phase)
    {
    case BEFORE:
    case AFTER:
        return 0;
    case ATTACK:
        return (m_max_velocity * m_progress_percentage) / 100;
    case DECAY:
        return sustain_level + (((m_max_velocity - sustain_level) * (100 - m_progress_percentage)) / 100);
    case SUSTAIN:
        // TODO
        return 0;
    case RELEASE:
        return (sustain_level * (100 - m_progress_percentage)) / 100;
    default:
        return 0;
    }
}

void CChannel::ComputePhase(void)
{
    ms now = duration_cast<ms>(system_clock::now().time_since_epoch());
    ms delay_from_start = duration_cast<ms>(now - m_trigger_time);
    if(delay_from_start < ms(0))
    {
        // This should never happen
        m_phase = BEFORE;
        m_progress_percentage = 0;
    }
    else if(delay_from_start >= ms(0) && delay_from_start < m_adsr.attack)
    {
        m_phase = ATTACK;
        if(m_adsr.attack == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * delay_from_start) / m_adsr.attack;
        }
    }
    else if(delay_from_start >= m_adsr.attack && delay_from_start < (m_adsr.attack + m_adsr.decay))
    {
        m_phase = DECAY;
        if(m_adsr.decay == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - m_adsr.attack)) / m_adsr.decay;
        }
    }
//    else if(TODO)
//    {
//        m_phase = SUSTAIN;
//        m_progress_percentage = 0;
//    }
    else if(delay_from_start >= (m_adsr.attack + m_adsr.decay) && delay_from_start < (m_adsr.attack + m_adsr.decay + m_adsr.release))
    {
        m_phase = RELEASE;
        if(m_adsr.release == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - m_adsr.attack - m_adsr.decay)) / m_adsr.release;
        }
    }
    else
    {
        m_phase = AFTER;
        m_progress_percentage = 0;
    }
}
