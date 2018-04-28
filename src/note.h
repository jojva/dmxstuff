#pragma once

#include "adsr.h"

#include <chrono>

class CNote
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
    CNote(void);

    // TODO : ADSR is loaded once at start-up. Don't send it through NoteOn as it will never change.
    void NoteOn(const CADSR& adsr, int max_velocity, bool is_pedal_sustained);
    void NoteOff(void);
    void ReleaseSustainPedal(void);
    void TriggerSostenuto(bool abEnabled);
    void CheckGateClosed(void);
    int ComputeVelocity(void) const;
    void ComputePhase(EPhase& phase, double &progress_percentage) const;

private:
    CADSR*  m_adsr;
    int     m_max_velocity;
    ms      m_trigger_time;
    ms      m_gate_time;
    bool    m_is_note_on;
    bool    m_is_pedal_sustained;
    bool    m_is_sostenuto;
};
