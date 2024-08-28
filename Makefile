CC := $(CROSS_COMPILE)gcc
#CROSS_COMPILE 交叉编译

PEER := root@192.168.44.61

#CFLAGS 在 makefile 中用于定义编译器的参数。它是一个环境变量，用于传递给编译器的命令行选项。
#包括优化选项、警告选项、宏定义等
CFLAGS := -Wall -Wextra


ifdef DEBUG
CFLAGS += -ggdb
endif


ifdef SYSROOT
CFLAGS += --sysroot $(SYSROOT)
endif

INC_DIRS += /home/amu/桌面/gateway

SRCS += $(shell find app -name '*.c')
SRCS += $(shell find daemon -name '*.c')
SRCS += $(shell find ota -name '*.c')
SRCS += $(shell find thirdparty -name '*.c')



OBJS := $(SRCS:.c=.o)

#$(addprefix <prefix>,<names...>)
#把前缀 <prefix> 加到 <names> 中的每个单词后面
CFLAGS += $(addprefix -I,$(INC_DIRS))

LDFLAGS += -lpaho-mqtt3c 

TARGET := gateway



$(TARGET):$(OBJS)

ggdb: $(OBJS) test/buffer_test.o
	-@$(CC) $(CFLAGS) $^ -o $@

clean:
	@$(RM) -f $(OBJS) $(TARGET)


message_test: $(OBJS) test/message_test.o
	-@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	-@./$@
	-@$(RM) $@


buffer_test: $(OBJS) test/buffer_test.o
	-@$(CC) $(CFLAGS) $^ -o $@
	-@./$@
	-@$(RM) $@


mqtt_test: $(OBJS) test/mqtt_test.o
	-@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	-@./$@
	-@$(RM) $@


cross:
	@CROSS_COMPILE=/桌面/board/toolchain/bin/arm-linux-gnueabihf- \
	SYSROOT=/桌面/board/sysroot \
	make all
	@scp $(TARGET) $(PEER):/usr/bin/gateway

