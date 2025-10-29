# Compilation
CC					=	g++
CFLAGS				=	-Wall -Wextra -Werror -g --std=c++17 -MP -MMD
LDFLAGS				=	-lvulkan -lGL -lGLEW

# Names
NAME				=	ft_vox

# Sources & Includes
SRCS				=	srcs/main.cpp \
						srcs/camera/Camera.cpp \
						srcs/engine/opengl/OpenGLEngine.cpp \
						srcs/engine/opengl/OpenGLShader.cpp \
						srcs/engine/opengl/OpenGLTexture.cpp \
						srcs/engine/opengl/OpenGLUniform.cpp \
						srcs/engine/vulkan/VulkanCommands.cpp \
						srcs/engine/vulkan/VulkanDebug.cpp \
						srcs/engine/vulkan/VulkanDepthTest.cpp \
						srcs/engine/vulkan/VulkanDescriptors.cpp \
						srcs/engine/vulkan/VulkanEngine.cpp \
						srcs/engine/vulkan/VulkanInstance.cpp \
						srcs/engine/vulkan/VulkanMesh.cpp \
						srcs/engine/vulkan/VulkanPipeline.cpp \
						srcs/engine/vulkan/VulkanSwapchain.cpp \
						srcs/engine/vulkan/VulkanTexture.cpp \
						srcs/engine/vulkan/VulkanUniform.cpp \
						srcs/input/InputManager.cpp \
						srcs/model/OBJModel.cpp \
						srcs/tools/glfwUtils.cpp \
						srcs/tools/stb_image.cpp \
						srcs/tools/utils.cpp \
						srcs/window/WindowManager.cpp

SHADERS				=	srcs/shaders/slang/triangle.slang
OBJ_FOLDER			=	objs
INCLUDES			=	-I libs \
						-I libs/stb \
						-I srcs \
						-I srcs/camera \
						-I srcs/engine \
						-I srcs/engine/opengl \
						-I srcs/engine/vulkan \
						-I srcs/input \
						-I srcs/model \
						-I srcs/tools \
						-I srcs/window

GLFW_PATH			=	libs/GLFW
GLEW_PATH			=	libs/GLEW
GLM_PATH			=	libs/glm
STB_PATH			=	libs/stb
SPIRV_PATH			=	srcs/shaders/spir-v
GLSL_PATH			=	srcs/shaders/glsl

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
SHADERS_DONE		=	@echo "🎨$(PURPLE) Shaders compiled! 🎨$(RESET)\n"
SHADERS_CLEAN		=	@echo "🧹$(LIGHT_GREEN) Shaders cleaned! 🧹$(RESET)\n"
ALL_CLEAN			=	@echo "🧹$(LIGHT_GREEN) Project's objects cleaned! 🧹$(RESET)\n"
ALL_FCLEAN			=	@echo "🧹$(LIGHT_GREEN) Project's objects & Executables cleaned! 🧹$(RESET)\n"

# Rules

all : glfw glm stb shader $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLFW) -o $(NAME) $(INCLUDES) $(LDFLAGS)
	$(EXE_DONE)

$(OBJ_FOLDER)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(NAME)

$(SPIRV_PATH)/triangle.spv : $(SHADERS)
	@mkdir -p $(SPIRV_PATH)
	@slangc $(SHADERS) -target spirv -entry vertMain -entry fragMain -o $@

$(GLSL_PATH)/triangle.vert : $(SHADERS)
	@mkdir -p $(GLSL_PATH)
	@slangc $(SHADERS) -target glsl -profile glsl_330 -entry vertMain -o $@

$(GLSL_PATH)/triangle.frag : $(SHADERS)
	@mkdir -p $(GLSL_PATH)
	@slangc $(SHADERS) -target glsl -profile glsl_330 -entry fragMain -o $@

shader: $(SPIRV_PATH)/triangle.spv $(GLSL_PATH)/triangle.vert $(GLSL_PATH)/triangle.frag
	$(SHADERS_DONE)

sclean:
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(SHADERS_CLEAN)

clean :
	@rm -rf $(OBJ_FOLDER)
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(ALL_CLEAN)

fclean :
	@rm -f $(NAME)_debug
	@rm -f $(NAME)
	@rm -rf $(OBJ_FOLDER)
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(ALL_FCLEAN)

re:
	@$(MAKE) fclean
	@$(MAKE) all

debug : fclean
	$(MAKE) NAME="$(NAME)_debug" CFLAGS="$(CFLAGS) -DDEBUG"
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
	cp GLFW_DOWNLOADED/include/GLFW/glfw3native.h GLFW/.; \
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

download_stb:
	@cd libs; \
	echo "\033[31;1;4mstb Not Found\033[0m"; \
	echo "\033[31;1mDownloading it from github\033[0m"; \
	git clone https://github.com/nothings/stb.git STB; \
	echo "\033[31;1mGetting the Headers\033[0m"; \
	mkdir -p stb; \
	mv STB/* stb/; \
	rm -rf STB;

stb:
	@mkdir -p libs
	@if ! ls ./libs/stb 2>/dev/null | grep -q "stb_image.h" ; then \
		make --no-print-directory download_stb; \
	fi

.PHONY: all shader clean sclean fclean re debug stb download_stb glm download_glm glfw download_glfw

-include $(DEPS)
