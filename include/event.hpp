#pragma once

namespace DES
{

class Event
{

protected:

    double m_time;

public:

    Event(double time) : m_time(time) {}
    virtual ~Event(){}
    
    virtual void run() = 0;

    inline
    double get_time() const
    {
        return m_time;
    }

};

};