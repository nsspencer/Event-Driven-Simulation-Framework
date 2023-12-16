#include <ctime>
#include <iostream>
#include "discrete_event_simulator.hpp"
#include <thread>
#include <chrono>

using Event = DES::Event<time_t>;
using Simulator = DES::Simulator<time_t>;

struct PrintTimeEvent : public Event
{

    PrintTimeEvent(time_t time) : Event(time) {}

    void run()
    {
        std::cout << m_time << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Simulator::get_instance()->add_event(std::move(std::unique_ptr<PrintTimeEvent>(new PrintTimeEvent(time(NULL)))));
    }

};

int main()
{
    Simulator* sim = Simulator::get_instance();

    sim->add_event(std::move(std::unique_ptr<PrintTimeEvent>(new PrintTimeEvent(time(NULL)))));
    sim->run_forever();

    return 0;
}