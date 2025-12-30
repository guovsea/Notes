#pragma once
#include "MVPInterfaces.h"

// 可选
class MockLoginModel : public ILoginModel {
public:
    void login(const std::string& username, 
               const std::string& password,
               LoginModelCallback* callback) override {
        if (username == "admin" && password == "123456") {
            callback->onLoginSuccess("mock_token");
        } else {
            callback->onLoginFailed("mock_error");
        }
    }
};