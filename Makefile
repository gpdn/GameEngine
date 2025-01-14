
CC=gcc
FLAGS=-Wextra -Wall
BIN=./main
SRC=./src
OBJ=./obj

VULKAN = C:\VulkanSDK\1.3.290.0

SOURCES=$(wildcard ${SRC}/*.c)
SUB_FOLDERS=$(wildcard ${SRC}/*/)

SUB_FOLDERS_SOURCES=$(wildcard $(addsuffix *.c, ${SUB_FOLDERS}))
SUB_FOLDERS_OBJECTS=$(filter %.o, $(foreach F, ${SUB_FOLDERS}, $(patsubst $F%.c, ${OBJ}/%.o, ${SUB_FOLDERS_SOURCES})))
OBJECTS=$(patsubst ${SRC}/%.c, ${OBJ}/%.o, ${SOURCES}) ${SUB_FOLDERS_OBJECTS}
INCLUDES= -I./${SRC}/ -I${VULKAN}/Include
LIBRARIES= -L${VULKAN}/Lib -lvulkan-1

$(info $$SUB_FOLDERS = $(SUB_FOLDERS))
$(info $$SUB_FOLDERS_SOURCES = $(SUB_FOLDERS_SOURCES))
$(info $$SUB_FOLDERS_OBJECTS = $(SUB_FOLDERS_OBJECTS))
$(info $$OBJECTS = $(OBJECTS))

all: ${BIN}

${BIN}: ${OBJECTS}
	${CC} ${INCLUDES} ${FLAGS} $^ -o $@ ${LIBRARIES}

${OBJ}/%.o: ${SRC}/%.c
	${CC} ${INCLUDES} ${FLAGS} -c $< -o $@

${OBJ}/%.o: ${SRC}/*/%.c
	${CC} ${INCLUDES} ${FLAGS} -c $< -o $@

clear:
	rm -r ${OBJ}/* 