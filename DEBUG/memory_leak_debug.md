Linux如何调试内存泄漏？超牛干货奉献给你（代码全）
https://mp.weixin.qq.com/s/8HcPYzmaLxIAXIRHkrU8MA
加参数：-fsanitize=address -g
例如：gcc -fsanitize=address -g memory_leak_debug.c &&   ./a.out
sanitize 审查
jdy:只有运行到才会报出错误



valgrind  --tool=memcheck  ./test

【开源工具】推荐一款Linux下内存检测工具：valgrind - https://mp.weixin.qq.com/s/aQoiF43m3oRCK4JksgcCTQ

【底层原理】一次程序crash后的调试之旅 —— GDB调试心得 - https://mp.weixin.qq.com/s/UfeUXG0cYYfw9y4-xZGp2g