
define func1
	@echo "my name is $(0)"
endef

var1  := $(call func1)
var11 := $(func1)

test :
	@echo "var1 => $(var1)"
	@echo "var11 => $(var11)"
