#include <report.h>
#include <asio/io_service.hpp>
#include <asio/signal_set.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <clocale>

int main(int argc, char* argv[])
{
  // Set the locale.
  std::setlocale(LC_ALL, "");

  // Create a thread pool.
  std::vector<std::thread> threads;

  int ret = 0;
  try {
    // Initialize asio.
    asio::io_service service;
    asio::signal_set signals(service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#ifdef SIGQUIT
    signals_.add(SIGQUIT);
#endif

    // Create the server.
    report report(service);

    // Handle signals.
    signals.async_wait([&report](std::error_code ec, int code) {
      report.stop();
    });

    // Start the thread pool threads.
    std::exception_ptr exception;
    try {
      for (auto i = 0; i < 4; i++) {
        threads.emplace_back([&service]() {
          service.run();
        });
      }
    }
    catch (...) {
      exception = std::current_exception();
    }
    
    // Join the thread pool threads.
    for (auto& thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }

    if (exception) {
      std::rethrow_exception(exception);
    }
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    ret = 1;
  }

#ifdef _WIN32
  if (IsDebuggerPresent()) {
    std::cerr << "Press ENTER to close this window..." << std::flush;
    std::cin.get();
  }
#endif
  return ret;
}
