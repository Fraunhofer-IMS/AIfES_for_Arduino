# Prepare Arm CMSIS library for AIfES

AIfES provides the possibility to use the Arm CMSIS (DSP/NN) library for a faster runtime (e.g. faster inference).
To use the "_cmsis" functions in AIfES, the CMSIS library must be copied to this folder in the Arduino library
and the AIfES_WITH_CMSIS macro in the ```aifes_config.h``` must be enabled. Furthermore, the ```aifes_cmsis.h```
must be included in ypur Arduino sketch.


1. Download or clone the Arm CMSIS repository (https://github.com/ARM-software/CMSIS_5).
2. Copy the ```CMSIS``` folder from the repository into the AIfES_for_Arduino library in the ```AIfES_for_Arduino/src/CMSIS``` folder.
    * The AIfES_for_Arduino library can be found in your sketchbook folder (See https://www.arduino.cc/en/guide/libraries -> Manual installation).
    * The AIfES_for_Arduino folder should now have a  ```AIfES_for_Arduino/src/CMSIS/CMSIS``` directory.
3. Run the python script ```AIfES_for_Arduino/src/CMSIS/prepare_cmsis.py```. It will copy the needed files from the CMSIS folder and update the include paths.
4. In the ```AIfES_for_Arduino/src/aifes_config.h```, uncomment the line with the ```#define AIFES_WITH_CMSIS```
5. Add ```#include <aifes_cmsis.h>``` to the sketch in which you want to use a "_cmsis" function.

The AIfES_for_Arduino library folder should afterwards look like this:

```
├── AIfES_for_Arduino
│   ├── examples
│   ├── src
│   │   ├── CMSIS
│   │   │   ├── Core
│   │   │   ├── DSP
│   │   │   ├── NN
│   │   ├── aifes_config.h
```

Warning: Adding the CMSIS library to AIfES may increase the compilation time of your Arduino sketch.