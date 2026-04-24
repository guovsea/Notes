#pragma once
#include <string>
#include <memory>
#include <functional>

// ====================== 基础类型/回调 ======================
// Model 回调接口（通知 Presenter 登录结果）
class LoginModelCallback {
public:
    virtual ~LoginModelCallback() = default;
    virtual void onLoginSuccess(const std::string& token) = 0;
    virtual void onLoginFailed(const std::string& errorMsg) = 0;
};

// ====================== Model 层抽象接口 ======================
class ILoginModel {
public:
    virtual ~ILoginModel() = default;
    virtual void login(const std::string& username, 
                       const std::string& password,
                       LoginModelCallback* callback) = 0;
};

// ====================== View 层抽象接口 ======================
class ILoginView {
public:
    virtual ~ILoginView() = default;
    virtual void showLoading() = 0;
    virtual void hideLoading() = 0;
    virtual void showSuccess(const std::string& msg) = 0;
    virtual void showError(const std::string& msg) = 0;
    virtual std::string getUsername() = 0;
    virtual std::string getPassword() = 0;
};

// ====================== Presenter 层抽象接口 ======================
// 关键补充：Presenter 抽象接口，View 仅依赖此接口
class ILoginPresenter {
public:
    virtual ~ILoginPresenter() = default;
    // 绑定 View 接口（View 初始化时调用，将自身注入 Presenter）
    virtual void attachView(ILoginView* view) = 0;
    // 解除 View 绑定（View 销毁时调用，避免野指针）
    virtual void detachView() = 0;
    // 核心业务逻辑：处理登录
    virtual void handleLogin() = 0;
};