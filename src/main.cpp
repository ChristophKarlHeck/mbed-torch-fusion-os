/*
Change the values of the following variables in the file: mbed-os/connectivity/FEATUR_BLE/source/cordio/mbed_lib.json
- "desired-att-mtu": 250
- "rx-acl-buffer-size": 255
*/

#include "AD7124.h"
#include "Conversion.h"
#include "ReadingQueue.h"
#include "SendingQueue.h"
// #include "BLEProcess.h"
// #include "PinNames.h"
// #include "mbed_trace.h"
// #include "Service.h"

#include "mbed.h"
#include "ModelExecutor.h"

// *** DEFINE GLOBAL CONSTANTS ***
// ADC
#define DATABITS 8388608
#define VREF 2.5
#define GAIN 4.0
#define SPI_FREQUENCY 10000000 // 1MHz
#define DOWNSAMPLING_RATE 10 // ms

// Thread for reading data from ADC
Thread reading_data_thread;

// Function called in thread to read data
void get_input_model_values_from_adc(unsigned int* model_input_size){

	AD7124 adc(SPI_FREQUENCY);
	adc.init(true, true); // activate both channels
	adc.read_voltage_from_both_channels(DOWNSAMPLING_RATE,*model_input_size);
}



int main()
{
	// Instantiate and initialize the model executor
	ModelExecutor executor;
	executor.initRuntime();
    Result<torch::executor::Program> program = executor.loadModelBuffer();
    const char* method_name = executor.getMethodName(program);
    Result<torch::executor::MethodMeta> method_meta = executor.getMethodMeta(program, method_name);
    torch::executor::MemoryAllocator method_allocator = executor.getMemoryAllocator();
    std::vector<torch::executor::Span<uint8_t>> planned_spans = executor.setUpPlannedBuffer(program, method_meta);
    Result<torch::executor::Method> method = executor.loadMethod(program, method_allocator, planned_spans, method_name);
	unsigned int model_input_size = executor.getNumberOfInputValues(method);
    executor.prepareInputs(method, method_name);

    // Access the shared ReadingQueue instance
    ReadingQueue& reading_queue = ReadingQueue::getInstance();
    
	//Start reading data from ADC Thread
	reading_data_thread.start(callback(get_input_model_values_from_adc, &model_input_size));


	while (true) {
		osEvent evt = reading_queue.mail_box.get();
		if (evt.status == osEventMail) {
		    // Retrieve the message from the mail box
		    ReadingQueue::mail_t *mail = (ReadingQueue::mail_t *)evt.value.p;
			std::vector<float> inputs = get_analog_inputs(mail->inputs, DATABITS, VREF, GAIN);
	        // executor.setModelInput(method, inputs);
            // executor.executeModel(method, method_name);
			reading_queue.mail_box.free(mail); // make mail box empty
		    // Free the allocated mail to avoid memory leaks
		    
		}

		// Needed to avoid immediate resource exhaustion
		thread_sleep_for(DOWNSAMPLING_RATE); // ms
	}



	// printf("\nhi\n");
	// std::vector<float> outputs = {1.0f, 2.0f, 3.0f, 4.0f};
	// SerialCommunication serial_communication(BAUD_RATE);
	// printf("Calling send_model_output\n");
	// int i = serial_communication.send_model_output(outputs);

	// AD7124 adc(DATABITS, VREF, GAIN, SPI_FREQUENCY);
	// adc.init(true, true);
	// adc.read_data_continous();

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







	//ET_LOG(Info, "################### Starting Executorch application... ####################");




	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}