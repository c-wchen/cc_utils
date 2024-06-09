# radix_tree_example-y:=radix_tree_example.i

obj-m:=radix_tree_example.o	    	#根据make的自动推导原则，make会自动将源程序radix_tree_example.c编译成目标程序radix_tree_example.o
                                    #所有在配置文件中标记为-m的模块将被编译成可动态加载进内核的模块。即后缀为.ko的文件。
CURRENT_PATH:=${PWD}  		        #参数化，将模块源码路径保存在CURRENT_PATH中
# LINUX_KERNEL:=$(shell uname -r) 	#参数化，将当前内核版本保存在LINUX_KERNEL中
LINUX_KERNEL_VER:=4.18.0-552.el8.x86_64
LINUX_KERNEL_PATH:=/usr/src/kernels/${LINUX_KERNEL_VER} 	
                                    #参数化，将内核源代码的绝对路径保存在LINUX_KERNEL_PATH中

# 用于将过程文件输出到build目录下
BUILD_PATH=${PWD}/build
BUILD_PATH_MAKEFILE=${PWD}/build/Makefile



CONFIG_MODULE_SIG=n                

all:${BUILD_PATH_MAKEFILE}
	make -C $(LINUX_KERNEL_PATH) M=$(BUILD_PATH) src=${CURRENT_PATH} modules 	#编译模块
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(BUILD_PATH) src=${CURRENT_PATH} clean  	#清理
	rm -rf ${BUILD_PATH}

$(BUILD_PATH):
	mkdir -p "$@"

$(BUILD_PATH_MAKEFILE): $(BUILD_PATH)
	touch "$@"