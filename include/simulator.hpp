#pragma once
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "event.hpp"


namespace DES
{

template <typename TIME>
struct Comparer
{
    inline
    bool operator()(const std::unique_ptr<Event<TIME>>& a, const std::unique_ptr<Event<TIME>>& b)
    {
        return a->get_time() > b->get_time();
    }
};


template <typename TIME>
class Simulator
{

private:

    bool m_active;
    std::priority_queue<std::unique_ptr<Event<TIME>>, std::vector<std::unique_ptr<Event<TIME>>>, Comparer<TIME>> m_queue;
    static Simulator<TIME>* m_instance;
    TIME m_current_time;

    // Thread safety members
    std::mutex mtx;
    std::condition_variable cv;
    bool m_events_available;

    Simulator()
    {
        m_active = true;
        m_events_available = false;
    }
    
    ~Simulator()
    {
        delete m_instance;
    }

    /// @brief Pause the thread while there are no events in the queue.
    /// This saves cpu cycles from being wasted.
    void wait_for_events()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return m_events_available; });
    }
    
public:

    // Delete the copy constructor and assignment operator to pertain to a singleton
    Simulator(Simulator const&) = delete;
    void operator=(Simulator const&) = delete;

    /// @brief Executes Events until shutdown() has been called. If no Events are available, the thread
    /// is put to sleep until one is added.
    void run_forever()
    {
        while (m_active)
        {
            if (!m_events_available)
                wait_for_events();

            // Take ownership of the event
            std::unique_ptr<Event<TIME>> curr_event = std::move(const_cast<std::unique_ptr<Event<TIME>>&>(m_queue.top()));
            
            // remove the event from the queue, since it may add more events
            m_queue.pop();

            m_current_time = curr_event->get_time();
            curr_event->run();

            // check for more events
            if (m_queue.empty())
                m_events_available = false;
        }
    }

    /// @brief will stop the run_forever() loop when called.
    void shutdown()
    {
        m_active = false;
    }

    /// @brief Get the singleton instance of the Simulator. Creates one of one hasn't been created before.
    static
    Simulator* get_instance()
    {
        if (m_instance)
            return m_instance;

        m_instance = new Simulator();
        return m_instance;
    }

    /// @brief Add an event to the queue.
    void add_event(std::unique_ptr<Event<TIME>> event)
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_queue.emplace(std::move(event));

        // only notify if we are changing state to non-empty
        if (!m_events_available)
        {
            m_events_available = true;
            cv.notify_one();
        }        
    }

    /// @brief Get the current time of the Simulator, provided by the last Event that was run.
    TIME get_current_time()
    {
        return m_current_time;
    }

};

// initialize the instance pointer
template<typename T>
Simulator<T>* Simulator<T>::m_instance = nullptr;

}