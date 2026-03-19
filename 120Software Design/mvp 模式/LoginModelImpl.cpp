#include "LoginModelImpl.h"

void LoginModelImpl::login(const std::string& username, 
                           const std::string& password,
                           LoginModelCallback* callback) {
    // 子线程模拟异步登录（避免阻塞 UI 线程）
    std::thread loginThread(&LoginModelImpl::doLogin, this, username, password, callback);
    loginThread.detach();
}

void LoginModelImpl::doLogin(const std::string& username, 
                             const std::string& password,
                             LoginModelCallback* callback) {
    // 模拟网络延迟
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 模拟业务校验
    if (username == "admin" && password == "123456") {
        callback->onLoginSuccess("token_123456789");
    } else {
        callback->onLoginFailed("用户名或密码错误");
    }
}