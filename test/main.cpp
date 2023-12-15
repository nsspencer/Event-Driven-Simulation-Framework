#include "discrete_event_simulator.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

using Event = DES::Event<double>;
using Simulator = DES::Simulator<double>;

struct Print : public Event
{
    Print(double time) : Event(time){}

    void run()
    {
        std::cout << "Time: " << m_time << std::endl;
    }
};

struct Shutdown : public Event
{
    Shutdown(double time) : Event(time) {}

    void run()
    {
        Simulator* sim = Simulator::get_instance();
        std::cout << "Shutting down at time: " << m_time << std::endl;
        sim->shutdown();
    }
};

struct PrintForNSeconds : public Event
{
    double duration_s;
    double offset_s;

    PrintForNSeconds(double duration_s, double offset_s, double start_time_s) : duration_s(duration_s), offset_s(offset_s), Event(start_time_s){}

    void run()
    {
        std::cout << "Printing Time: " << m_time << std::endl;
        auto* sim = Simulator::get_instance();        
        
        if (m_time >= duration_s)
        {
            sim->add_event(std::move(std::unique_ptr<Shutdown>(new Shutdown(sim->get_current_time()))));
            return;
        }
            
        auto next_event = std::unique_ptr<PrintForNSeconds>(new PrintForNSeconds(duration_s, offset_s, m_time + offset_s));
        sim->add_event(std::move(next_event));
    }
};


void delay_add_event(int delay_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    Simulator* sim = Simulator::get_instance();
    sim->add_event(std::move(std::unique_ptr<Shutdown>(new Shutdown(sim->get_current_time()))));
}

int main()
{
    Simulator* sim = Simulator::get_instance();

    sim->add_event(std::move(std::unique_ptr<Print>(new Print(1.0))));
    sim->add_event(std::move(std::unique_ptr<PrintForNSeconds>(new PrintForNSeconds(1000000, 1.0, 0))));


    std::thread waitingThread(delay_add_event, 500);
    waitingThread.detach();

    sim->run_forever();

    if (waitingThread.joinable())
        waitingThread.join();

    return 0;
}