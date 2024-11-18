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
const int DOWNSAMPLING_RATE = 1;

struct DataPassedToReadingThread {
	AD7124* adc;					// Reference to the ADC object
	int number_of_input_values;		// Size of inputs passed to main thread
	const int* downsampling;				// Downsampling rate

	// Constructor to initialize the struct with a reference and two integer values
    DataPassedToReadingThread(
		AD7124* adc_ref,
		int num_values_ref,
		const int* downsampling) : adc(adc_ref), number_of_input_values(num_values_ref), downsampling(downsampling) {}
};

// Function called in thread to read data
void read_data(DataPassedToReadingThread* data){

	printf("hi\n");
	data->adc->read_voltage_from_both_channels(data->number_of_input_values);
	printf("hi2\n");

}

// Thread for reading data from ADC
Thread reading_data_thread;

//Serial pc(UART1_TX, UART1_RX,); // tx, rx
static BufferedSerial serial_port(USBTX, USBRX);

int main()
{
	/*
	// Instantiate and initialize the model executor
	ModelExecutor executor;
	executor.initRuntime();
    Result<torch::executor::Program> program = executor.loadModelBuffer();
    const char* method_name = executor.getMethodName(program);
    Result<torch::executor::MethodMeta> method_meta = executor.getMethodMeta(program, method_name);
    torch::executor::MemoryAllocator method_allocator = executor.getMemoryAllocator();
    std::vector<torch::executor::Span<uint8_t>> planned_spans = executor.setUpPlannedBuffer(program, method_meta);
    Result<torch::executor::Method> method = executor.loadMethod(program, method_allocator, planned_spans, method_name);
	int number_of_input_values = executor.getNumberOfInputValues(method);
    executor.prepareInputs(method, method_name);

	//Instantiate the AD7124 object with databits, Vref, and Gain
	int n = 4;
    AD7124 adc(DATABITS, VREF, GAIN);
	adc.init(true, true); // activate both channels
	DataPassedToReadingThread data_for_reading_thread(&adc, number_of_input_values, &DOWNSAMPLING_RATE);


	reading_data_thread.start(callback(read_data, &data_for_reading_thread));


	while (true) {
		osEvent evt = adc.mail_box.get();
		if (evt.status == osEventMail) {
		    // Retrieve the message from the mail box
		    mail_t *mail = (mail_t *)evt.value.p;
	        executor.setModelInput(method, mail->inputs);
            executor.executeModel(method, method_name);
			adc.mail_box.free(mail); // make mail box empty
		    // Free the allocated mail to avoid memory leaks
		    
		}

		// Needed to avoid immediate resource exhaustion
		thread_sleep_for(10); // ms
	}

	*/

	AD7124 adc(DATABITS, VREF, GAIN);
	adc.init(true, false);
	adc.read_data_continous();

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