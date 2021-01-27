.PHONY : test

define func1
	@echo "my name is $(0)"
endef

func2 := @echo "my name is $(0)"

test:
	$(call func1)
	$(call func2)
