# UART BSL of MSP430F5529

This repository contains everything necessary to burn a program in a msp4305529 through uart using the BSL (Bootstrap Loader). My intention with BSL was that I could get a command through an UCA0 from msp, and turn on the "firmware upgrade" function. Upon entering this mode, the same UCA0 would receive the new firmware, update and restart the controller. So it would be possible to put my project in the field, and with each bug found, be able to correct it effectively.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for aplication purposes. All code in this project can be found in these two links:

http://www.ti.com/tool/mspbsl (In the session custom-bsl430) <Last accessed 9/27/2017>
http://processors.wiki.ti.com/index.php/Custom_MSP430_BSL <Last accessed 9/27/2017>

All I did was just get several parts of the codes contained in these links and make it work the right way. 

### Prerequisites

Here is all the pertinent information to be able to burn this code in an msp430f5529 that I also used.

```
IDE: Code Composer Studio v6.1.1.00022
Compiler: TI v4.1.9
BSL Scripter: MSPBSL_Scripter  v3.2.1
SO: Windowns 7 Ultimate 64 bits
```

I do not know exactly if a previous or later version of CCS will present problems, but if any appears, download the same version as mine and it will practically be solved. Unlike the IDE, the compiler version and the script are important that are the ones I'm using, as they are the ones that work, I do not guarantee that later or previous versions will work as expected.

I left links to get my CCS and scripter versions. And a texas tutorial on how to download other versions of the TI compiler inside the CCS.

CCS versions: http://processors.wiki.ti.com/index.php/Download_CCS <Last accessed 9/27/2017>
Scripter: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPBSL_Scripter/latest/index_FDS.html  <Last accessed 9/27/2017>
TI compiler tutorial: http://processors.wiki.ti.com/index.php/Compiler_Releases <Last accessed 9/27/2017>

### Using in CSS with MSP-FET

After certifying that everything is as I listed above, we will use the CCS. Doing this is a very basic thing, but I think it needs to be explained right and you need to be aware if everything is correct. First go to File-> Import...

![001]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/001.png")

In the Code Composer Studio tab select CCS projects

![002]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/002.png")

Click browse, point to the src folder inside the cloned repository on your machine. The project will appear under the name of MSP430F5529_BSL. Click finish.

![003]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/003.png")

This project will appear in project explorer. Right-click on this and go to properties.

![004]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/004.png")

This part is important. If the "Compiler Version" field is not TI v4.1.9, back and read the link to download older versions of the compiler. Make sure the rest is the same and now go to Build-> MSP430 Linker.

![005]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/005.png")

Go to "MSP430 Properties" and check if the "Allow Read/Write/Erase access to BSL memory" box is checked. And if "Erase main, information and protected information memory in Erase Options" is checked. If all is correct, click Apply and then OK.

![006]("https://github.com/rafaellcoellho/bsl-msp430/tree/master/doc/img/006.png")

Finally, go to Project-> Build Project and wait. When finished, this indicated line displayed in the photo will be with warning. As far as I've used this code, it does not affect how it works. If someone has problems and/or find out how to resolve this warning correctly, please contact me.

## Testing

In the examples folder you can find all the information you need to test whether these codes really work. It's quite simple indeed.

I used a PL2303 USB to UART to test. Any such chip that converts USB to serial works. (http://www.prolific.com.tw/US/ShowProduct.aspx?p_id=225&pcid=41)

First look at the schematic contained in the sch folder, and after mounting the hardware correctly open the windonws prompt and navigate to the bsl-msp430\examples\usb_to_serial\script folder. Open the script.txt and change the COM number. There simply call the command line:

```
BSL_Scripter.exe script.txt
```
The following must appear:

```
---------------------------------------------------------
BSL Scripter 3.2.1
PC software for BSL programming
2017-Sep-27 11:02:09
---------------------------------------------------------
Input file script is : bsl-msp430/examples/usb_to_serial/script/script.txt
MODE 5xx UART 9600 COM17 PARITY
VERBOSE
        Verbose mode is now on!
MASS_ERASE
        [80] [01] [00] [15] [64] [a3]
        <80> <02> <00> <3b> <00> <60> <c4>
        Mass Erase is successful!
RX_PASSWORD
        [80] [21] [00] [11] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff]
        [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff] [ff]
        [ff] [ff] [ff] [ff] [9e] [e6]
        <80> <02> <00> <3b> <00> <60> <c4>
        BSL Password is correct!
RX_DATA_BLOCK 5529_LED_BLINK.txt
        Read Txt File  : bsl-msp430\examples\usb_to_serial\script\5529_LED_BLINK.txt
        [80] [40] [00] [10] [00] [80] [00] [31] [40] [00] [34] [b0] [13] [0c] [80] [b0]
        [13] [32] [80] [21] [83] [d2] [43] [04] [02] [d2] [43] [02] [02] [b2] [40] [80]
        [5a] [5c] [01] [07] [3c] [91] [53] [00] [00] [b1] [93] [00] [00] [fb] [23] [d2]
        [e3] [02] [02] [81] [43] [00] [00] [f8] [3f] [80] [00] [36] [80] [80] [00] [3a]
        [80] [ff] [3f] [6e] [6c]
        <80> <02> <00> <3b> <00> <60> <c4>
        [80] [24] [00] [10] [e0] [ff] [00] [12] [34] [56] [78] [99] [10] [11] [12] [13]
        [14] [15] [16] [17] [18] [19] [20] [12] [34] [56] [78] [99] [10] [11] [12] [13]
        [14] [15] [16] [17] [18] [00] [80] [a6] [16]
        <80> <02> <00> <3b> <00> <60> <c4>
        Time elapsed of writing 92 bytes : 0.346 seconds
        Speed of writing data :0.2596(kB/s)
DELAY 2000
        Delay 2000 ms
RX_DATA_BLOCK BOR.txt
        Read Txt File  : bsl-msp430\examples\usb_to_serial\script\BOR.txt
        [80] [06] [00] [10] [20] [01] [00] [a5] [02] [5d] [a8]
        <80>
        [ACK_ERROR_MESSAGE]Unknown ACK value!
        Time elapsed of writing 2 bytes : 0.043 seconds
        Speed of writing data :0.04542(kB/s)
```
Now it is necessary to look if the LED previously connected is blinking. After all this, you are able to upload any firmware in txt format. How to generate this kind of file for your firmware in the CCS or IAR Workbench to be able to burn through this method? Read the documentation provided on the texas instruments website or in the folder in that repository called docs.

I hope you get the same result as me: D.

After testing like this, you'll need to test how to call BSL burn mode within an actual application. Inside the proj folder you have a project that you can import into CCS and sending the strings through the same upload uart as "RUN BSL\r" will put in the same state, and then the process of running the script is the same.

This code is not complicated, I recommend you read it well, and understand exactly how it works. You can adapt a function to call BSL in your own application easily.

## Contributing

If you think this is worth improving in any way, either with this README.md or anything, send me an email :D.

## Author

* **Rafael Coelho Silva** - *Initial work* - [rafaellcoellho](https://github.com/rafaellcoellho)

My personal e-mail: rafaellcoellho@gmail.com

## License

As all the code written here was provided by texas instruments all comments describing the license were kept, if you have any questions just look in src files or email texas instruments.

