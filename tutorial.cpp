VulkanEngine::VulkanEngine(GLFWwindow * window): _window(window)
{
	// On initialise une instance Vulkan, qui représente notre application au niveau de l'API Vulkan.
	// C'est le point d'entrée principal entre l'application et le driver Vulkan (on y précise nom, version, extensions, etc.)
	_createInstance();

	// On installe le système de debug pour capturer les erreurs, avertissements et validations pendant le développement.
	// Cela ne fait rien côté rendu, mais aide énormément à détecter les mauvaises utilisations de l’API.
	_initDebugMessenger();

	// On crée la surface d'affichage (VkSurfaceKHR), qui relie Vulkan à la fenêtre GLFW.
	// C’est via cette surface que Vulkan saura *où* présenter les images (interface avec le système de fenêtre).
	_createSurface();

	// On sélectionne un *physical device* (carte graphique) compatible avec Vulkan.
	// Ici, on parcourt les GPU disponibles et on choisit celui qui supporte les fonctionnalités et extensions nécessaires.
	_selectPhysicalDevice();

	// On vérifie que le GPU choisi supporte bien toutes les extensions dont on aura besoin
	// (par exemple VK_KHR_swapchain, nécessaire pour l’affichage).
	_checkDeviceExtensions();

	// On crée un *logical device*, c’est-à-dire une interface logique vers le GPU physique sélectionné.
	// On y active les *queue families* nécessaires (graphics, presentation, etc.) et certaines fonctionnalités matérielles optionnelles.
	_createLogicalDevice();

	// On crée une *swapchain* : c’est un ensemble d’images que Vulkan utilisera pour afficher les rendus à l’écran.
	// Elle permet d’alterner entre plusieurs buffers (double/triple buffering).
	_createSwapChain();

	// On crée des *image views* à partir des images de la swapchain.
	// Chaque image view décrit comment une image doit être interprétée (format, aspect, niveau de mipmap, etc.).
	_createImageViews();

	// On définit la structure de nos *descriptor sets*, qui servent à passer des données aux shaders
	// (comme les uniform buffers, textures ou samplers).
	_createDescriptorSetLayout();

	// On construit notre *graphics pipeline*, c’est-à-dire la configuration complète du pipeline graphique :
	// shaders, entrées vertex, assemblage, rasterization, blending, etc.
	_createGraphicsPipeline();

	// On crée deux *command pools* :
	// - un "reset" pool pour des commandes réutilisables (e.g. dessin, rendu)
	// - un "transient" pool pour des commandes temporaires (e.g. copie de buffer, transfert de ressources)
	_createCommandPool(_resetCommandPool, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	_createCommandPool(_transientCommandPool, vk::CommandPoolCreateFlagBits::eTransient);

	// On crée les buffers pour stocker les données géométriques :
	// - vertex buffer : contient les positions, couleurs, normales, etc.
	// - index buffer : décrit l’ordre dans lequel les sommets sont connectés.
	_createVertexBuffer(TRIANGLE);
	_createIndexBuffer(TRIANGLE);

	// On enregistre dans un *command buffer* toutes les commandes Vulkan nécessaires pour dessiner nos objets.
	// Ce buffer sera soumis à une file de commandes à chaque frame.
	_createCommandBuffer();

	// On crée les *semaphores* et *fences* pour la synchronisation :
	// ils permettent de s’assurer que les opérations GPU (rendu, présentation, etc.)
	// s’exécutent dans le bon ordre et ne se chevauchent pas.
	_createSyncObjects();
}
