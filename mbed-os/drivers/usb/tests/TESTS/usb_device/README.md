# Testing the Mbed OS USB device

## Setup
Before running tests, please make sure to install all the
required Python modules.

```
pip install -r mbed-os/tools/requirements-ci-build.txt
```

Additional, platform-specific setup is described below.
See also [Known issues](#known-issues).

### Windows
1. Install libusb0.dll somewhere on your PATH.  The bitness of the DLL should match the bitness of your python interpreter.  This DLL can be downloaded from the project [here](https://sourceforge.net/projects/libusb-win32/files/libusb-win32-releases/).  Note that libusb1 does NOT work, it has to be libusb0!  See the comments in host_tests/pyusb_basic.py for details.
1.  Install a **generic USB driver** for two test devices.
    1. Download `Zadig` application from [the Zadig website][LN-zadig].
    1. Unplug the Mbed device.
    1. Open `Zadig`.
    1. Select *Device -> Load Preset Device*.
    1. Open [TESTS/usb_device/basic/zadig_conf/mbed_os-usb_test_device1.cfg][LN-zadig_conf1]
    1. Choose `libusb-win32 (v1.2.6.0)` driver.
    1. Select `Install Driver` and click it.
    1. Select *Device -> Load Preset Device*.
    1. Open [TESTS/usb_device/basic/zadig_conf/mbed_os-usb_test_device2.cfg][LN-zadig_conf2]
    1. Choose `libusb-win32 (v1.2.6.0)` driver.
    1. Select `Install Driver` and click it.
    1. Close `Zadig`.
    1. Plug both USB interfaces (*DAPLink* and *USB device*).

### Linux
1. Install the `hidapi` and `usb` (aka pyusb) Python modules.  It's recommended to install these via a package manager because they have some binary dependencies:
```
sudo apt-get install python3-hidapi python3-usb
```
2. Add your user to the `plugdev` group with `sudo usermod -a -G plugdev <your username>`
3. Update the `udev` rules for the USB VIDs/PIDs used in the test as follows:

    ```bash
    sudo tee /etc/udev/rules.d/99-ttyacms.rules >/dev/null <<EOF
    # Mbed OS USB Device test suite
    ATTRS{idVendor}=="0d28", ATTRS{idProduct}=="0007", MODE="660", GROUP="plugdev", TAG+="uaccess"
    ATTRS{idVendor}=="0d28", ATTRS{idProduct}=="0205", MODE="660", GROUP="plugdev", TAG+="uaccess"
    ATTRS{idVendor}=="0d28", ATTRS{idProduct}=="0206", MODE="660", GROUP="plugdev", TAG+="uaccess"
    ATTRS{idVendor}=="1f00" ATTRS{idProduct}=="2013", ENV{ID_MM_DEVICE_IGNORE}="1", MODE="660", GROUP="plugdev"
    ATTRS{idVendor}=="1f00" ATTRS{idProduct}=="2012", ENV{ID_MM_DEVICE_IGNORE}="1", MODE="660", GROUP="plugdev"
    EOF
    sudo udevadm control --reload-rules
    sudo udevadm trigger
    ```

    Among other things, this will [prevent][LN-udev_rules] the `ModemManager` daemon from automatically opening the
    port and sending the `AT commands`, which it does for every new
    `/dev/ttyACM` device registered in system.
4. Install the `udisks2` package, which the test script uses to mount USB disks.  Additionally, you may need to disable any automounting of disks provided by your file manager / distro.
5. Last but not least, at least on Ubuntu, you may need to change permission settings such that udisks2 actually allows ordinary users to mount disks.  You are supposed to do this with a [polkit rule](https://askubuntu.com/a/1457819/1612342), but I had absolutely no luck getting this to work.  So instead I had to edit `/usr/share/polkit-1/actions/org.freedesktop.UDisks2.policy1` and change the first
```
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>yes</allow_active>
    </defaults>
```
block to
```
    <defaults>
      <allow_any>yes</allow_any>
      <allow_inactive>yes</allow_inactive>
      <allow_active>yes</allow_active>
    </defaults>
```

### Mac
No setup method has been verified for this platform.

## Running tests
1.  Plug both USB interfaces (*DAPLink* and *USB device*) to your host machine.
1. An additional macro `USB_DEVICE_TESTS` is needed to be defined when running tests:
    ```
    mbed test -t <toolchain> -m <target> -DUSB_DEVICE_TESTS -n *-tests-usb_device-*
    ```

## Known issues

### Insufficient user permissions on a Linux machine
Some of the tests require privileged access to the USB device. Running these
as an unprivileged user will manifest with either of the following errors:
*   ```
    [HTST][INF] Device not found
    ```
*   ```
    [HTST][INF] TEST ERROR: Failed with "The device has no langid". Tried 20 times.
    ```

#### Solution
Execute tests with elevated permissions using `sudo`:
```bash
mbed test -t <toolchain> -m <target> -DUSB_DEVICE_TESTS -n tests-usb_device-* --compile
sudo mbed test -t <toolchain> -m <target> -n *-tests-usb_device-* --run -v
```
Note only the `mbed test --run` command requires `sudo`. You can still
`mbed test --compile` as a normal user.

#### Alternative solution
Add an `udev` rule to set the ownership of the device node
[as shown here][LN-libusb_permissions].

### Data toggle reset test fails on a Linux machine
The `tests-usb_device-basic` / `"endpoint test data toggle reset"` test fails
with the following error:
```
[HTST][INF] TEST FAILED: Data toggle not reset when calling
ClearFeature(ENDPOINT_HALT) on an endpoint that has not been halted.
```

Implementations of the *xHCI* driver prior to version 4.17 of the Linux kernel did
not have the functionality necessary to test `"endpoint test data toggle reset"`.
Even though the data toggle is correctly reset on the device side, the host
side will not be synchronized and the test will falsely fail.

#### Solution
Make sure that **at least one** of the following prerequisites is met:
* using the Linux kernel ***4.17* or newer**,
* using the ***eHCI*** USB driver instead of *xHCI*.

Changing the USB driver may be as simple as updating one of the BIOS settings
on your machine. If you'd rather avoid rebooting, you can try
[using setpci command][LN-xhci_setpci].

#### Further reading
1. [the Linux kernel patch adding missing xHCI behavior][LN-linux_xhci_patch],
1. [LKML discussion explaining the details of this issue][LN-xhci_lkml_discussion].

### Mass storage tests are skipped on some targets
The `tests-usb_device-msd` test outputs the following message:
```
[CONN][RXD] SKIP: Not enough heap memory for HeapBlockDevice creation
```

#### Solution
A device with at least *70 kB* of RAM is required to run this test.
The FAT32 filesystem cannot be mounted on a device smaller than 64 kB.

The test can be easily extended to use any block device available in Mbed.

### Windows 8/10: Mass storage tests are failing on test file read
By default Windows 8 and 10 access and write to removable drives shortly after they are connected. It's caused by drive indexing mechanisms. Because disk used in test has only 64kB its content can be easily corrupted by writing large files, what actually was encountered during test runs.

To prevent Windows from writing to removable drives on connect drive indexing can be turned off with the following procedure:
- Start the program "gpedit.msc"
- Navigate to "Computer Configuration \ Administrative Templates \ Windows Components \ Search"
- Enable the policy "Do not allow locations on removable drives to be added to  libraries."

### Isochronous endpoints are skipped in loopback tests
#### Unresolved

### Serial tests fail intermittently on a Linux machine
#### Unresolved
You may want to connect the device directly to the host machine with no hubs on the way.

<!-- LINKS -->
[LN-zadig]: https://zadig.akeo.ie/
[LN-zadig_conf1]: basic/zadig_conf/mbed_os-usb_test_device1.cfg
[LN-zadig_conf2]: basic/zadig_conf/mbed_os-usb_test_device2.cfg
[LN-hidapi_readme]: https://github.com/trezor/cython-hidapi/blob/master/README.rst#install
[LN-hid_requirements]: hid/requirements.txt
[LN-udev_rules]: https://linux-tips.com/t/prevent-modem-manager-to-capture-usb-serial-devices/284
[LN-libusb_permissions]: https://stackoverflow.com/questions/3738173/why-does-pyusb-libusb-require-root-sudo-permissions-on-linux/8582398#8582398
[LN-linux_xhci_patch]: https://github.com/torvalds/linux/commit/f5249461b504d35aa1a40140983b7ec415807d9e
[LN-xhci_lkml_discussion]: https://lkml.org/lkml/2016/12/15/388
[LN-xhci_setpci]: https://linuxmusicians.com/viewtopic.php?t=16901
