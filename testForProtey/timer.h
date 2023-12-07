
#include <thread>
#include <chrono>
#include <boost/function.hpp>
#include <boost/asio/ip/tcp.hpp>

class Timer {
public:
    Timer();
   
    void sozvon(int delay);
};

Timer::Timer() { 
}

inline void Timer::sozvon(int delay) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}


