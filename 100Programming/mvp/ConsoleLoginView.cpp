#include "ConsoleLoginView.h"
#include <iostream>

// View 构造：持有 Presenter 接口，并将自身（ILoginView）注入 Presenter
ConsoleLoginView::ConsoleLoginView(std::shared_ptr<ILoginPresenter> presenter)
    : m_presenter(std::move(presenter)) {
    // 关键：View 主动将自身（抽象接口）绑定到 Presenter
    m_presenter->attachView(this);
}

ConsoleLoginView::~ConsoleLoginView() {
    // View 销毁时，通知 Presenter 解除绑定
    if (m_presenter) {
        m_presenter->detachView();
    }
}

// 启动控制台 View，处理用户输入
void ConsoleLoginView::run() {
    std::cout << "===== MVP 登录示例（控制台版）=====" << std::endl;
    std::cout << "请输入用户名：";
    std::cin >> m_username;
    std::cout << "请输入密码：";
    std::cin >> m_password;
    std::cout << "---------------------------------" << std::endl;

    // 触发登录：View 调用 Presenter 抽象接口（不感知具体 Presenter 实现）
    m_presenter->handleLogin();
}

// 实现 ILoginView 接口（仅 UI 操作，无任何业务逻辑）
void ConsoleLoginView::showLoading() {
    std::cout << "[UI] 加载中...（模拟网络请求）" << std::endl;
}

void ConsoleLoginView::hideLoading() {
    std::cout << "[UI] 加载完成" << std::endl;
}

void ConsoleLoginView::showSuccess(const std::string& msg) {
    std::cout << "[UI] ✅ " << msg << std::endl;
}

void ConsoleLoginView::showError(const std::string& msg) {
    std::cout << "[UI] ❌ " << msg << std::endl;
}

std::string ConsoleLoginView::getUsername() {
    return m_username;
}

std::string ConsoleLoginView::getPassword() {
    return m_password;
}