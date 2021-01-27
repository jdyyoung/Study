#makefile--函数定义 函数调用 - 简书 - https://www.jianshu.com/p/e2c78c8fb4a5#
define func1
	@echo "my name is $(0)"
endef

define func2
	@echo "my name is $(0)"
	@echo "param1 => $(1)"
	@echo "param2 => $(2)"
endef

test :
	$(call func1)
	$(call func2, value1)
