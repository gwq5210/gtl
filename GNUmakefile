ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ALL_TARGETS := all base check install package clean
MAKE_FILE := Makefile

DEFAULT_BUILD_DIR := build
DEFAULT_PREFIX := /opt/sogou
BUILD_DIR := $(shell if [ -f $(MAKE_FILE) ]; then echo "."; else echo $(DEFAULT_BUILD_DIR); fi)
CMAKE3 := $(shell if which cmake3>/dev/null ; then echo cmake3; else echo cmake; fi;)
CMAKE_OPTIONS :=
ifeq ($(PREFIX),)
	PREFIX := $(DEFAULT_PREFIX)
endif
ifeq ($(DEBUG),y)
	CMAKE_OPTIONS := $(CMAKE_OPTIONS) -D CMAKE_BUILD_TYPE=Debug
endif
ifneq ($(PREFIX),)
	CMAKE_OPTIONS := $(CMAKE_OPTIONS) -D CMAKE_INSTALL_PREFIX=$(PREFIX)
endif

.PHONY: $(ALL_TARGETS)

all: base
	make -C $(BUILD_DIR) -f Makefile

base:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE3) $(CMAKE_OPTIONS) $(ROOT_DIR)

check: all
	make -C test check

benchmarks: all
	make -C benchmarks run

install: base
	make -C $(BUILD_DIR) -f Makefile $@

package: base
	rm -f ./*.rpm
	make -C $(BUILD_DIR) -f Makefile $@
ifneq ($(BUILD_DIR),.)
	mv $(BUILD_DIR)/*.rpm ./
endif
	mv dist/*noarch.rpm ./

clean:
ifeq (build, $(wildcard build))
	-make -C build clean
endif
	-make -C test clean
	rm -rf $(DEFAULT_BUILD_DIR)
	rm -rf include
	rm -rf lib
	rm -rf bin
	rm -f SRCINFO SRCNUMVER SRCVERSION
