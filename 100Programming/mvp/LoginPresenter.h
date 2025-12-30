#pragma once
#include "MVPInterfaces.h"
#include <memory>
#include <mutex>

// 具体 Presenter 实现（依赖抽象接口，不耦合具体 View/Model）
class LoginPresenter : public ILoginPresenter {
public:
    // 构造：注入 Model 抽象（Presenter 不关心 Model 具体实现）
    explicit LoginPresenter(std::shared_ptr<ILoginModel> model);
    ~LoginPresenter() override;

    // 实现 ILoginPresenter 接口
    void attachView(ILoginView* view) override;
    void detachView() override;
    void handleLogin() override;

private:
    // Model 回调实现（内部类，解耦回调逻辑）
    class LoginModelCallbackImpl : public LoginModelCallback {
    public:
        explicit LoginModelCallbackImpl(LoginPresenter* presenter) : m_presenter(presenter) {}
        void onLoginSuccess(const std::string& token) override;
        void onLoginFailed(const std::string& errorMsg) override;
    private:
        LoginPresenter* m_presenter;
    };

    std::weak_ptr<ILoginModel> m_model;       // 弱引用 Model（避免循环引用）
    ILoginView* m_view = nullptr;             // 持有 View 抽象接口（非具体 View）
    std::unique_ptr<LoginModelCallbackImpl> m_callback;
    std::mutex m_mutex;                       // 线程安全：保护 View 指针
};

// Presenter 工厂函数（封装创建逻辑，View 无需感知具体 Presenter 类型）
std::shared_ptr<ILoginPresenter> createLoginPresenter(std::shared_ptr<ILoginModel> model);