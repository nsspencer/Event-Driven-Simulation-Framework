#pragma once
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "event.hpp"


namespace DES
{

struct Comparer
{
    inline
    bool operator()(const std::unique_ptr<Event>& a, const std::unique_ptr<Event>& b)
    {
        return a->get_time() > b->get_time();
    }
};


class Simulator
{

private:

    bool m_active;
    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, Comparer> m_queue;
    static Simulator* m_instance;
    double m_current_time;

    std::mutex mtx;
    std::condition_variable cv;
    bool isDataReady = false;

    Simulator()
    {
        m_active = true;
        m_current_time = 0.0;
    }
    
    ~Simulator()
    {
        delete m_instance;
    }

    void wait_for_events()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return isDataReady; }); // Thread sleeps until isDataReady becomes true
    }
    
public:

    // Delete the copy constructor and assignment operator
    Simulator(Simulator const&) = delete;
    void operator=(Simulator const&) = delete;

    void run_forever()
    {
        while (m_active)
        {
            if (!isDataReady)
                wait_for_events();

            // Take ownership of the event
            std::unique_ptr<Event> curr_event = std::move(const_cast<std::unique_ptr<Event>&>(m_queue.top()));
            
            // remove the event from the queue, since it may add more events
            m_queue.pop();

            m_current_time = curr_event->get_time();
            curr_event->run();

            if (m_queue.empty())
                isDataReady = false;
        }
    }

    void shutdown()
    {
        m_active = false;
    }

    static
    Simulator* get_instance()
    {
        if (m_instance != nullptr)
        {
            return m_instance;
        }

        m_instance = new Simulator();
        return m_instance;
    }

    void add_event(std::unique_ptr<Event> event)
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_queue.emplace(std::move(event));

        if (isDataReady == false)
        {
            isDataReady = true;
            cv.notify_one();
        }        
    }


    double get_current_time()
    {
        return m_current_time;
    }

};

}