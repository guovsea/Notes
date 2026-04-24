---
---
According to [standard C++](http://www.efnetcpp.org/wiki/ISO/IEC_14882) ([wayback machine link](http://web.archive.org/web/20070403232333/http://www.efnetcpp.org/wiki/ISO/IEC_14882)) : 

A **translation unit** is the basic unit of compilation in C++. It consists of the contents of **a single source file, plus the contents of any header files directly or indirectly included by it, minus those lines that were ignored using conditional preprocessing statements.**  

编译单元是 C++ 中编译的基本单元。它由**单个源文件的内容以及它直接或间接包含的任何头文件的内容组成，减去使用条件预处理语句忽略的行**。*编译单元就是预处理后的源文件*

A single translation unit can be compiled into an object file, library, or executable program.  
单个编译单元可以编译为目标文件、库或可执行程序。

The notion of a translation unit is most often mentioned in the contexts of the One Definition Rule, and templates.  

编译单元的概念最常在关于 "ODR" 规则和模板的内容中提及。