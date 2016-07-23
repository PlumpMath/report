#pragma once
#include <connection.h>
#include <asio.hpp>
#include <memory>
#include <mutex>
#include <set>
#include <string>

class report {
public:
  report(asio::io_service& service);

  void stop();

  void open(std::shared_ptr<connection> connection);
  void close(std::shared_ptr<connection> connection);

  std::string handle() const;

  asio::io_service& service();
  const asio::io_service& service() const;

private:
  void accept();

  asio::io_service& service_;
  asio::ip::tcp::acceptor acceptor_;
  std::set<std::weak_ptr<connection>, std::owner_less<std::weak_ptr<connection>>> connections_;
  std::mutex connections_mutex_;
};
