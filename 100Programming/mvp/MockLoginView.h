#pragma once
#include "MVPInterfaces.h"
#include <string>
#include <atomic>

class MockLoginView : public ILoginView {
public:
    void showLoading() override { m_loading = true; }
    void hideLoading() override { m_loading = false; }
    void showSuccess(const std::string& msg) override { m_successMsg = msg; }
    void showError(const std::string& msg) override { m_errorMsg = msg; }
    std::string getUsername() override { return m_username; }
    std::string getPassword() override { return m_password; }

    // 测试辅助方法
    void setInput(const std::string& username, const std::string& password) {
        m_username = username;
        m_password = password;
    }
    bool isLoading() const { return m_loading; }
    std::string getSuccessMsg() const { return m_successMsg; }
    std::string getErrorMsg() const { return m_errorMsg; }

private:
    std::string m_username;
    std::string m_password;
    std::string m_successMsg;
    std::string m_errorMsg;
    std::atomic<bool> m_loading{false};
};

