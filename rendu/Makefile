# Compilation
CC					=	g++
CFLAGS				=	-Wall -Wextra -Werror -g --std=c++17 -MP -MMD
LDFLAGS				=	-lvulkan

# Names
NAME				=	ft_vox

# Sources & Includes
SRCS				=	srcs/main.cpp \
						srcs/tools/vulkan_utils.cpp \
						srcs/vulkan/VulkanEngine.cpp
OBJ_FOLDER			=	objs
INCLUDES			=	-I srcs \
						-I libs

GLFW_PATH			=	libs/GLFW
GLM_PATH			=	libs/glm

GLFW				=	$(GLFW_PATH)/libglfw3.a

# Objects
OBJS				=	$(patsubst srcs/%.cpp, $(OBJ_FOLDER)/%.o, $(SRCS)) \
						$(patsubst %.c, $(OBJ_FOLDER)/%.o, $(LIBS)) \
						$(patsubst %.cpp, $(OBJ_FOLDER)/%.o, $(LIBS))

DEPS				=	$(patsubst srcs/%.cpp, $(OBJ_FOLDER)/%.d, $(SRCS)) \
						$(patsubst %.c, $(OBJ_FOLDER)/%.d, $(LIBS)) \
						$(patsubst %.cpp, $(OBJ_FOLDER)/%.d, $(LIBS))

# Custom Makefile Flags
MAKEFLAGS			+=	--no-print-directory --silent

# Custom Colors
PURPLE				=	\033[1;35m
YELLOW				=	\033[33;1m
LIGHT_GREEN			=	\033[1;32m
RESET				=	\033[0m

# Custom messages
EXE_DONE			=	@echo "🎉$(PURPLE) $(NAME) compiled! 🎉$(RESET)\n"
DEBUG_DONE			=	@echo "🎉$(YELLOW) This is the DEBUG MODE version! 🎉$(RESET)\n"
ALL_CLEAN			=	@echo "🧹$(LIGHT_GREEN) Project's objects cleaned! 🧹$(RESET)\n"
ALL_FCLEAN			=	@echo "🧹$(LIGHT_GREEN) Project's objects & Executables cleaned! 🧹$(RESET)\n"

# Rules

all : glfw glm
	@make -j4 $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLFW) -o $(NAME) $(INCLUDES) ${LDFLAGS}
	$(EXE_DONE)

$(OBJ_FOLDER)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	@rm -rf $(OBJ_FOLDER)
	$(ALL_CLEAN)

fclean :
	@rm -f $(NAME)_debug
	@rm -f $(NAME)
	@rm -rf $(OBJ_FOLDER)
	$(ALL_FCLEAN)

re : fclean all

debug : fclean
	$(MAKE) -j4 NAME="$(NAME)_debug" CFLAGS="$(CFLAGS) -DDEBUG"
	$(DEBUG_DONE)

# Libraries
download_glfw:
	@cd libs; \
	rm -rf GLFW 2>/dev/null; \
	echo "\033[31;1;4mglfw Not Found\033[0m"; \
	echo "\033[31;1mDownloading it from github\033[0m"; \
	git clone https://github.com/glfw/glfw.git GLFW_DOWNLOADED; \
	echo "\033[31;1mCompiling it\033[0m"; \
	cmake -S GLFW_DOWNLOADED -B GLFW_DOWNLOADED/build; \
	cmake --build GLFW_DOWNLOADED/build; \
	mkdir -p GLFW; \
	cp GLFW_DOWNLOADED/include/GLFW/glfw3.h GLFW/.; \
	cp GLFW_DOWNLOADED/build/src/libglfw3.a GLFW/.; \
	rm -rf GLFW_DOWNLOADED

glfw:
	@mkdir -p libs
	@if ! ls ./libs/GLFW 2>/dev/null | grep -q "glfw3.h" ; then \
		make --no-print-directory download_glfw; \
	fi
	@if ! ls ./libs/GLFW 2>/dev/null | grep -q "libglfw3.a" ; then \
		make --no-print-directory download_glfw; \
	fi

download_glm:
	@cd libs; \
	echo "\033[31;1;4mglm Not Found\033[0m"; \
	echo "\033[31;1mDownloading it from github\033[0m"; \
	git clone https://github.com/g-truc/glm.git GLM; \
	echo "\033[31;1mGetting the Headers\033[0m"; \
	mkdir -p glm; \
	mv GLM/glm .; \
	rm -rf GLM

glm:
	@mkdir -p libs
	@if ! ls ./libs/glm 2>/dev/null | grep -q "glm.hpp" ; then \
		make --no-print-directory download_glm; \
	fi

.PHONY: all clean fclean re debug

-include $(DEPS)
