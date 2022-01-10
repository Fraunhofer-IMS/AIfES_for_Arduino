import os
import shutil
import re

if not os.path.exists("./CMSIS"):
    raise Exception("CMSIS folder missing. Copy the CMSIS folder from the Arm CMSIS repository into this folder first.")

# Add Core part
shutil.copytree("./CMSIS/Core/Include", "./Core/Include")

# Add NN part
shutil.copytree("./CMSIS/NN/Include", "./NN/Include")
shutil.copytree("./CMSIS/NN/Source", "./NN/Source")

# Add DSP part
shutil.copytree("./CMSIS/DSP/Include", "./DSP/Include")
shutil.copytree("./CMSIS/DSP/Source/BasicMathFunctions", "./DSP/Source/BasicMathFunctions", 
                    ignore=shutil.ignore_patterns('BasicMathFunctions.c', 'BasicMathFunctions.c'))
shutil.copytree("./CMSIS/DSP/Source/FastMathFunctions", "./DSP/Source/FastMathFunctions", 
                    ignore=shutil.ignore_patterns('FastMathFunctions.c', 'FastMathFunctions.c'))
shutil.copytree("./CMSIS/DSP/Source/MatrixFunctions", "./DSP/Source/MatrixFunctions", 
                    ignore=shutil.ignore_patterns('MatrixFunctions.c', 'MatrixFunctions.c'))
shutil.copytree("./CMSIS/DSP/Source/SupportFunctions", "./DSP/Source/SupportFunctions", 
                    ignore=shutil.ignore_patterns('SupportFunctions.c', 'SupportFunctions.c'))
shutil.copytree("./CMSIS/DSP/PrivateInclude", "./DSP/PrivateInclude")
shutil.copytree("./CMSIS/DSP/ComputeLibrary/Include", "./DSP/ComputeLibrary/Include")
shutil.copytree("./CMSIS/DSP/ComputeLibrary/Source", "./DSP/ComputeLibrary/Source")


# Update include paths
files_core = [os.path.join(dirpath, filename) for dirpath, dirnames, filenames in os.walk("Core") for filename in filenames]
files_dsp = [os.path.join(dirpath, filename) for dirpath, dirnames, filenames in os.walk("DSP") for filename in filenames]
files_nn = [os.path.join(dirpath, filename) for dirpath, dirnames, filenames in os.walk("NN") for filename in filenames]

files = files_core + files_dsp + files_nn
basenames = [os.path.basename(file) for file in files]

file_dict = {basename:file.replace('\\', '/') for (basename, file) in zip(basenames, files)}

for file in files:
    #new_text = ""
    with open(file, 'r') as f:
        text = f.read()
        includes = re.findall('#include ".*"', text)
        
        for include in includes:
            include_basename = os.path.split(include.split('"')[1])[-1]
            if include_basename in file_dict:
                print("Replace ", include_basename, "with CMSIS/" + file_dict[include_basename])
                text = re.sub(include, '#include "CMSIS/{}"'.format(file_dict[include_basename]), text)
            else:
                print(file, include_basename, " not in CMSIS folder")
                pass
    
    text = "#include <aifes_config.h>\n#if defined AIFES_WITH_CMSIS && __arm__\n\n" + text + "\n\n#endif // AIFES_WITH_CMSIS\n"
    with open(file, 'w') as f:
        f.write(text)
        
shutil.rmtree("./CMSIS")