#include "raftrepeattimer.h"
#include <iostream>

int main() {
    int counter = 0;

    auto timer = Nano::Raft::make_repeating_timer(1, [&counter]() {
        std::cout << "Counter: " << ++counter << std::endl;
        });
    timer.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));
    timer.stop();

    return 0;
}