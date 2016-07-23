#pragma once
#include <asio/ip/tcp.hpp>
#include <http_parser.h>
#include <array>
#include <memory>

class report;

class connection : public std::enable_shared_from_this<connection> {
public:
  connection(report& report);

  void start();
  void close();

  asio::ip::tcp::socket& socket();
  const asio::ip::tcp::socket& socket() const;

private:
  void read();

  void on_close();

  report& report_;
  asio::ip::tcp::socket socket_;
  std::array<char, 8192> buffer_;
  http_parser parser_;
  http_parser_settings parser_settings_;
};
