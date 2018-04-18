#include "channel.h"
#include "synesthesizer.h"

using namespace std::chrono;

#define MAX_VELOCITY_HARD_CAP 254

CChannel::CChannel(void) :
    m_max_velocity(0),
    m_fake_max_velocity(0),
    m_trigger_time(0)
{
}

void CChannel::NoteOn(const SADSR &adsr, int max_velocity)
{
    ms rollback_time = ms(0);
    int current_velocity = ComputeVelocity();
    if(current_velocity > 0)
    {
        rollback_time = ms((current_velocity * m_adsr.attack) / m_fake_max_velocity);
    }
    m_trigger_time = duration_cast<ms>(system_clock::now().time_since_epoch() - rollback_time);
    m_max_velocity = max_velocity;
    m_fake_max_velocity = MAX_VELOCITY_HARD_CAP;
    m_adsr = adsr;
}

void CChannel::NoteOff(void)
{
}

int CChannel::ComputeVelocity(void)
{
    if(m_fake_max_velocity == 0)
    {
        return 0;
    }
    ComputePhase();
    int sustain_level = (m_fake_max_velocity * m_adsr.sustain) / 100;
    switch(m_phase)
    {
    case BEFORE:
    case AFTER:
        return 0;
    case ATTACK:
        return (m_fake_max_velocity * m_progress_percentage) / 100;
    case DECAY:
        return sustain_level + (((m_fake_max_velocity - sustain_level) * (100 - m_progress_percentage)) / 100);
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
    SADSR adsr_fixed = m_adsr;
    adsr_fixed.attack = (m_adsr.attack * m_max_velocity) / MAX_VELOCITY_HARD_CAP;
    adsr_fixed.decay = (m_adsr.decay * m_max_velocity) / MAX_VELOCITY_HARD_CAP;
    adsr_fixed.release = (m_adsr.release * m_max_velocity) / MAX_VELOCITY_HARD_CAP;

    ms now = duration_cast<ms>(system_clock::now().time_since_epoch());
    ms delay_from_start = duration_cast<ms>(now - m_trigger_time);
    if(delay_from_start < ms(0))
    {
        // This should never happen
        m_phase = BEFORE;
        m_progress_percentage = 0;
    }
    else if(delay_from_start >= ms(0) && delay_from_start < adsr_fixed.attack)
    {
        m_phase = ATTACK;
        if(adsr_fixed.attack == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * delay_from_start) / adsr_fixed.attack;
        }
    }
    else if(delay_from_start >= adsr_fixed.attack && delay_from_start < (adsr_fixed.attack + adsr_fixed.decay))
    {
        m_phase = DECAY;
        if(adsr_fixed.decay == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - adsr_fixed.attack)) / adsr_fixed.decay;
        }
    }
//    else if(TODO)
//    {
//        m_phase = SUSTAIN;
//        m_progress_percentage = 0;
//    }
    else if(delay_from_start >= (adsr_fixed.attack + adsr_fixed.decay) && delay_from_start < (adsr_fixed.attack + adsr_fixed.decay + adsr_fixed.release))
    {
        m_phase = RELEASE;
        if(adsr_fixed.release == ms(0))
        {
            m_progress_percentage = 100;
        }
        else
        {
            m_progress_percentage = (100 * (delay_from_start - adsr_fixed.attack - adsr_fixed.decay)) / adsr_fixed.release;
        }
    }
    else
    {
        m_phase = AFTER;
        m_progress_percentage = 0;
    }
}
