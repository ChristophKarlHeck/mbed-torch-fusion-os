#ifndef GATT_SERVER_EXAMPLE_BLE_PROCESS_H_
#define GATT_SERVER_EXAMPLE_BLE_PROCESS_H_

#include "events/EventQueue.h"
#include "ble/BLE.h"
#include "Service.h"
#include "AD7124.h"
/**
 * Handle initialization adn shutdown of the BLE Instance.
 *
 * Setup advertising payload and manage advertising state.
 * Delegate to GattClientProcess once the connection is established.
 */


class BLEProcess : private mbed::NonCopyable<BLEProcess>, public ble::Gap::EventHandler {
public:
    /**
     * Construct a BLEProcess from an event queue and a ble interface.
     *
     * Call start() to initiate ble processing.
     */
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface, WatchPlant_service &SRNS,AD7124 &adc) :
        _event_queue(event_queue),
        _ble_interface(ble_interface),
        _post_init_cb(),
        _adv_data_builder(_adv_buffer),
        _SRNS_Service(SRNS),
        _adc(adc)  {
    }

    ~BLEProcess()
    {
        stop();
    }

   /**
     * Subscription to the ble interface initialization event.
     *
     * @param[in] cb The callback object that will be called when the ble
     * interface is initialized.
     */
    void on_init(mbed::Callback<void(BLE&, events::EventQueue&)> cb)
    {
        _post_init_cb = cb;
    }

    /**
     * Initialize the ble interface, configure it and start advertising.
     */
    bool start()
    {
        printf("Ble process started.\r\n");
        
        if (_ble_interface.hasInitialized()) {
            printf("Error: the ble instance has already been initialized.\r\n");
            return false;
        }

        _ble_interface.onEventsToProcess(
            makeFunctionPointer(this, &BLEProcess::schedule_ble_events)
        );

        // assign event handler
        Gap &gap = _ble_interface.gap(); 
        gap.setEventHandler(this);

        ble_error_t error = _ble_interface.init(
            this, &BLEProcess::when_init_complete
        );

        if (error) {
            printf("Error: %u returned by BLE::init.\r\n", error);
            return false;
        }
        return true;
    }

    /**
     * Close existing connections and stop the process.
     */
    void stop()
    {
        if (_ble_interface.hasInitialized()) {
            _ble_interface.shutdown();
            printf("Ble process stopped.");
        }
    }

private:
    ble::AdvertisingDataBuilder _adv_data_builder;
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    /**
     * Schedule processing of events from the BLE middleware in the event queue.
     */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }

    /**
     * Sets up adverting payload and start advertising.
     *
     * This function is invoked when the ble interface is initialized.
     */
    void when_init_complete(BLE::InitializationCompleteCallbackContext *event)
    {
        if (event->error) {
            printf("Error %u during the initialization\r\n", event->error);
            return;
        }
        printf("Ble instance initialized\r\n");

        Gap &gap = _ble_interface.gap();

        if (!set_advertising_parameters()) {
            return;
        }

        if (!set_advertising_data()) {
            return;
        }

        if (!start_advertising()) {
            return;
        }

        if (_post_init_cb) {
            _post_init_cb(_ble_interface, _event_queue);
        }
    }

    void onConnectionComplete(const ble::ConnectionCompleteEvent &event) override
    {
        printf("Connected.\r\n");
    }
    
    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) override
    {
        NVIC_SystemReset(); // ------------------------ this causes a complete reset. Problem: after sudden death (Likce ctrl-C) of RPi, no data send
        _event_queue.cancel(_adc.readADC_ID);
        printf("Disconnected.\r\n");
        start_advertising();
    }
  
    bool start_advertising(void)
    {
        Gap &gap = _ble_interface.gap();

        /* Start advertising the set */
        ble_error_t error = gap.startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            printf("Error %u during gap.startAdvertising.\r\n", error);
            return false;
        } else {
            printf("Advertising started.\r\n");
            return true;
        }
    }

    bool set_advertising_parameters()
    {
        Gap &gap = _ble_interface.gap();

        ble_error_t error = gap.setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            ble::AdvertisingParameters()
            .setOwnAddressType(ble::own_address_type_t::PUBLIC)
        );

        if (error) {
            printf("Gap::setAdvertisingParameters() failed with error %d", error);
            return false;
        }

        return true;
    }

    bool set_advertising_data()
    {
        Gap &gap = _ble_interface.gap();

        /* create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            /* you cannot connect to this device, you can only read its advertising data,
             * scannable means that the device has extra advertising data that the peer can receive if it
             * "scans" it which means it is using active scanning (it sends a scan request) */
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        const uint8_t _vendor_specific_data[4] = { 0xAD, 0xDE, 0xBE, 0xEF };
        _adv_data_builder.setManufacturerSpecificData(_vendor_specific_data);

        gap.setAdvertisingScanResponse(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );
        /* now we set the advertising payload that gets sent during advertising without any scan requests */

        _adv_data_builder.clear();
        _adv_data_builder.setFlags();
        _adv_data_builder.setName("P1");
        ble_error_t error = gap.setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters.setOwnAddressType(ble::own_address_type_t::PUBLIC)
        );

        error = gap.setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );
        return true;
    }

    events::EventQueue &_event_queue;
    BLE &_ble_interface;
    mbed::Callback<void(BLE&, events::EventQueue&)> _post_init_cb;
    WatchPlant_service &_SRNS_Service;
    AD7124 &_adc;
};

#endif /* GATT_SERVER_EXAMPLE_BLE_PROCESS_H_ */
