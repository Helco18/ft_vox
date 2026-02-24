# Compilation
CC					=	g++
CFLAGS				=	-Wall -Wextra -Werror --std=c++17 -MP -MMD -march=native -O3 -ffast-math -fno-rtti -pthread
# CFLAGS				=	-Wall -Wextra -Werror -g --std=c++17 -MP -MMD -fsanitize=address -ffast-math -fno-rtti -pthread
LDFLAGS				=	-lvulkan -lGL -ldl

# Names
NAME				=	ft_vox

# Paths
SHADER_PATH			=	resources/shaders
OBJ_PATH			=	objs
GLAD_PATH			=	libs/glad
GLFW_PATH			=	libs/GLFW
SPIRV_PATH			=	$(SHADER_PATH)/spir-v
GLSL_PATH			=	$(SHADER_PATH)/glsl

# Sources
SRCS				=	srcs/main.cpp \
						srcs/core/camera/Camera.cpp \
						srcs/core/camera/CameraFrustum.cpp \
						srcs/core/engine/opengl/OpenGLAsset.cpp \
						srcs/core/engine/opengl/OpenGLDebug.cpp \
						srcs/core/engine/opengl/OpenGLEngine.cpp \
						srcs/core/engine/opengl/OpenGLImGui.cpp \
						srcs/core/engine/opengl/OpenGLPipeline.cpp \
						srcs/core/engine/opengl/OpenGLShader.cpp \
						srcs/core/engine/opengl/OpenGLTexture.cpp \
						srcs/core/engine/opengl/OpenGLUniform.cpp \
						srcs/core/engine/vulkan/VulkanAsset.cpp \
						srcs/core/engine/vulkan/VulkanCommands.cpp \
						srcs/core/engine/vulkan/VulkanDebug.cpp \
						srcs/core/engine/vulkan/VulkanDepthTest.cpp \
						srcs/core/engine/vulkan/VulkanDescriptors.cpp \
						srcs/core/engine/vulkan/VulkanEngine.cpp \
						srcs/core/engine/vulkan/VulkanImGui.cpp \
						srcs/core/engine/vulkan/VulkanImage.cpp \
						srcs/core/engine/vulkan/VulkanInstance.cpp \
						srcs/core/engine/vulkan/VulkanMesh.cpp \
						srcs/core/engine/vulkan/VulkanMultisampling.cpp \
						srcs/core/engine/vulkan/VulkanPipeline.cpp \
						srcs/core/engine/vulkan/VulkanSyncObjects.cpp \
						srcs/core/engine/vulkan/VulkanSwapchain.cpp \
						srcs/core/engine/vulkan/VulkanTexture.cpp \
						srcs/core/engine/vulkan/VulkanUniform.cpp \
						srcs/core/input/InputManager.cpp \
						srcs/core/model/OBJModel.cpp \
						srcs/core/pipeline/PipelineManager.cpp \
						srcs/core/texture/TextureAtlas.cpp \
						srcs/core/thread/ThreadPool.cpp \
						srcs/core/thread/ThreadWorker.cpp \
						srcs/core/window/WindowCreation.cpp \
						srcs/core/window/WindowManager.cpp \
						srcs/blockoverlay/BlockOverlay.cpp \
						srcs/crosshair/Crosshair.cpp \
						srcs/entity/player/Player.cpp \
						srcs/environment/Environment.cpp \
						srcs/skybox/Skybox.cpp \
						srcs/tools/logger/Logger.cpp \
						srcs/tools/gui/Gui.cpp \
						srcs/tools/profiler/Profiler.cpp \
						srcs/tools/utils/stb_image.cpp \
						srcs/tools/utils/utils.cpp \
						srcs/voxel/WorldManager.cpp \
						srcs/voxel/biome/BiomeManager.cpp \
						srcs/voxel/biome/biomeTypes/DesertBiome.cpp \
						srcs/voxel/biome/biomeTypes/IceSpikesBiome.cpp \
						srcs/voxel/biome/biomeTypes/MesaBiome.cpp \
						srcs/voxel/biome/biomeTypes/MountainsBiome.cpp \
						srcs/voxel/biome/biomeTypes/OceanBiome.cpp \
						srcs/voxel/biome/biomeTypes/PlainsBiome.cpp \
						srcs/voxel/biome/biomeTypes/TundraBiome.cpp \
						srcs/voxel/block/BlockData.cpp \
						srcs/voxel/chunk/proceduralTools/heightMap/HeightMap.cpp \
						srcs/voxel/chunk/proceduralTools/terrainGenerator/TerrainGenerator.cpp \
						srcs/voxel/chunk/Chunk.cpp \
						srcs/voxel/chunk/ChunkGreedyMeshing.cpp \
						srcs/voxel/world/World.cpp \
						srcs/voxel/world/WorldGeneration.cpp \
						srcs/voxel/world/WorldRayCaster.cpp \
						srcs/voxel/world/WorldUtils.cpp
