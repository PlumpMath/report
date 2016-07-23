#include <report.h>
#include <config.h>
#include <iostream>

report::report(asio::io_service& service) :
  service_(service), acceptor_(service_)
{
  std::clog << PROJECT << ' ' << VERSION << std::endl;
  asio::ip::tcp::resolver resolver(service_);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve({ "0.0.0.0", "8080" });
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  accept();
  std::clog << "server started: " << endpoint << std::endl;
}

void report::stop()
{
  std::error_code ec;
  acceptor_.close(ec);
  std::lock_guard<std::mutex> lock(connections_mutex_);
  for (auto& wp : connections_) {
    if (auto sp = wp.lock()) {
      sp->close();
    }
  }
}

void report::open(std::shared_ptr<connection> connection)
{
  std::lock_guard<std::mutex> lock(connections_mutex_);
  connections_.insert(connection);
}

void report::close(std::shared_ptr<connection> connection)
{
  std::lock_guard<std::mutex> lock(connections_mutex_);
  connections_.erase(connection);
}

std::string report::handle() const
{
  // TODO: This can be any dynamic content.
  return "test";
}

asio::io_service& report::service()
{
  return service_;
}

const asio::io_service& report::service() const
{
  return service_;
}

void report::accept()
{
  auto c = std::make_shared<connection>(*this);
  acceptor_.async_accept(c->socket(), [this, c](std::error_code ec) {
    if (ec) {
      if (!acceptor_.is_open()) {
        std::clog << "server stopped" << std::endl;
        return;
      }
      std::cerr << "accept error: " << ec.message() << std::endl;
      accept();
      return;
    }
    try {
      open(c);
      c->start();
    }
    catch (const std::exception& e) {
      std::cerr << "connection error: " << e.what() << std::endl;
    }
    catch (...) {
      std::cerr << "connection error: unhandled exception" << std::endl;
    }
    accept();
  });
}
