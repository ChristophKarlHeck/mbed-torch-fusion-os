#ifndef SRNS_SERVER_BLE_
#define SRNS_SERVER_BLE_

#include "Characteristic.h"
#include "events/EventQueue.h"
#include "AD7124.h"
#include <cstdint>
#include <csignal>
#include <signal.h>
#include <functional>


using mbed::callback;
//inline Thread read_adc_thread;

namespace{
    std::function<void(int)> mailHandler_internal;
    void mailHandler(int signal)
    {
        mailHandler_internal(signal);
    }
}

class WatchPlant_service: public ble::GattServer::EventHandler{
    typedef WatchPlant_service Self;

public: 
    Thread read_adc_thread;


    WatchPlant_service(AD7124 &adc):
        _notification("5a3b0203-f6dd-4c45-b31f-e89c05ae3390",0),
        _SRNS(
            /* uuid */ "5a3b0003-f6dd-4c45-b31f-e89c05ae3390",
            /* characteristics */ _SRNS_characteristics,
            /* numCharacteristics */ sizeof(_SRNS_characteristics) /
                                     sizeof(_SRNS_characteristics[0])        
        ),
        _server(NULL),
        _event_queue(NULL),
        _adc(adc)        
    {
        _SRNS_characteristics[0] = &_notification;
    }
    void readMail()
    {
        _adc.readADC_ID = _event_queue->call(callback(this, &Self::multiple_value_mail));
    }
    void start(BLE &ble_interface, events::EventQueue &event_queue)
    {
            
        //declare the signal handler function here using lambda
        mailHandler_internal = [this](int signal){
            readMail();
            //signal handler must be re set every time, otherwise it will only run once
            std::signal(SIGUSR1, mailHandler);
            return;
        };

        //tell signal which signal to look for and which handler to use
        std::signal(SIGUSR1, mailHandler);


         if (_event_queue) {
            return;
        }
        _server = &ble_interface.gattServer();
        _event_queue = &event_queue;
        _server->setEventHandler(this);
        // register the service
        INFO("Adding WatchPlant Service\r\n");
        ble_error_t err = _server->addService(_SRNS);

        if (err) {
            printf("Error %u during demo service registration.\r\n", err);
            return;
        }

        // print the handles
        INFO("Service handle: %u\r\n", _SRNS.getHandle());
        INFO("\t notification characteristic value handle %u\r\n", _notification.getValueHandle());
    }

    /**
     * Used to receive a multiple values via mailing from the ADC thread
     * change notification length at the end of the script " NotifyCharacteristic<uint8_t,{here}> _notification;"
     */
    void multiple_value_mail(void){
        //printf("open");
        uint8_t data_array[6] = {0};
        uint8_t *ptr;

        AD7124::mail_t *mail = (AD7124::mail_t *) _adc.mail_box.try_get();
        if (mail != nullptr &&  mail->voltage != 0.00 && mail->ready){
            ptr = mail->raw_measurement_array;
            long measurement1 =  (((long)ptr[0] << 16)|((long)ptr[1] << 8)|((long)ptr[2] << 0));
            long measurement2 =  (((long)ptr[3] << 16)|((long)ptr[4] << 8)|((long)ptr[5] << 0));
            //printf("1: %ld   ", measurement1);
            //_adc.get_analog_value(measurement1);
            //printf("2: %ld   ", measurement2);
            // _adc.get_analog_value(measurement2);
            _adc.mail_box.free(mail);
            
            ble_error_t err = _notification.set(*_server, ptr);
            if (err) {
                printf("write of the second value returned error %u\r\n", err);
                return;
            }
        }
    }
    /**
     * Used to receive a single value via mailing from the ADC thread
     */
    void single_value_mail(void){        
        long raw_measurement = 0;
        uint8_t data [3] = {0};
        AD7124::mail_t *mail = (AD7124::mail_t *) _adc.mail_box.try_get();
        if (mail != nullptr &&  mail->voltage != 0.00 && mail->ready){
            printf("\nVoltage: %.2f V\n\r#", mail->voltage);
            raw_measurement = mail->raw_measurement;
            _adc.mail_box.free(mail);

            INFO("%ld", raw_measurement);
            data[2] = raw_measurement         & 0xFF;
            data[1] = (raw_measurement >>  8) & 0xFF;
            data[0] = (raw_measurement >> 16) & 0xFF;

            ble_error_t err = _notification.set(*_server, data);
        
            //memset(data, 0, 3* sizeof(data));
            if (err) {
                printf("write of the second value returned error %u\r\n", err);
                return;
            }
        }
    }

private:
    /**
     * Handler called when an indication confirmation has been received.
     *
     * @param handle Handle of the characteristic value that has emitted the
     * indication.
     */
    void onConfirmationReceived(const GattConfirmationReceivedCallbackParams &params) override
    {
        printf("confirmation received on handle %d\r\n", params.attHandle);
    }
    /**
     * Handler called when a notification or an indication has been sent.
     */
    void onDataSent(const GattDataSentCallbackParams &params) override
    {
        // do something when data was send
    }
    /**
     * Handler called after a client has subscribed to notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override
    {
        //_adc.readADC_ID = _event_queue->call_every(10ms, callback(this, &Self::multiple_value_mail));
        INFO("update enabled")
        read_adc_thread.start(callback(&_adc, &AD7124::read_thread_multiple_valuev2));   
    }

    /**
     * Handler called after a client has cancelled his subscription from
     * notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override
    {
        printf("update disabled on handle %d\r\n", params.attHandle);
        _event_queue->cancel(_adc.readADC_ID); //           <---- check change
       //read_adc_thread.terminate(); //                     <---- check change
    }

    /**
     * Helper that construct an event handler from a member function of this
     * instance.
     */
    template<typename Arg>
    FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg))
    {
        return makeFunctionPointer(this, member);
    }



// list of the characteristics of the clock service
GattCharacteristic* _SRNS_characteristics[1];

// service for electrical potential notification
GattService _SRNS;
// ADC class
AD7124 &_adc; 
// BLE server
GattServer* _server;
//event queue
events::EventQueue *_event_queue;

//
NotifyCharacteristic<uint8_t,6> _notification;
};

#endif