LIBS_C				=	libs/glad/gl.c
LIBS_CPP			=	libs/imgui/imgui_demo.cpp \
						libs/imgui/imgui_draw.cpp \
						libs/imgui/imgui_tables.cpp \
						libs/imgui/imgui_impl_glfw.cpp \
						libs/imgui/imgui_impl_opengl3.cpp \
						libs/imgui/imgui_impl_vulkan.cpp \
						libs/imgui/imgui_widgets.cpp \
						libs/imgui/imgui.cpp
SHADERS_SRC			=	$(SHADER_PATH)/slang/blockoverlay.slang \
						$(SHADER_PATH)/slang/crosshair.slang \
						$(SHADER_PATH)/slang/lines.slang \
						$(SHADER_PATH)/slang/skybox.slang \
						$(SHADER_PATH)/slang/voxel.slang
SPIRV_OBJS			=	$(patsubst $(SHADER_PATH)/slang/%.slang, $(SPIRV_PATH)/%.spv, $(SHADERS_SRC))
GLSL_VERT_OBJS		=	$(patsubst $(SHADER_PATH)/slang/%.slang, $(GLSL_PATH)/%.vert, $(SHADERS_SRC))
GLSL_FRAG_OBJS		=	$(patsubst $(SHADER_PATH)/slang/%.slang, $(GLSL_PATH)/%.frag, $(SHADERS_SRC))

# Includes
INCLUDES			=	-I libs \
						-I libs/glad \
						-I libs/GLFW \
						-I libs/glm \
						-I libs/imgui \
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
						-I srcs/core/thread \
						-I srcs/core/window \
						-I srcs/blockoverlay \
						-I srcs/crosshair \
						-I srcs/entity \
						-I srcs/entity/player \
						-I srcs/environment \
						-I srcs/skybox \
						-I srcs/tools \
						-I srcs/tools/exceptions \
						-I srcs/tools/gui \
						-I srcs/tools/logger \
						-I srcs/tools/profiler \
						-I srcs/tools/utils \
						-I srcs/voxel \
						-I srcs/voxel/biome \
						-I srcs/voxel/biome/biomeTypes \
						-I srcs/voxel/block \
						-I srcs/voxel/chunk \
						-I srcs/voxel/chunk/proceduralTools/noise \
						-I srcs/voxel/chunk/proceduralTools/heightMap \
						-I srcs/voxel/chunk/proceduralTools/terrainGenerator \
						-I srcs/voxel/world
GLFW				=	$(GLFW_PATH)/libglfw3.a

# Objects
OBJS				=	$(patsubst srcs/%.cpp, $(OBJ_PATH)/%.o, $(SRCS)) \
						$(patsubst libs/%.cpp, $(OBJ_PATH)/%.o, $(LIBS_CPP)) \
						$(patsubst libs/%.c, $(OBJ_PATH)/%.o, $(LIBS_C))

DEPS				=	$(patsubst srcs/%.cpp, $(OBJ_PATH)/%.d, $(SRCS)) \
						$(patsubst libs/%.cpp, $(OBJ_PATH)/%.d, $(LIBS_CPP)) \
						$(patsubst libs/%.c, $(OBJ_PATH)/%.d, $(LIBS_C))

# Custom Makefile Flags
MAKEFLAGS			+=	--no-print-directory --silent

