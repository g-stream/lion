cc=gcc
build_dir=build
source_dir=src
vm_dir=${source_dir}/vm
cli_dir=${source_dir}/cli
include_dir=${source_dir}/include
module_dir=${source_dir}/module
test_dir=test
cflag=-g -Wall -I ${source_dir}/vm
vm_source=${wildcard ${vm_dir}/*.c}
vm_head=${wildcard ${vm_dir}/*.h}
vm_object=${addprefix ${build_dir}/vm/, ${patsubst %.c, %.o, ${notdir ${vm_source}}}}

cli_source=${wildcard ${cli_dir}/*.c}
cli_head=${wildcard ${cli_dir}/*.h}

test_source=${wildcard ${test_dir}/*.c}
test_object=${addprefix ${build_dir}/test/, ${patsubst %.c, %.o, ${notdir ${test_source}}}}
test_target=${patsubst %.o, %, ${test_object}}
all: test lion
test:${test_target}
${test_target}:%:%.o ${vm_object}
	${cc} -o $@ $^ ${cflag}
${test_object}:${build_dir}/test/%.o:${test_dir}/%.c ${vm_source} ${vm_head}
	${cc} -c $< ${cflag} -o $@
${vm_object}:${build_dir}/vm/%.o:${vm_dir}/%.c ${vm_source} ${vm_head}
	${cc} -c $< ${cflag} -o $@
lion:${cli_dir}/lion.c
	${cc} -o $@ $< ${cflag}
clean:
	rm ${build_dir}/*/*
.PHONY:test all
