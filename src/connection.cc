#include <connection.h>
#include <report.h>
#include <iostream>
#include <sstream>

connection::connection(report& report) :
  report_(report), socket_(report_.service()), parser_({}), parser_settings_({})
{
  http_parser_init(&parser_, HTTP_REQUEST);
  
  // TODO: Gather information about the request from preceding http_parser callbacks.

  parser_settings_.on_message_complete = [this](::http_parser* parser) {
    std::cout << "handle" << std::endl;
    auto body = report_.handle();
    std::ostringstream oss;
    oss << "HTTP/1.0 200 OK\r\n"
        << "Server: report\r\n"
        //<< "Date: " << date() << "\r\n"
        << "Content-Type: text/plain\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n\r\n";
    auto data = std::make_shared<std::array<std::string, 2>>();
    (*data)[0] = oss.str();
    (*data)[1] = std::move(body);
    std::array<asio::const_buffer, 2> buffers = {
      asio::buffer((*data)[0]), asio::buffer((*data)[1])
    };
    asio::async_write(socket_, buffers, [this, data](std::error_code ec, std::size_t size) {
      close();  // TODO: Only if HTTP/1.0 and the connection type is not "close".
    });
    return 0;
  };
}

asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
  read();
}

void connection::close()
{
  std::error_code ec;
  socket_.close(ec);
}

const asio::ip::tcp::socket& connection::socket() const
{
  return socket_;
}

void connection::read()
{
  auto self = shared_from_this();
  socket_.async_read_some(asio::buffer(buffer_), [this, self(std::move(self))](std::error_code ec, std::size_t size) {
    if (ec) {
      if (ec != asio::error::eof && ec != asio::error::connection_reset && socket_.is_open()) {
        std::cerr << "read error: " << ec.message() << std::endl;
      }
      on_close();
      return;
    }

    try {
      while (size > 0) {
        auto data = buffer_.data();
        auto read = http_parser_execute(&parser_, &parser_settings_, data, size);
        if (parser_.http_errno != http_errno::HPE_OK) {
          std::cerr << "parse error: " << http_errno_name(static_cast<http_errno>(parser_.http_errno)) << std::endl;
          close();
          break;
        }
        data += read;
        size -= read;
      }
    }
    catch (const std::exception& e) {
      std::cerr << "parse error: " << e.what() << std::endl;
      on_close();
      return;
    }
    catch (...) {
      std::cerr << "parse error: unhandled exception" << std::endl;
      on_close();
      return;
    }

    read();
  });
}

void connection::on_close()
{
  report_.close(shared_from_this());
}
