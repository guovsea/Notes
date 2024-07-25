## 指定路径


```css
<RCC>
    <qresource prefix="/res">
        <file>res/ice.png</file>
</RCC>
```

在上面这个 qss 文件中，`ice.png` 文件路径是什么： **`:/res/res/ice.png`**。

而不是！`/res/ice.png`

搜索文件的路径为 ： `prefix / file`