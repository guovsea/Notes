## 非 UI 生成的对象使用qss

```css
QDialog#LoginDialog{
    background-color:rgb(0,255,255)
}
```

```cpp
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setObjectName("LoginDialog");
 }
```