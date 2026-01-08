# Compilation
CC					=	g++
CFLAGS				=	-Wall -Wextra -Werror -g --std=c++17 -MP -MMD
LDFLAGS				=	-lvulkan -lGL -lGLEW

# Names
NAME				=	ft_vox

# Paths
SHADER_PATH			=	srcs/core/shaders
OBJ_PATH			=	objs
GLFW_PATH			=	libs/GLFW
GLM_PATH			=	libs/glm
STB_PATH			=	libs/stb
SPIRV_PATH			=	$(SHADER_PATH)/spir-v
GLSL_PATH			=	$(SHADER_PATH)/glsl

# Sources
SRCS				=	srcs/main.cpp \
						srcs/core/camera/Camera.cpp \
						srcs/core/engine/opengl/OpenGLEngine.cpp \
						srcs/core/engine/opengl/OpenGLShader.cpp \
						srcs/core/engine/opengl/OpenGLTexture.cpp \
						srcs/core/engine/opengl/OpenGLUniform.cpp \
						srcs/core/engine/vulkan/VulkanCommands.cpp \
						srcs/core/engine/vulkan/VulkanDebug.cpp \
						srcs/core/engine/vulkan/VulkanDepthTest.cpp \
						srcs/core/engine/vulkan/VulkanDescriptors.cpp \
						srcs/core/engine/vulkan/VulkanEngine.cpp \
						srcs/core/engine/vulkan/VulkanImage.cpp \
						srcs/core/engine/vulkan/VulkanInstance.cpp \
						srcs/core/engine/vulkan/VulkanMesh.cpp \
						srcs/core/engine/vulkan/VulkanPipeline.cpp \
						srcs/core/engine/vulkan/VulkanSwapchain.cpp \
						srcs/core/engine/vulkan/VulkanTexture.cpp \
						srcs/core/engine/vulkan/VulkanUniform.cpp \
						srcs/core/input/InputManager.cpp \
						srcs/core/model/OBJModel.cpp \
						srcs/core/pipeline/PipelineManager.cpp \
						srcs/core/texture/TextureAtlas.cpp \
						srcs/core/window/WindowCreation.cpp \
						srcs/core/window/WindowManager.cpp \
						srcs/environment/Environment.cpp \
						srcs/tools/stb_image.cpp \
						srcs/tools/utils.cpp \
						srcs/tools/logger/Logger.cpp \
						srcs/voxel/WorldManager.cpp \
						srcs/voxel/block/BlockData.cpp \
						srcs/voxel/chunk/Chunk.cpp \
						srcs/voxel/chunk/ChunkGreedyMeshing.cpp \
						srcs/voxel/world/World.cpp
SHADERS				=	$(SHADER_PATH)/slang/voxel.slang

# Includes
INCLUDES			=	-I libs \
						-I libs/stb \
						-I srcs \
						-I srcs/core \
						-I srcs/core/camera \
						-I srcs/core/engine \
						-I srcs/core/engine/opengl \
						-I srcs/core/engine/vulkan \
						-I srcs/core/input \
						-I srcs/core/model \
						-I srcs/core/pipeline \
						-I srcs/core/texture \
						-I srcs/core/window \
						-I srcs/environment \
						-I srcs/tools \
						-I srcs/tools/exceptions \
						-I srcs/tools/logger \
						-I srcs/voxel \
						-I srcs/voxel/block \
						-I srcs/voxel/chunk \
						-I srcs/voxel/world

GLFW				=	$(GLFW_PATH)/libglfw3.a

# Objects
OBJS				=	$(patsubst srcs/%.cpp, $(OBJ_PATH)/%.o, $(SRCS)) \
						$(patsubst %.c, $(OBJ_PATH)/%.o, $(LIBS)) \
						$(patsubst %.cpp, $(OBJ_PATH)/%.o, $(LIBS))

DEPS				=	$(patsubst srcs/%.cpp, $(OBJ_PATH)/%.d, $(SRCS)) \
						$(patsubst %.c, $(OBJ_PATH)/%.d, $(LIBS)) \
						$(patsubst %.cpp, $(OBJ_PATH)/%.d, $(LIBS))

# Custom Makefile Flags
MAKEFLAGS			+=	--no-print-directory --silent

# Custom Colors
PURPLE				=	\033[1;35m
YELLOW				=	\033[33;1m
LIGHT_GREEN			=	\033[1;32m
RESET				=	\033[0m

# Custom messages
EXE_DONE			=	@echo "[PROGRAM] $(PURPLE)$(NAME) compiled!$(RESET)"
DEBUG_DONE			=	@echo "[PROGRAM] $(YELLOW)This is the DEBUG MODE version!$(RESET)"
SHADERS_DONE		=	@echo "[SHADER] $(PURPLE)Shaders compiled!$(RESET)"
SHADERS_CLEAN		=	@echo "[SHADER] $(LIGHT_GREEN)Shaders cleaned!$(RESET)"
ALL_CLEAN			=	@echo "[SRCS] $(LIGHT_GREEN)Project's objects cleaned!$(RESET)"
ALL_FCLEAN			=	@echo "[SRCS] $(LIGHT_GREEN)Project's objects & Executables cleaned!$(RESET)"

# Rules

all : glfw glm stb shader $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLFW) -o $(NAME) $(INCLUDES) $(LDFLAGS)
	$(EXE_DONE)

$(OBJ_PATH)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "[SRCS] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

run: all
	./$(NAME)

$(SPIRV_PATH)/voxel.spv : $(SHADERS)
	@mkdir -p $(SPIRV_PATH)
	@slangc $(SHADERS) -target spirv -entry vertMain -entry fragMain -o $@
	@echo "[SHADER] SPIRV: $(YELLOW)Compiled: $@ $(RESET)"

$(GLSL_PATH)/voxel.vert : $(SHADERS)
	@mkdir -p $(GLSL_PATH)
	@slangc $(SHADERS) -target glsl -profile glsl_330 -entry vertMain -o $@
	@echo "[SHADER] GLSL_VERTEX: $(YELLOW)Compiled: $@ $(RESET)"

$(GLSL_PATH)/voxel.frag : $(SHADERS)
	@mkdir -p $(GLSL_PATH)
	@slangc $(SHADERS) -target glsl -profile glsl_330 -entry fragMain -o $@
	@echo "[SHADER] GLSL_FRAGMENT: $(YELLOW)Compiled: $@ $(RESET)"

shader: $(SPIRV_PATH)/voxel.spv $(GLSL_PATH)/voxel.vert $(GLSL_PATH)/voxel.frag
	$(SHADERS_DONE)

sclean:
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(SHADERS_CLEAN)

clean :
	@rm -rf $(OBJ_PATH)
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(ALL_CLEAN)

fclean :
	@rm -f $(NAME)_debug
	@rm -f $(NAME)
	@rm -rf $(OBJ_PATH)
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
	@if ! ls ./libs/GLFW 2>/dev/null | grep -q "glfw3.h" ; then \
		mkdir -p libs; \
		make --no-print-directory download_glfw; \
	fi
	@if ! ls ./libs/GLFW 2>/dev/null | grep -q "libglfw3.a" ; then \
		mkdir -p libs; \
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
	@if ! ls ./libs/glm 2>/dev/null | grep -q "glm.hpp" ; then \
		mkdir -p libs; \
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
	@if ! ls ./libs/stb 2>/dev/null | grep -q "stb_image.h" ; then \
		mkdir -p libs; \
		make --no-print-directory download_stb; \
	fi

.PHONY: all shader clean sclean fclean re debug stb download_stb glm download_glm glfw download_glfw

-include $(DEPS)
