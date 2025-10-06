#pragma once

#include <memory>
#include <string>
#include <vector>

class AppState;
class IOHandler;
class AuthService;
class UserService;
class LogService;

class BaseCommand {
public:
    BaseCommand(std::string name, std::string description,
                std::shared_ptr<AppState> app_state,
                std::shared_ptr<IOHandler> io_handler,
                std::shared_ptr<AuthService> auth_service,
                std::shared_ptr<UserService> user_service,
                std::shared_ptr<LogService> log_service)
        : name_(std::move(name)), description_(std::move(description)),
          app_state_(std::move(app_state)), io_handler_(std::move(io_handler)),
          auth_service_(std::move(auth_service)),
          user_service_(std::move(user_service)),
          log_service_(std::move(log_service)) {}

    virtual ~BaseCommand() = default;

    virtual bool execute(const std::vector<std::string> &args) = 0;
    virtual bool isVisible() const = 0;

    std::string get_name() const { return name_; }
    std::string get_description() const { return description_; }

protected:
    const std::shared_ptr<AppState> app_state_;
    const std::shared_ptr<IOHandler> io_handler_;
    const std::shared_ptr<AuthService> auth_service_;
    const std::shared_ptr<UserService> user_service_;
    const std::shared_ptr<LogService> log_service_;

private:
    const std::string name_;
    const std::string description_;
};
