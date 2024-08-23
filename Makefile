cc := $(CROSS_COMPILE)gcc
#CROSS_COMPILE 交叉编译
log:=thirdparty/log/log.h thirdparty/log/log.c

cjson:=thirdparty/cJSON/cJSON.h thirdparty/cJSON/cJSON.c

#CFLAGS 在 makefile 中用于定义编译器的参数。它是一个环境变量，用于传递给编译器的命令行选项。
#包括优化选项、警告选项、宏定义等
CFLAGS := -Wall -Wextra

INC_DIRS += /home/amu/桌面/gateway

SRCS += $(shell find app -name '*.c')
SRCS += $(shell find daemon -name '*.c')
SRCS += $(shell find ota -name '*.c')
SRCS += $(shell find thirdparty -name '*.c')

OBJS := $(SRCS:.c=.o)

#$(addprefix <prefix>,<names...>)
#把前缀 <prefix> 加到 <names> 中的每个单词后面
CFLAGS += $(addprefix -I,$(INC_DIRS))


TARGET := gateway



$(TARGET):$(OBJS)
	

clean:
	@$(RM) -f $(OBJS) $(TARGET)


buffer_test: $(OBJS) test/buffer_test.o
	-@$(CC) $(CFLAGS) $^ -o $@
	-@./$@
	-@$(RM) $@




cjson_test:test/cjson_test.c $(cjson)
	-@$(cc) -o $@ $^ -Ithirdparty
	-@./$@
	-@rm ./$@



log_test:test/log_test.c $(log)
	-@$(cc) -o $@ $^ -Ithirdparty
	-@./$@
	-@rm ./$@