# Custom Colors
PURPLE				=	\033[1;35m
YELLOW				=	\033[33;1m
LIGHT_GREEN			=	\033[1;32m
RESET				=	\033[0m

# Custom messages
EXE_DONE			=	@echo "[PROGRAM] $(PURPLE)$(NAME) compiled!$(RESET)"
SHADERS_DONE		=	@echo "[SHADER] $(PURPLE)Shaders compiled!$(RESET)"
SHADERS_CLEAN		=	@echo "[SHADER] $(YELLOW)Shaders cleaned!$(RESET)"
ALL_CLEAN			=	@echo "[SRCS] $(YELLOW)Project's objects cleaned!$(RESET)"
ALL_FCLEAN			=	@echo "[SRCS] $(YELLOW)Project's objects & Executables cleaned!$(RESET)"

# Rules
all : glfw glm stb glad imgui shader $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLFW) -o $(NAME) $(INCLUDES) $(LDFLAGS)
	$(EXE_DONE)

$(OBJ_PATH)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "[SRCS] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

$(OBJ_PATH)/%.o: libs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "[SRCS] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

$(OBJ_PATH)/%.o: libs/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "[SRCS] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

run: all
	./$(NAME)

# SPV (Vulkan)
$(SPIRV_PATH)/%.spv: $(SHADER_PATH)/slang/%.slang
	@mkdir -p $(SPIRV_PATH)
	@slangc $< -target spirv -D __VULKAN__ -entry vertMain -entry fragMain -o $@
	@echo "[SHADER-SPRV] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

# VERT (OpenGL)
$(GLSL_PATH)/%.vert: $(SHADER_PATH)/slang/%.slang
	@mkdir -p $(GLSL_PATH)
	@slangc $< -profile glsl_330 -target glsl -entry vertMain -o $@
	@echo "[SHADER-GLSL] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

# FRAG (OpenGL)
$(GLSL_PATH)/%.frag: $(SHADER_PATH)/slang/%.slang
	@mkdir -p $(GLSL_PATH)
	@slangc $< -profile glsl_330 -target glsl -entry fragMain -o $@
	@echo "[SHADER-GLSL] $(LIGHT_GREEN)Compiled: $@ $(RESET)"

shader: $(SPIRV_OBJS) $(GLSL_VERT_OBJS) $(GLSL_FRAG_OBJS)
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
	@rm -f $(NAME)
	@rm -rf $(OBJ_PATH)
	@rm -rf $(SPIRV_PATH)
	@rm -rf $(GLSL_PATH)
	$(ALL_FCLEAN)

re:
	@$(MAKE) fclean
	@$(MAKE) all

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
	mkdir -p stb; \
	mv STB/* stb/; \
	rm -rf STB;

stb:
	@if ! ls ./libs/stb 2>/dev/null | grep -q "stb_image.h" ; then \
		mkdir -p libs; \
		make --no-print-directory download_stb; \
	fi

download_glad:
	@cd libs; \
	echo "\033[31;1;4mglad Not Found\033[0m"; \
	echo "\033[31;1mDownloading it from my google drive which is very unsafe and needs to be changed but I'm too lazy right now\033[0m"; \
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1m5uKvKhd8hvi6pQ7lvgL6U9O5RIkBtm2' -O glad.tar.gz; \
	tar -xpf glad.tar.gz; \
	rm -rf glad.tar.gz;

glad:
	@if ! ls ./libs/glad 2>/dev/null | grep -q "gl.h" ; then \
		mkdir -p libs; \
		make --no-print-directory download_glad; \
	fi

download_imgui:
	@cd libs; \
	rm -rf imgui_downloaded 2>/dev/null; \
	echo "\033[31;1;4mImGUI Not Found\033[0m"; \
	echo "\033[31;1mDownloading it from github\033[0m"; \
	git clone https://github.com/ocornut/imgui.git imgui_downloaded; \
	mkdir imgui; \
	mv imgui_downloaded/imgui.h imgui/; \
	mv imgui_downloaded/imconfig.h imgui/; \
	mv imgui_downloaded/imgui_internal.h imgui/; \
	mv imgui_downloaded/imstb_rectpack.h imgui/; \
	mv imgui_downloaded/imstb_textedit.h imgui/; \
	mv imgui_downloaded/imstb_truetype.h imgui/; \
	mv imgui_downloaded/imgui_demo.cpp imgui/; \
	mv imgui_downloaded/imgui_draw.cpp imgui/; \
	mv imgui_downloaded/imgui_tables.cpp imgui/; \
	mv imgui_downloaded/imgui_widgets.cpp imgui/; \
	mv imgui_downloaded/imgui.cpp imgui/; \
	mv imgui_downloaded/backends/imgui_impl_glfw.cpp imgui/; \
	mv imgui_downloaded/backends/imgui_impl_glfw.h imgui/; \
	mv imgui_downloaded/backends/imgui_impl_vulkan.cpp imgui/; \
	mv imgui_downloaded/backends/imgui_impl_vulkan.h imgui/; \
	mv imgui_downloaded/backends/imgui_impl_opengl3.cpp imgui/; \
	mv imgui_downloaded/backends/imgui_impl_opengl3.h imgui/; \
	mv imgui_downloaded/backends/imgui_impl_opengl3_loader.h imgui/; \
	rm -rf imgui_downloaded 2>/dev/null;

imgui:
	@mkdir -p libs
	@if ! ls ./libs/imgui 2>/dev/null | grep -q "imgui.h" ; then \
		make --no-print-directory download_imgui; \
	fi

.PHONY: all shader clean sclean fclean re stb download_stb glm download_glm glfw download_glfw glad download_glad imgui download_imgui

-include $(DEPS)
