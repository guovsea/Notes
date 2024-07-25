Ubuntu 20.04 在安装 Neovim 的时候，直接使用 `sudo apt-get install neovim`，会安装比较老的版本，而老版本的 Neovim 不能使用 Lua 进行配置，所以，需要安装较新版本。

参考 [https://github.com/neovim/neo...](https://link.segmentfault.com/?enc=IFNLxuXp%2BBGPOqGPaKWeTA%3D%3D.zytyR72Y0FjjjKU6%2F%2F2ysvqio1xxP2QtVhRI87i6dtMlC%2BpxSIok%2B6wO%2FlbekdgAv3jqCXiiefWurmhvAr620Q%3D%3D)

![image.png](https://segmentfault.com/img/bVcXU5X "image.png")

如果要安装 stable 版本，运行：

sudo add-apt-repository ppa:neovim-ppa/stable

如果要安装 unstable 版本，运行：

sudo add-apt-repository ppa:neovim-ppa/unstable


**删除PPA源**

1. 使用以下命令来删除 PPA（请确保你明确知道要删除的 PPA 名称）：

`sudo add-apt-repository --remove ppa:neovim-ppa/unstable`

3. 系统会要求你确认删除该PPA。按下"Enter"键以继续。
4. 接下来，更新软件包信息，以确保不再从该 PPA 获取软件包信息：
   
   `sudo apt-get update`

现在，你已经成功删除了PPA。系统将不再从这个PPA获取新的软件包或更新信息。