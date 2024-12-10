/* Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 * Copyright 2023-2024 Arm Limited and/or its affiliates.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "model_executor/ModelExecutor.h"

int main() {

	ModelExecutor& executor = ModelExecutor::getInstance(512); // Pass the desired pool size

	// Prepare input data
	while(true){
		std::vector<float> inputs = {1.0f, 2.0f, 3.0f, 4.0f};
    	// Run the model
    	std::vector<float> results = executor.run_model(inputs);
	}


	return 0;
}