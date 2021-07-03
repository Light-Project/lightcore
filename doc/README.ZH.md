特征

1. 可深度裁剪, 内核最小只需要64KiB的Ram(xip)
2. 

系统启动流程: 

1. preload流程: 初始化dram等外设, 载入kboot和kernel到内存
2. kboot流程, 解压校验内核, 初始化mmu并使内核映射到虚拟地址空间, kboot镜像可直接被uboot等bootloader加载
3. kernel流程, 内核启动

支持三种自由选择的应用加载模式: 

- 传统的文件系统模式 (适用于大内存的系统机)
- XIP模式,应用直接运行在flash上 (适用于支持xip的嵌入式平台)
- Build in模式, 应用直接与内核一起链接 (适用于不支持xip的嵌入式平台)

