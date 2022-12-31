# 根据[acwj](https://github.com/DoctorWkt/acwj)写的一个可以自编译的C语言子集编译器

## 编译器后端目标平台: 
### x86-64
## 开发平台: 
### ubuntu 18.04.5
## 最终效果: 
### 在这里我们称我们编写的编译器为cwj,用gun c编译器编译我们cwj的源代码生成cwj并用cwj编译cwj的源码生成编译器cwj0,最终用cwj0编译cwj的源代码生成cwj1最终cwj0,cwj1的可执行文件都能通过所有测试并且他们不论是在功能还是文件上完全等价，具体操作可以查看 `make quad`