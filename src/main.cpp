/*
Change the values of the following variables in the file: mbed-os/connectivity/FEATUR_BLE/source/cordio/mbed_lib.json
- "desired-att-mtu": 250
- "rx-acl-buffer-size": 255
*/

#include "AD7124.h"
// #include "BLEProcess.h"
// #include "PinNames.h"
// #include "mbed_trace.h"
// #include "Service.h"

#include "mbed.h"
#include "ModelExecutor.h"

// Define global constants
const float DATABITS = 8388608.0; // or constexpr if known at compile time
const float VREF = 2.5;
const float GAIN = 4.0;

//#include <cstdio>

void read_data(AD7124 *adc){

	adc->read_voltage_from_both_channels();

}

Thread thread; 
int main()
{
	/* PREPARE MODEL */
	// ModelExecutor executor;
	// executor.initRuntime();
	// Result<torch::executor::Program> program = executor.loadModelBuffer();
	// const char* method_name = executor.getMethodName(program);
	// Result<torch::executor::MethodMeta> method_meta = executor.getMethodMeta(program, method_name);
	// torch::executor::MemoryAllocator method_allocator = executor.getMemoryAllocator();
	// std::vector<torch::executor::Span<uint8_t>> planned_spans = executor.setUpPlannedBuffer(program, method_meta);
	// Result<torch::executor::Method> method = executor.loadMethod(program, method_allocator, planned_spans, method_name);
	// executor.prepareInputs(method, method_name);


	// Instantiate the AD7124 object with databits, Vref, and Gain
    AD7124 adc(DATABITS, VREF, GAIN);
	adc.init(true, true);
	thread.start(callback(read_data, &adc));
	while (true) {
		osEvent evt = adc.mail_box.get();
		if (evt.status == osEventMail) {
		    // Retrieve the message from the mail box
		    mail_t *mail = (mail_t *)evt.value.p;
		    printf("Voltage CH1: %.3f V, Voltage CH2: %.3f V\n", mail->voltage_channel_0, mail->voltage_channel_1);

		    // Free the allocated mail to avoid memory leaks
		    adc.mail_box.free(mail);
		}
	}

    /* initialize the BLE interface */
    // BLE &ble_interface = BLE::Instance();
    // events::EventQueue event_queue;
    // /* load the custom service */
    // WatchPlant_service  notification_only(adc);
	/* load and start the BLE process */
    // BLEProcess ble_process(event_queue, ble_interface, notification_only, adc);
    // ble_process.on_init(callback(&notification_only, &WatchPlant_service::start));
    // ble_process.start();
    // // Process the event queue.
    // event_queue.dispatch_forever();

	/* EXECUTE MODEL WITH VARIABLE INPUT */
	// std::vector<float> inputs = {2.0f, 2.0f, 3.0f, 4.0f};
	// executor.setModelInput(method, inputs);
	// executor.printModelInput(method);
	// executor.executeModel(method, method_name);
	// executor.printModelOutput(method);
	// std::vector<float> outputs = executor.getModelOutput(method);





	//ET_LOG(Info, "################### Starting Executorch application... ####################");




	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}