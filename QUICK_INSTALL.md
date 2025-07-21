# Quick Install Guide.

## Requirements
* Rocky 9 or Ubuntu 22.05
* For Containers: 
  * 3 Interfaces
  * Mount the source folder into the container.
* For Bare Bone Metal or VM:
  * 3 network interfaces or the capability to install alias interfaces. 

## Compile and install BRIO

1) **cd project-directory**
   Change to the directory where the BRIO source code is located
2) **./install_dependencies.sh --confirm**
   This scrupt installs all required system libraries and dependencies
3) **./buildBRIO**
   Compile and install the BRIO. The installation will be placed in:
   ```project-directory/local```

## Prepare and run experiments
1) **generate-router-config.sh**
   Scans the ```local/opt/demo-aspa...``` folder for router configuration markup
   files and generates router configuration files (currently for NIST QuaggaSRX).
   The generated configurations are stored in:
   ```local/etc```
2) **Start the BGP router** using the generated configuration (modify it as needed
   for your specific router).
3) **cd local/opt/brio-examples/demo-aspa-...stream**
   Switch to either the **upstream** or **downstream** experiment folder.
4) **start_expXYZ.sh** 
   Starts the experiment XYZ.  
   Use ```--SKIP-ROUTER``` to skip router startup, or update the ```router.sh```
   script located in: ```local/opt/brio-exampes/bin```  
   The file is well documented!

## Additional Notes
* Run ```start_expXYZ.sh -?``` for detailed help.
* Each experiment script may have slighly different options depending on its 
  configuration.

[back](README.md)