#include "VulkanEngine.hpp"
#include "utils.hpp"
#include "colors.hpp"
#include <iostream>

vk::raii::ShaderModule VulkanEngine::_createShaderModule(const std::vector<char> & shaderSrc) const
{
	vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
	shaderModuleCreateInfo.codeSize = shaderSrc.size() * sizeof(char);
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderSrc.data());

	vk::raii::ShaderModule shaderModule(_device, shaderModuleCreateInfo);
	return shaderModule;
}

void VulkanEngine::_createGraphicsPipeline()
{
	const std::vector<char> shaderSrc = readFile("srcs/shaders/spir-v/base.spv");

	// On créé le shader module qui va stocker notre code source
	vk::raii::ShaderModule shaderModule = _createShaderModule(shaderSrc);

	// On indique notre vertex shader ainsi que son point d'entrée
	vk::PipelineShaderStageCreateInfo vertexInfo;
	vertexInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexInfo.module = shaderModule;
	vertexInfo.pName = "vertMain";

	// On indique notre fragment shader ainsi que son point d'entrée
	vk::PipelineShaderStageCreateInfo fragInfo;
	fragInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragInfo.module = shaderModule;
	fragInfo.pName = "fragMain";

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexInfo, fragInfo };
	(void) shaderStages;
	
	// On rend notre viewport dynamique pour plus de flexibilité
	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();
	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	const vk::VertexInputBindingDescription bindingDescription = _getBindingDescription();
	const std::array<vk::VertexInputAttributeDescription, 3> attributeDescription = _getAttributeDescription();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescription.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
	// (void) vertexInputInfo; // coucou nous après le triangle // coucou !!! on a un triangle omg
	
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputInfo;
	pipelineInputInfo.topology = vk::PrimitiveTopology::eTriangleList;

	// On créé le viewport (la taille de nos images) avec la width et height de notre swap chain
	vk::Viewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_swapChainExtent.width);
	viewport.height = static_cast<float>(_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// On créé le scissor (la vue sur notre image), on met la même taille que notre viewport pour voir les images en entier
	vk::Rect2D scissor;
	scissor.offset = vk::Offset2D{0, 0};
	scissor.extent = _swapChainExtent;

	// On créé l'étape du rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.depthClampEnable = vk::False; // Est-ce que l'on clamp les pixels non-visibles ou est-ce qu'on les discard
	rasterizer.rasterizerDiscardEnable = vk::False;
	rasterizer.polygonMode = vk::PolygonMode::eFill; // Changer ici en line pour le wireframe
	rasterizer.cullMode = vk::CullModeFlagBits::eBack; // Culling
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = vk::False; // Shadow map
	rasterizer.depthBiasSlopeFactor = 1.0f;
	rasterizer.lineWidth = 1.0f;

	// Depth
	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.depthTestEnable = vk::True;
	depthStencil.depthWriteEnable = vk::True;
	depthStencil.depthCompareOp = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = vk::False;
	depthStencil.stencilTestEnable = vk::False;

	// Antialiasing
	vk::PipelineMultisampleStateCreateInfo multisampling;
	// multisampling.rasterizationSamples = vk::SampleCountFlagBits::e2; // Degré de MSAA (Multisample Antialiasing)
	// multisampling.sampleShadingEnable = vk::True;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1; // Degré de MSAA (Multisample Antialiasing)
	multisampling.sampleShadingEnable = vk::False;

	// Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	// On écrit sur les 4 composantes de couleur
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	// Est-ce qu'on active le blending ou non
	colorBlendAttachment.blendEnable = vk::True;
	// Comment la couleur source (nouvelle) et la couleur de destination (déjà présente) sont pondérées avant d’être additionnées.
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	// Indique que les deux couleurs pondérées sont additionnées
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	// Même chose pour l'alpha
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

	// Si on veut utiliser une deuxième méthode de blending (bitwise combinaison), alors logicOpEnable = true
	// Et on spécifie l'opération dans logicOp.
	// NOTE : Ceci désactive la première option (colorBlendAttachment) comme si blendEnable = false
	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.logicOpEnable = vk::False;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &*_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	_pipelineLayout = vk::raii::PipelineLayout(_device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingInfo;
	pipelineRenderingInfo.colorAttachmentCount = 1;
	pipelineRenderingInfo.pColorAttachmentFormats = &_swapChainImageFormat;
	pipelineRenderingInfo.depthAttachmentFormat = vk::Format::eD24UnormS8Uint;

	// On lie toutes les infos de notre pipeline
	vk::GraphicsPipelineCreateInfo graphicsPipelineInfo;
	graphicsPipelineInfo.pNext = &pipelineRenderingInfo;
	graphicsPipelineInfo.stageCount = 2;
	graphicsPipelineInfo.pStages = shaderStages;
	graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState = &pipelineInputInfo;
	graphicsPipelineInfo.pViewportState = &viewportState;
	graphicsPipelineInfo.pRasterizationState = &rasterizer;
	graphicsPipelineInfo.pMultisampleState = &multisampling;
	graphicsPipelineInfo.pDepthStencilState = &depthStencil;
	graphicsPipelineInfo.pColorBlendState = &colorBlending;
	graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;
	graphicsPipelineInfo.layout = _pipelineLayout;
	graphicsPipelineInfo.renderPass = nullptr;

	_graphicsPipeline = vk::raii::Pipeline(_device, nullptr, graphicsPipelineInfo);

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Graphics Pipeline" << RESET << std::endl;
}
