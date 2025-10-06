#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <string>

void handle_request(boost::beast::http::request<boost::beast::http::string_body> req, boost::beast::http::response<boost::beast::http::string_body>& res)
{
    if (req.method() != boost::beast::http::verb::get)
    {
        res.result(boost::beast::http::status::method_not_allowed);
        res.set(boost::beast::http::field::content_type, "text/plain");
        res.body() = "Method not allowed\n";
        res.prepare_payload();
        return;
    }

    res.result(boost::beast::http::status::ok);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.body() = "<html><body><h1>Hello from Boost.Beast!</h1></body></html>";
    res.prepare_payload();
}

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> req_;

public:
    explicit HttpSession(boost::asio::ip::tcp::socket&& socket)
        : socket_(std::move(socket))
    {
    }

    void run()
    {
        boost::beast::http::async_read(socket_, buffer_, req_,
            boost::beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
    }

private:
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::http::error::end_of_stream)
        {
            return do_close();
        }

        if (ec)
        {
            std::cerr << "Read error: " << ec.message() << std::endl;
            return;
        }

        boost::beast::http::response<boost::beast::http::string_body> res;
        handle_request(std::move(req_), res);

        boost::beast::http::async_write(socket_, res,
            boost::beast::bind_front_handler(&HttpSession::on_write, shared_from_this(), res.need_eof()));
    }

    void on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
        {
            std::cerr << "Write error: " << ec.message() << std::endl;
            return;
        }

        if (close)
        {
            return do_close();
        }

        req_ = {};
        boost::beast::http::async_read(socket_, buffer_, req_,
            boost::beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
    }

    void do_close()
    {
        boost::beast::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }
};

class HttpServer
{
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_context& ioc_;

public:
    HttpServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint)
        : acceptor_(ioc), ioc_(ioc)
    {
        boost::beast::error_code ec;
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) { fail(ec, "open"); return; }

        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) { fail(ec, "set_option"); return; }

        acceptor_.bind(endpoint, ec);
        if (ec) { fail(ec, "bind"); return; }

        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) { fail(ec, "listen"); return; }
    }

    void run()
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<HttpSession>(std::move(socket))->run();
                }
                do_accept();
            });
    }

    void fail(boost::beast::error_code ec, char const* what)
    {
        std::cerr << what << ": " << ec.message() << std::endl;
    }
};

int main()
{
    try
    {
        const auto address = boost::asio::ip::make_address("0.0.0.0");
        const unsigned short port = 8080;

        boost::asio::io_context ioc{ 1 };

        HttpServer server{ ioc, {address, port} };
        std::cout << "HTTP server listening on " << address << ":" << port << std::endl;

        server.run();
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}