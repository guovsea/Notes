#pragma once
#include "MVPInterfaces.h"
#include <thread>
#include <chrono>

class LoginModelImpl : public ILoginModel {
public:
    void login(const std::string& username, 
               const std::string& password,
               LoginModelCallback* callback) override;
private:
    void doLogin(const std::string& username, 
                 const std::string& password,
                 LoginModelCallback* callback);
};