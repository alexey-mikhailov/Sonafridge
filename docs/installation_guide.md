Installation guide
==================

If you are not familiar with Unreal Engine plugins, you can read about them [here](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/Plugins/). 

### Building Unreal Engine plugin using the editor

1. Create an empty C++ project with the Engine version you wish to use.
1. Bring the plugin(s) to that C++ project
	- Create a "Plugins" folder in the C++ project's root folder.
	- Copy the plugin(s) folder(s) you want to build into the newly created "Plugins" folder.
1. Manually delete the “Intermediate” and “Binaries” folders of the plugin(s).
1. Double-click on your .uproject file to relaunch the Editor. You should see a popup telling you that some modules need to be rebuilt.   Click Accept.  This should rebuild your plugin(s).
1. Once done, copy the plugin from the compiled C++ project to the Engine's plugins folder, and overwrite any conflicting files.

### Building plugin using Visual Studio (Windows only)

1. Create an empty C++ project with the Engine version you wish to use.
1. Bring the plugin(s) to that C++ project
	- Create a "Plugins" folder in the C++ project's root folder.
	- Copy the plugin(s) folder(s) you want to build into the newly created "Plugins" folder.
1. Compile the C++ project in Visual Studio
	- Right click on the project's uproject file and select "Generate Visual Studio project files"
	- Double click on the *.sln file to launch Visual Studio
	- In Visual Studio select Development Editor and Win64 ( same workflow as for building the engine with VS).
	- Build the project.
4. Once that's done, copy the plugin from the compiled C++ project to the Engine's plugins folder, and overwrite any conflicting files.

If the build fails, the log file detailing the errors is under “YourProject/Saved/Logs/YourProject.log”. 

### Building plugin using Visual Studio using UAT (Windows only)

UAT means Unreal Automation Tool. You can read more about UAT [here](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/BuildTools/AutomationTool/). This method assumes you have built Unreal Engine from Source Code. 

1. Find the location where your Unreal Engine Source Code is installed.
1. Locate the folder where your plugin is located, and the *.uplugin file located in that folder.
1. Open a command line window
1. Change the directory to the Batchfiles folder packaged with the Engine Source Code:

	```bat
	cd [installed UE location]\Engine\Build\Batchfiles
	rem For Example: `cd D:\Epic Games\UE_5.0\Engine\Build\Batchfiles`
	```
1. Run the following command:

	```bat
	RunUAT.bat BuildPlugin -plugin="[path to .uplugin]" -package="[target dir]"
	rem For Example:
	rem RunUAT.bat BuildPlugin 
	rem -plugin="D:\MyPlugins\SomeFancyPlugin\SomeFancyPlugin.uplugin" 
	rem -package="D:\Temp\SomeFancyPlugin"
	```
1. Once done, copy the plugin from the temp folder specified by the package argument above to the Engine's plugins folder, and overwrite any conflicting files.

### Checking successful build

In main menu __Edit ➔ Plugins__ search __Sonafridge__. Ensure checkbox __Enabled__ is on. 

[Return to main page](index.md)

