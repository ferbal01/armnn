//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#pragma once

#include "ClAdditionBaseWorkload.hpp"

namespace armnn
{

class ClAdditionUint8Workload : public ClAdditionBaseWorkload<DataType::QuantisedAsymm8>
{
public:
    using ClAdditionBaseWorkload<DataType::QuantisedAsymm8>::ClAdditionBaseWorkload;
    void Execute() const override;
};

} //namespace armnn
