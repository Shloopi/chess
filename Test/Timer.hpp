#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>
#include <string>

class Timer {
public:
    explicit Timer(const std::string& name = "")
        : name(name),
        start(std::chrono::high_resolution_clock::now())
    {
    }

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();

        if (!name.empty())
            std::cout << name << ": " << duration << " ms\n";
        else
            std::cout << duration << " ms\n";
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};

#endif