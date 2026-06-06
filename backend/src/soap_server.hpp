#pragma once
#include <atomic>
#include <string>
#include <thread>

class SoapServer {
  public:
    SoapServer(std::string xml_path, int port = 8081);
    ~SoapServer();

    void start();
    void stop();

  private:
    int port_;
    std::string xml_path_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    void run_loop();
};
