#pragma once
#include "MVPInterfaces.h"
#include <memory>
#include <string>

// 具体 View 实现（控制台 UI），持有 Presenter 抽象接口
class ConsoleLoginView : public ILoginView {
public:
    // 构造：注入 Presenter 抽象接口（View 不感知具体 Presenter 类型）
    explicit ConsoleLoginView(std::shared_ptr<ILoginPresenter> presenter);
    ~ConsoleLoginView() override;

    // 启动 View（显示界面，处理用户交互）
    void run();

    // 实现 ILoginView 抽象接口
    void showLoading() override;
    void hideLoading() override;
    void showSuccess(const std::string& msg) override;
    void showError(const std::string& msg) override;
    std::string getUsername() override;
    std::string getPassword() override;

private:
    std::shared_ptr<ILoginPresenter> m_presenter; // 持有 Presenter 抽象接口
    std::string m_username;                       // 存储用户输入
    std::string m_password;
};