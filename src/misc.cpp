#include <iostream>
#include <csignal>
#include <cstdlib>

// Global flag to indicate whether Ctrl-C was pressed
volatile sig_atomic_t ctrlCPressed = 0;

// Signal handler function for Ctrl-C (SIGINT)
void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "Ctrl-C pressed. Cleaning up..." << std::endl;
        ctrlCPressed = 1;
    }
}

int main() {
    // Install the signal handler for SIGINT
    std::signal(SIGINT, signalHandler);

    // Your main program logic goes here
    while (!ctrlCPressed) {
        // Perform your program logic
    }

    std::cout << "Program exited due to Ctrl-C." << std::endl;

    return 0;
}
