#!/bin/sh

echo "12345i"

boot_hook_init(){
        local hook="${1}_hook"

        export -n "PI_STACK_LIST=${PI_STACK_LIST:+$PI_STACK_LIST }$hook" #加入到PI_STACK_LIST，以空格分隔

        export -n "$hook="   #保存到环境变量中，以参数_hook命名

}

boot_hook_init preinit_main
