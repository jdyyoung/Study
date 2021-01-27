.PHONY : test

define func1
	@echo "my name is $(0)"
endef

define func2
	@echo "my name is $(0)"
endef

var1 := $(call func1)
var2 := $(call func2)
var3 := $(abspath ./)
var4 := $(abspath test.cpp)


test:
	@echo "var1 => $(var1)"
	@echo "var2 => $(var2)"
	@echo "var3 => $(var3)"
	@echo "var4 => $(var4)"
