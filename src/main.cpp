/*
Change the values of the following variables in the file: mbed-os/connectivity/FEATUR_BLE/source/cordio/mbed_lib.json
- "desired-att-mtu": 250
- "rx-acl-buffer-size": 255
*/

// Standard Library Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

// Third-Party Library Headers
#include "mbed.h"
#include "USBSerial.h" // Uncomment if needed for debugging

// Project-Specific Headers
#include "AD7124.h"
#include "ReadingQueue.h"
#include "SendingQueue.h"
#include "ModelExecutor.h"
#include "SerialCommunication.h"
// #include "SerialMail_generated.h"

// Utility Headers
#include "Conversion.h"
#include "logger.h"

// Virtual USB Port for logging on Raspberry PI
//USBSerial rapi; // Define the instance

// *** DEFINE GLOBAL CONSTANTS ***
#define DOWNSAMPLING_RATE 100 // ms
#define CLASSIFICATION 0

// CONVERSION
#define DATABITS 8388608
#define VREF 2.5
#define GAIN 4.0

// ADC
#define SPI_FREQUENCY 10000000 // 1MHz

// UART
#define BAUD_RATE 19200

// Thread for reading data from ADC
Thread reading_data_thread;

// Thread for sending data to data sink
Thread sending_data_thread;

// Function called in thread "reading_data_thread"
void get_input_model_values_from_adc(unsigned int* model_input_size){

	AD7124 adc(SPI_FREQUENCY);
	adc.init(true, true); // activate both channels
	adc.read_voltage_from_both_channels(DOWNSAMPLING_RATE,*model_input_size);
}

/// Function called in thread "sending_data_thread"
// Also mark in in CMakeLists since when we don't need BLE we don't neet to compile the software for it
void send_output_to_data_sink(void){

	SerialCommunication serial_comm(BAUD_RATE);
	// Start sending data via UART
	serial_comm.send_struct_via_serial_port();  

}


int main()
{	
	// Create a FlatBufferBuilder with an initial size of 1024 bytes
    //flatbuffers::FlatBufferBuilder builder(1024);

	// Just run that program and nothing else to fix weierd issues
	// while(1){
	// 	printf("hi\n");
	// 	thread_sleep_for(1000);
	// }

	// Instantiate and initialize the model executor
	/*
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
	*/
	// std::vector<float> my_vector = {3.0f, 4.23f, 2.3f, 1.2f};
	// executor.setModelInput(method, my_vector);
	// executor.printModelInput(method);
	// executor.executeModel(method, method_name);
	// executor.printModelOutput(method);



    // Access the shared ReadingQueue instance
    ReadingQueue& reading_queue = ReadingQueue::getInstance();

	// Access the shared queue
    SendingQueue& sending_queue = SendingQueue::getInstance();
    
	//Start reading data from ADC Thread
	unsigned int n = 4;
	reading_data_thread.start(callback(get_input_model_values_from_adc, &n));

	//Start sending Thread
	sending_data_thread.start(callback(send_output_to_data_sink));

	while (true) {
		osEvent evt = reading_queue.mail_box.get();
		if (evt.status == osEventMail) {

		    // Retrieve the message from the mail box
		    ReadingQueue::mail_t *reading_mail = (ReadingQueue::mail_t *)evt.value.p;

			// Store reading data temporary
			std::vector<std::array<uint8_t, 3>> inputs_as_bytes = reading_mail->inputs;
			bool channel = reading_mail->channel;

			// Free the allocated mail to avoid memory leaks
			// make mail box empty
			reading_queue.mail_box.free(reading_mail); 

			thread_sleep_for(DOWNSAMPLING_RATE); // needed here too otherwise CPU exhaustion

			// Convert received bytes to floats
			std::vector<float> inputs = get_analog_inputs(inputs_as_bytes, DATABITS, VREF, GAIN);

			// Prepare result vector
			std::vector<float> classification_result;

			if(CLASSIFICATION){
				// Execute Model with received inputs
				executor.setModelInput(method, inputs);
				executor.executeModel(method, method_name);
				classification_result = executor.getModelOutput(method);
			}

			while (!sending_queue.mail_box.empty()) {
                // Wait until sending queue is empty
                thread_sleep_for(1);
				//printf("Wait for the sending queue to become empty.\n");
            }
		    
			if (sending_queue.mail_box.empty()) {
				SendingQueue::mail_t* sending_mail = sending_queue.mail_box.try_alloc();
				sending_mail->inputs = inputs_as_bytes;
				sending_mail->classification = classification_result;
				sending_mail->classification_active = CLASSIFICATION;
				sending_mail->channel = channel;
				sending_queue.mail_box.put(sending_mail); 
			}

		}

		// Needed to avoid immediate resource exhaustion
		thread_sleep_for(DOWNSAMPLING_RATE); // ms
		// rapi.printf("no hard fault");
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







	//ET_LOG(Info, "################### Starting Executorch application... ####################");




	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}