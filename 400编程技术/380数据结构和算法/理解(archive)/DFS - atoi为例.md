## DFS

确定有限状态机

- 当前状态
- 事件
- 动作
- 下一状态
## atoi

请你来实现一个 `myAtoi(string s)` 函数，使其能将字符串转换成一个 32 位有符号整数（类似 C/C++ 中的 `atoi` 函数）。

函数 `myAtoi(string s)` 的算法如下：

1. 读入字符串并丢弃无用的前导空格
2. 检查下一个字符（假设还未到字符末尾）为正还是负号，读取该字符（如果有）。 确定最终结果是负数还是正数。 如果两者都不存在，则假定结果为正。
3. 读入下一个字符，直到到达下一个非数字字符或到达输入的结尾。字符串的其余部分将被忽略。
4. 将前面步骤读入的这些数字转换为整数（即，"123" -> 123， "0032" -> 32）。如果没有读入数字，则整数为 `0` 。必要时更改符号（从步骤 2 开始）。
5. 如果整数数超过 32 位有符号整数范围 `[−231,  231 − 1]` ，需要截断这个整数，使其保持在这个范围内。具体来说，小于 `−231` 的整数应该被固定为 `−231` ，大于 `231 − 1` 的整数应该被固定为 `231 − 1` 。
6. 返回整数作为最终结果。

**注意：**

- 本题中的空白字符只包括空格字符 `' '` 。
- 除前导空格或数字后的其余字符串外，**请勿忽略** 任何其他字符。

```cpp
class Solution {
public:
    int myAtoi(string s) {
        enum State {
            START, SIGNED, IN_NUMBER, END
        };
        int result = 0;
        int sign = 1;
        State state = START;
        for (char c : s) {
            switch (state) {
                case START:
                    if ( c == ' ') {
                        state = START;
                    } else if (c == '+' || c == '-') {
                        sign = c == '+' ? 1 : -1;
                        state = SIGNED;
                    } else if (isdigit(c)) {
                        result = result * 10 + c - '0';
                        state = IN_NUMBER;
                    } else {
                        state = END;
                    }
                    break;
                case SIGNED:
                    if (isdigit(c)) {
                        result = result * 10 + c - '0';
                        state = IN_NUMBER;
                    } else {
                        state = END;
                    }
                    break;
                case IN_NUMBER:
                    if (isdigit(c)) {
                        if (result > INT_MAX / 10 || 
                            (result == INT_MAX / 10 && c - '0' >= INT_MAX % 10)) {
                                // overflow
                                return (sign == 1) ? INT_MAX : INT_MIN;
                            }
                        result = result * 10 + c - '0';
                        state = IN_NUMBER;
                    } else {
                        state = END;
                    }
                    break;
                case END:
                    state = END;
                    break;
            }
        }
        return result * sign;
    }

};
```
```cpp
class DFS {
public:
    void handle(char c) {
        // 状态转移
        state_ = table_[state_][col(c)];
        // 动作
        if (state_ == "in_number") {
            ans_ = ans_ * 10 + c - '0';
            ans_ =  sign_ == 1 ?
                min(ans_, (long long)INT_MAX) : min(ans_, -(long long)INT_MIN);
        }
        if (state_ == "signed") {
            sign_ = c == '+' ? 1 : -1;
        }
        // 其他情况时，遇到事件都不需要做动作
        // 只需要进行状态转移即可
    }
    int get(void) {
        return  sign_ * ans_;
    }
private:
    int sign_ = 1;
    long long ans_ = 0;
    string state_ = "start"; // 初始状态
    unordered_map<string, vector<string>> table_ = {
        {"start", {"start", "signed", "in_number", "end"}},
        {"signed", {"end", "end", "in_number", "end"}},
        {"in_number", {"end", "end", "in_number", "end"}},
        {"end", {"end", "end", "end", "end"}}
    };
    int col(char c) {
        if (c == ' ') return 0;
        if (c == '+' || c == '-') return 1;
        if (isdigit(c)) return 2;
        return 3;
    }
};
class Solution {
public:
    int myAtoi(string s) {
        DFS d;
        for (char c : s) {
            d.handle(c);
        }
        return d.get();
        std::cout << "INT_MAX = " << INT_MAX << std::endl;
    }
};
```
