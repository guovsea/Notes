#include "ConsoleLoginView.h"
#include "LoginModelImpl.h"
#include "LoginPresenter.h"
#include <memory>
#include <chrono>
#include <thread>

int main() {
    // 1. 创建 Model 具体实现（可替换为 MockModel，不影响其他层）
    std::shared_ptr<ILoginModel> model = std::make_shared<LoginModelImpl>();

    // 2. 创建 Presenter 抽象（通过工厂函数，View 无需感知具体 Presenter）
    std::shared_ptr<ILoginPresenter> presenter = createLoginPresenter(model);

    // 3. 创建 View 具体实现（注入 Presenter 抽象，View 不感知 Presenter 具体类型）
    std::unique_ptr<ConsoleLoginView> view = std::make_unique<ConsoleLoginView>(presenter);

    // 4. 启动 View，触发交互流程
    view->run();

    // 等待异步登录完成（控制台程序需阻塞）
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}