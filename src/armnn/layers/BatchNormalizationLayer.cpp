//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "BatchNormalizationLayer.hpp"

#include "LayerCloneBase.hpp"

#include <armnn/TypesUtils.hpp>
#include <backends/CpuTensorHandle.hpp>
#include <backends/WorkloadFactory.hpp>

namespace armnn
{

BatchNormalizationLayer::BatchNormalizationLayer(const armnn::BatchNormalizationDescriptor& param, const char* name)
    : LayerWithParameters(1, 1, LayerType::BatchNormalization, param, name)
{
}

std::unique_ptr<IWorkload> BatchNormalizationLayer::CreateWorkload(const Graph& graph,
                                                                   const IWorkloadFactory& factory) const
{
    // on this level constant data should not be released..
    BOOST_ASSERT_MSG(m_Mean != nullptr, "BatchNormalizationLayer: Mean data should not be null.");
    BOOST_ASSERT_MSG(m_Variance != nullptr, "BatchNormalizationLayer: Variance data should not be null.");
    BOOST_ASSERT_MSG(m_Beta != nullptr, "BatchNormalizationLayer: Beta data should not be null.");
    BOOST_ASSERT_MSG(m_Gamma != nullptr, "BatchNormalizationLayer: Gamma data should not be null.");

    BatchNormalizationQueueDescriptor descriptor;

    descriptor.m_Mean = m_Mean.get();
    descriptor.m_Variance = m_Variance.get();
    descriptor.m_Beta = m_Beta.get();
    descriptor.m_Gamma = m_Gamma.get();

    return factory.CreateBatchNormalization(descriptor, PrepInfoAndDesc(descriptor, graph));
}

BatchNormalizationLayer* BatchNormalizationLayer::Clone(Graph& graph) const
{
    auto layer = CloneBase<BatchNormalizationLayer>(graph, m_Param, GetName());

    layer->m_Mean = m_Mean ? std::make_unique<ScopedCpuTensorHandle>(*m_Mean) : nullptr;
    layer->m_Variance = m_Variance ? std::make_unique<ScopedCpuTensorHandle>(*m_Variance) : nullptr;
    layer->m_Beta = m_Beta ? std::make_unique<ScopedCpuTensorHandle>(*m_Beta) : nullptr;
    layer->m_Gamma = m_Gamma ? std::make_unique<ScopedCpuTensorHandle>(*m_Gamma) : nullptr;

    return std::move(layer);
}

void BatchNormalizationLayer::ValidateTensorShapesFromInputs()
{
    VerifyLayerConnections(1, CHECK_LOCATION());

    auto inferredShapes = InferOutputShapes({ GetInputSlot(0).GetConnection()->GetTensorInfo().GetShape() });

    BOOST_ASSERT(inferredShapes.size() == 1);

    ConditionalThrowIfNotEqual<LayerValidationException>(
        "BatchNormalizationLayer: TensorShape set on OutputSlot[0] does not match the inferred shape.",
        GetOutputSlot(0).GetTensorInfo().GetShape(),
        inferredShapes[0]);

}

Layer::ConstantTensors BatchNormalizationLayer::GetConstantTensorsByRef()
{
    return {m_Mean, m_Variance, m_Beta, m_Gamma};
}

} // namespace armnn
