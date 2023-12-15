#pragma once

namespace DES
{

/// @brief Represents a state change that happens at a discrete point in time.
template <typename TIME_T>
class Event
{

protected:

    TIME_T m_time;

public:

    Event(TIME_T time) : m_time(time) {}
    virtual ~Event(){}
    
    /// @brief Action to be performed when the Simulator reaches this events time.
    virtual void run() = 0;

    inline
    TIME_T get_time() const
    {
        return m_time;
    }

};

};