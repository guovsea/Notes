#include "LoginPresenter.h"
#include "MockLoginView.h"
#include "LoginModelImpl.h"
#include <memory>
#include <cassert>
#include <iostream>

void testEmptyInput() {
    // 1. 构建 Mock 依赖
    auto mockModel = std::make_shared<LoginModelImpl>();
    auto presenter = createLoginPresenter(mockModel);
    MockLoginView mockView;
    presenter->attachView(&mockView);

    // 2. 设置空输入
    mockView.setInput("", "123456");

    // 3. 触发登录
    presenter->handleLogin();

    // 4. 验证结果（仅通过抽象接口断言）
    assert(mockView.getErrorMsg() == "用户名/密码不能为空！");
    assert(!mockView.isLoading());
    std::cout << "testEmptyInput: 测试通过 ✅" << std::endl;
}

void testLoginSuccess() {
    auto mockModel = std::make_shared<LoginModelImpl>();
    auto presenter = createLoginPresenter(mockModel);
    MockLoginView mockView;
    presenter->attachView(&mockView);

    mockView.setInput("admin", "123456");
    presenter->handleLogin();

    assert(mockView.isLoading());
    // 等待异步登录完成
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "[";
    std::cout << mockView.getSuccessMsg();
    std::cout << "]";
    assert(mockView.getSuccessMsg() == "登录成功！Token: token_123456789");
    std::cout << "testLoginSuccess: 测试通过 ✅" << std::endl;
}

int main() {
    testEmptyInput();
    testLoginSuccess();
    return 0;
}