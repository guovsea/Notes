#include "LoginPresenter.h"
#include <iostream>

// 工厂函数实现
std::shared_ptr<ILoginPresenter> createLoginPresenter(std::shared_ptr<ILoginModel> model) {
    return std::make_shared<LoginPresenter>(model);
}

// Presenter 构造/析构
LoginPresenter::LoginPresenter(std::shared_ptr<ILoginModel> model)
    : m_model(std::move(model)) {
    m_callback = std::make_unique<LoginModelCallbackImpl>(this);
}

LoginPresenter::~LoginPresenter() {
    detachView();
}

// 绑定 View（View 主动将自身注入 Presenter）
void LoginPresenter::attachView(ILoginView* view) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_view = view;
}

// 解除 View 绑定
void LoginPresenter::detachView() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_view = nullptr;
}

// 核心：处理登录逻辑（Presenter 仅操作抽象接口）
void LoginPresenter::handleLogin() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // 1. 检查 View/Model 是否有效
    if (!m_view) {
        std::cerr << "Error: View 未绑定！" << std::endl;
        return;
    }
    auto model = m_model.lock();
    if (!model) {
        m_view->showError("Model 初始化失败！");
        return;
    }

    // 2. 从 View 抽象接口获取用户输入（不感知具体 View 类型）
    std::string username = m_view->getUsername();
    std::string password = m_view->getPassword();

    // 3. 基础校验（Presenter 处理 UI 相关逻辑）
    if (username.empty() || password.empty()) {
        m_view->showError("用户名/密码不能为空！");
        return;
    }

    // 4. 通知 View 显示加载（通过抽象接口）
    m_view->showLoading();

    // 5. 调用 Model 抽象接口执行登录（不感知具体 Model 类型）
    model->login(username, password, m_callback.get());
}

// Model 回调实现（更新 View 仅通过抽象接口）
void LoginPresenter::LoginModelCallbackImpl::onLoginSuccess(const std::string& token) {
     std::lock_guard<std::mutex> lock(m_presenter->m_mutex);
    if (!m_presenter->m_view) return;

    m_presenter->m_view->showSuccess("登录成功！Token: " + token);
}

void LoginPresenter::LoginModelCallbackImpl::onLoginFailed(const std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(m_presenter->m_mutex);
    if (!m_presenter->m_view) return;

    m_presenter->m_view->showError("登录失败：" + errorMsg);
    m_presenter->m_view->hideLoading();
}