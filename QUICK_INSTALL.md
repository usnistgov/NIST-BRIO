# Quick Install Guide

## Requirements
* Rocky Linux 9 or Ubuntu 22.05  
* For Containers:  
  * 3 network interfaces  
  * Mount the source folder into the container  
* For Bare Metal or Virtual Machines:  
  * 3 network interfaces, or the ability to configure alias interfaces  

## Compile and Install BRIO

1) **Enter the project directory**  
   ```
   cd project-directory
   ```
   Navigate to the directory where the BRIO source code is located.

2) **Install dependencies**  
   ```
   ./install_dependencies.sh --confirm
   ```
   This script installs all required system libraries and dependencies. It 
   automatically detects the operating system and selects the appropriate 
   requirements file from the `dependencies/` folder.

3) **Compile BRIO**  
   ```
   ./buildBRIO.sh
   ```
   The simplest way to compile BRIO is using the provided build script. The 
   installation will be placed in `project-directory/local`.

   If something goes wrong, clean the build environment with:
   ```
   ./buildBRIO.sh -D
   ```
   Then re-run the build script.

   **Note:**  
   If your system changes IP addresses after a reboot, you may need to 
   regenerate the example scripts:
   ```
   ./buildBRIO.sh EXPERIMENTS -D
   ./buildBRIO.sh
   ```
   This will **overwrite all scripts** in the installation folder and regenerate 
   all autogenerated files.  
   To customize these files, edit the corresponding template (`*.tpl`) files 
   instead.

## Prepare and Run Experiments

4) **Generate router configurations**  
   ```
   tools/generate-router-config.sh
   ```
   This utility scans the `local/opt/demo-aspa...` folder for router 
   configuration markup files and generates router configurations (currently 
   for NIST QuaggaSRX). The generated files are saved to `local/etc`.

   **Warning:**  
   Modified configuration files may be overwritten when this script is re-run. 
   Back up your changes before proceeding.

5) **Configure the router wrapper**  
   BRIO does not provide a router platform. Instead, it includes a wrapper script to interact with your selected router.

   Steps:
   * Copy the sample configuration file:
     ```
     cp local/opt/brio-examples/etc/rtr-wrapper.cfg.sample local/etc/rtr-wrapper.cfg
     ```
   * Alternatively:
     - Modify the `rtr-wrapper.sh` script directly  
     - Or pass **`--SKIP-ROUTER`** to the example script to skip router control

6) **Start the BGP router**  
   You can start the router in two ways:
   * Automatically via the example script (see step 8)  
   * Manually as an external process (see "External BGP Routers")

   Use the generated configuration files, and modify them if needed for your 
   specific router setup.

7) **Navigate to an example directory**  
   ```
   cd local/opt/brio-examples/demo-aspa-...stream
   ```
   Choose either the `upstream` or `downstream` folder.

8) **Run the experiment**  
   ```
   ./start_expXYZ.sh
   ```
   Launches example **XYZ**.

   Options:
   * `--SKIP-ROUTER`: Skip router startup
   * To change router behavior, edit:
     ```
     local/opt/brio-examples/bin/rtr-wrapper.sh
     ```
     This file is well documented.

## External BGP Routers

The BRIO automation scripts assume local router control. To use an external BGP 
router, follow these steps:

1) Edit the file:
   ```
   IP-Address.cfg
   ```
   Set the appropriate IP address for the correct ASN.

   Refer to the router configuration markup file attached to each example for
   guidance.

2) Rebuild the example scripts:
   ```
   ./buildBRIO.sh EXAMPLES
   ```
3) Configure the BGP Router
   Each example has a corresponding BGP router configuration page that explains
   the settings.
   The tools foldr contains a sample router configuration builder, currently 
   only for QuaggaSRx routers but can easily be adopted to fit other installs.
   ```
   tools/generate-router-config.sh
   ```

## Additional Notes

* Run any example script with `-?` to display help:
  ```
  ./start_expXYZ.sh -?
  ```

* Each example script may support slightly different options depending on its 
  configuration.

[back](README.md)
