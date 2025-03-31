/* Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 * Copyright 2023-2024 Arm Limited and/or its affiliates.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "utils/mbed_stats_wrapper.h"
#include "model_executor/ModelExecutor.h"

int main() {

	mbed_lib::print_memory_info("1");
	ModelExecutor& executor = ModelExecutor::getInstance(8192); // Pass the desired pool size

	std::vector<float> test_input_vector = {
		906.1954, 995.5962, 950.4269, 815.3856, 815.6190,
		905.4908, 725.5136, 770.2163, 681.5200, 860.7928,
		815.1478, 905.4908, 860.0837, 770.4496, 905.4908,
		861.4973, 771.1541, 770.9208, 681.9911, 681.0488,
		771.6253, 816.0901, 771.1541, 726.6893, 771.3920,
		771.3920, 637.0552, 53.5965, -440.6903, -845.1141,
		-799.7069, -934.5149, -755.2421, -844.8762, -844.8762,
		-1069.3182, -1068.8472, -1024.1489, -799.9403, -710.5395,
		-82.3826, 366.9727, 591.1769, 680.8155, 725.2803,
		725.9848, 725.7515, 636.1129, 681.7534, 770.9208,
		816.0901, 680.8155, 725.9848, 636.8174, 859.3792,
		816.0901, 816.0901, 726.6893, 770.9208, 726.9272,
		680.5777, 725.9848, 726.4560, 680.8155, 770.9208,
		726.6893, 680.5777, 725.7515, 726.6893, 366.7348,
		-81.9159, -350.3516, -260.7130, -440.6903, -619.4919,
		-665.3702, -620.2008, -665.3702, -440.6903, -126.3808,
		-260.9509, -126.6141, 6.7803, 277.8052, 97.5946,
		-36.5088, -709.3638, -1337.2874, -1966.3821, -2056.4873,
		-2101.1899, -2010.8468, -2190.3530, -2191.0620, -2145.8882,
		-2146.3591, -2191.0620, -2190.3530, -2190.3530, -2100.4854
	};


	// Prepare input data
	while(true){

		std::vector<float> inputs = {1.0f, 2.0f, 3.0f, 4.0f};

    	// Run the model
    	std::vector<float> results = executor.run_model(test_input_vector);

		break;
	}


	return 0;
}