#include "PreCompileHeader.h"

#include "CurveSequence.h"

namespace DoDo {
    FCurveSequence::FCurveSequence()
        : m_start_time(0.0)
        , m_pause_time(0.0)
        , m_total_duration(0.0)
        , m_b_in_reverse(true)
        , m_b_is_looping(false)
        , m_b_is_paused(false)
    {
    }
    FCurveSequence::~FCurveSequence()
    {
    }
}