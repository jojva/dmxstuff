#include "channel.h"
#include "synesthesizer.h"

using namespace std::chrono;

CChannel::CChannel(void) :
    m_max_velocity(0),
    m_trigger_time(0)
{
}

void CChannel::NoteOn(int velocity)
{
    m_max_velocity = velocity;
    m_trigger_time = duration_cast<ms>(system_clock::now().time_since_epoch());
}

void CChannel::NoteOff(void)
{
}

int CChannel::ComputeVelocity(const SADSR &adsr)
{
    if(m_max_velocity == 0)
    {
        return 0;
    }
    ComputePhase(adsr);
    int sustain_level = (m_max_velocity * adsr.sustain) / 100;
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

void CChannel::ComputePhase(const SADSR& adsr)
{
    ms now = duration_cast<ms>(system_clock::now().time_since_epoch());
    ms delay_from_start = duration_cast<ms>(now - m_trigger_time);
    if(delay_from_start < ms(0))
    {
        // This should never happen
        m_phase = BEFORE;
        m_progress_percentage = 0;
    }
    else if(delay_from_start >= ms(0) && delay_from_start < adsr.attack)
    {
        m_phase = ATTACK;
        if(adsr.attack == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * delay_from_start) / adsr.attack;
        }
    }
    else if(delay_from_start >= adsr.attack && delay_from_start < (adsr.attack + adsr.decay))
    {
        m_phase = DECAY;
        if(adsr.decay == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - adsr.attack)) / adsr.decay;
        }
    }
//    else if(TODO)
//    {
//        m_phase = SUSTAIN;
//        m_progress_percentage = 0;
//    }
    else if(delay_from_start >= (adsr.attack + adsr.decay) && delay_from_start < (adsr.attack + adsr.decay + adsr.release))
    {
        m_phase = RELEASE;
        if(adsr.release == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - adsr.attack - adsr.decay)) / adsr.release;
        }
    }
    else
    {
        m_phase = AFTER;
        m_progress_percentage = 0;
    }
}
