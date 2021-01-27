.PHONY : test

define func1
	@echo "my name is $(0)"
endef

define func2
	@echo "my name is $(0)"
endef

test:
	$(call func1)
	$(call func2)
