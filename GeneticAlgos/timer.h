//
//  timer.h
//  HillClimbImage
//
//  Created by asd on 29/04/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef timer_h
#define timer_h

#include <chrono>
#include <ratio>
#include <functional>
#include <thread>

class Timer {
    std::chrono::high_resolution_clock::time_point begin, end;
public:
    Timer() {
        start();
    }
    void start() {
        begin = std::chrono::high_resolution_clock::now();
    }
    long lapMicro() {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> time_span = end - begin;
        return (long)time_span.count();
    }
    long lapMilli() {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_span = end - begin;
        return (long)time_span.count();
    }
    long chronoMicro(std::function<void(void)>const& lambda) {
        start();
        lambda();
        return lapMicro();
    }
    long chronoMilli(std::function<void(void)>const& lambda) {
        start();
        lambda();
        return lapMilli();
    }
    static void pause(int secs) {
        std::this_thread::sleep_for (std::chrono::seconds(secs));
    }
};

#endif /* timer_h */
