#pragma once

namespace DES
{

/// @brief Represents a state change that happens at a discrete point in time.
template <typename TIME>
class Event
{

protected:

    TIME m_time;

public:

    Event(TIME time) : m_time(time) {}
    virtual ~Event(){}
    
    /// @brief Action to be performed when the Simulator reaches this events time.
    virtual void run() = 0;

    /// @brief Get the time that this event is set to be performed at.
    /// @return Time of this event.
    TIME get_time() const
    {
        return m_time;
    }

};

};