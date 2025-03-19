CURRENT_DIR := $(shell pwd)
BUILD_TYPE := Release		# Release Debug
BUILD_DIR := _build

build:
	@mkdir -p ${BUILD_DIR}
	@cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES="conan_provider.cmake" \
	-S ${CURRENT_DIR} -B ${BUILD_DIR}
	@cmake --build ${BUILD_DIR} --parallel 8

run:
	${BUILD_DIR}/ctask

run_test:
	${BUILD_DIR}/test/test

clear:
	@rm -rf ${BUILD_DIR}

clear_all: clear
	@conan remove "*" --confirm -vtrace
