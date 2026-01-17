#include "VulkanEngine.hpp"
#include "utils.hpp"
#include "Logger.hpp"
#include <iostream>

vk::raii::ShaderModule VulkanEngine::_createShaderModule(const std::vector<char> & shaderSrc) const
{
	vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
	shaderModuleCreateInfo.codeSize = shaderSrc.size() * sizeof(char);
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderSrc.data());

	vk::raii::ShaderModule shaderModule(_device, shaderModuleCreateInfo);
	return shaderModule;
}

PipelineID VulkanEngine::uploadPipeline(PipelineInfo & pipelineInfo)
{
	int pipelineID;
	std::vector<char> shaderSrc;
	std::shared_ptr<vk::raii::ShaderModule> shaderModule = nullptr;

	pipelineID = _pipelineMap.size();
	const ShaderCache::iterator & shaderit = _shaderCache.find(pipelineInfo.shaderName);
	if (shaderit != _shaderCache.end())
		shaderModule = shaderit->second;
	else
	{
		shaderSrc = readFile(VULKAN_SHADER_PATH + pipelineInfo.shaderName + ".spv");
		// On créé le shader module qui va stocker notre code source
		shaderModule = std::make_shared<vk::raii::ShaderModule>(_createShaderModule(shaderSrc));
		_shaderCache.emplace(pipelineInfo.shaderName, shaderModule);
	}

	// On indique notre vertex shader ainsi que son point d'entrée
	vk::PipelineShaderStageCreateInfo vertexInfo;
	vertexInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexInfo.module = *shaderModule;
	vertexInfo.pName = "vertMain";

	// On indique notre fragment shader ainsi que son point d'entrée
	vk::PipelineShaderStageCreateInfo fragInfo;
	fragInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragInfo.module = *shaderModule;
	fragInfo.pName = "fragMain";

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = { vertexInfo, fragInfo };
	shaderStages.shrink_to_fit();

	// On rend notre viewport dynamique pour plus de flexibilité
	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();
	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	const vk::VertexInputBindingDescription bindingDescription = _getBindingDescription(pipelineInfo);
	VertexAttributeDescriptionVector attributeDescription = _getAttributeDescription(pipelineInfo);
	attributeDescription.shrink_to_fit();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescription.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
	// (void) vertexInputInfo; // coucou nous après le triangle // coucou !!! on a un triangle omg
	
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputInfo;
	pipelineInputInfo.topology = vk::PrimitiveTopology::eTriangleList;

	vk::Extent2D extent;
	extent.width = pipelineInfo.width == -1 ? _swapChainExtent.width : pipelineInfo.width;
	extent.height = pipelineInfo.height == -1 ? _swapChainExtent.height : pipelineInfo.height;
	// On créé le viewport (la taille de nos images) avec la width et height de notre swap chain
	vk::Viewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// On créé le scissor (la vue sur notre image), on met la même taille que notre viewport pour voir les images en entier
	vk::Rect2D scissor;
	scissor.offset = vk::Offset2D{0, 0};
	scissor.extent = extent;

	// On créé l'étape du rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizerFill;
	rasterizerFill.depthClampEnable = vk::False; // Est-ce que l'on clamp les pixels non-visibles ou est-ce qu'on les discard
	rasterizerFill.rasterizerDiscardEnable = vk::False;
	switch (static_cast<int>(pipelineInfo.polygonMode))
	{
		case FILL: rasterizerFill.polygonMode = vk::PolygonMode::eFill; break;
		case LINE: rasterizerFill.polygonMode = vk::PolygonMode::eLine; break;
	}
	switch (static_cast<int>(pipelineInfo.cullMode))
	{
		case OFF: rasterizerFill.cullMode = vk::CullModeFlagBits::eNone; break;
		case BACK: rasterizerFill.cullMode = vk::CullModeFlagBits::eBack; break;
	}
	rasterizerFill.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizerFill.depthBiasEnable = vk::False; // Shadow map
	rasterizerFill.depthBiasSlopeFactor = 1.0f;
	rasterizerFill.lineWidth = 1.0f;

	// Depth
	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.depthTestEnable = pipelineInfo.depthTest;
	depthStencil.depthWriteEnable = pipelineInfo.depthTest;
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
	colorBlendAttachment.blendEnable = pipelineInfo.blend;
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

	_pipelineMap.try_emplace(pipelineID);
	PipelineObjects & pipelineObjects = _pipelineMap[pipelineID];
	pipelineObjects.layout = vk::raii::PipelineLayout(_device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingInfo;
	pipelineRenderingInfo.colorAttachmentCount = 1;
	pipelineRenderingInfo.pColorAttachmentFormats = &_swapChainImageFormat;
	pipelineRenderingInfo.depthAttachmentFormat = _depthFormat;

	// On lie toutes les infos de notre pipeline
	vk::GraphicsPipelineCreateInfo graphicsPipelineFillInfo;
	graphicsPipelineFillInfo.pNext = &pipelineRenderingInfo;
	graphicsPipelineFillInfo.stageCount = shaderStages.size();
	graphicsPipelineFillInfo.pStages = shaderStages.data();
	graphicsPipelineFillInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineFillInfo.pInputAssemblyState = &pipelineInputInfo;
	graphicsPipelineFillInfo.pViewportState = &viewportState;
	graphicsPipelineFillInfo.pRasterizationState = &rasterizerFill;
	graphicsPipelineFillInfo.pMultisampleState = &multisampling;
	graphicsPipelineFillInfo.pDepthStencilState = &depthStencil;
	graphicsPipelineFillInfo.pColorBlendState = &colorBlending;
	graphicsPipelineFillInfo.pDynamicState = &dynamicStateInfo;
	graphicsPipelineFillInfo.layout = pipelineObjects.layout;
	graphicsPipelineFillInfo.renderPass = nullptr;

	pipelineObjects.pipeline = vk::raii::Pipeline(_device, nullptr, graphicsPipelineFillInfo);
	pipelineObjects.pipelineInfo = pipelineInfo;

	Logger::log(ENGINE_VULKAN, INFO, "Created Pipeline ID: " + toString(pipelineID));
	return pipelineID;
}
