# SVO Exporter

The svo export application is specifically designed to export ZED labs SVOs to AVIs and PNGs
without user presence. It requires Nvidia CUDA along with Stereolabs ZED SDK preinstalled on 
the system. This application makes it easy to export a large number of SVOs in any format that
we need (AVIs only OR AVIs + PNGs). This makes it easier to evaluate our data on computers not
equipped with Nvidia GPUs.

### Commandline Options
The tool has the following command line options:
* **-v/--video**: This command makes the svo tool only export AVIs from any videos it ingests.
* **-a/-all**: This command makes the svo tool output full decompositions (AVIs, Side-by-side PNGs and Depth PNGs).
* **-d/--folder**: This command option sets the location of the folder where the svo export tool looks for the videos.
* **-f/--file**:The command option sets the location of the file the svo export opens and exports.
* **-o/--output**: This command option sets a generic output name for our file exports required for full decompositions.
* **-h/--help**: Prints out all the options and command structure for the file.

### Possible use cases
We're going to use the following folder structure for each of the examples.
```
./ 
    5Hz_15fps_20c.svo
    15Hz_30fps_20c.svo
    30Hz_60fps_20c.svo
    50Hz_100fps_20c.svo
    25Hz_100fps_20c.svo
    1Hz_100fps_20c.svo
    on_100fps_20c.svo
    off_100fps_20c.svo
    svo_export.exe
```

**Case 1:** Exporting a single SVO into a video. Command to run: `$ ./svo_export.exe -v -f 25Hz_100fps_20c.svo`
Result: It will export the `25Hz_100fps_20c.svo` file into an AVI file called `25Hz_100fps_20c.avi` in the same folder as
the executable.

**Case 2:** Exporting a single SVO file into a complete decomposition. Command to run: `$ ./svo_export.exe -a -f 25Hz_100fps_20c.svo`
Result: It will export the file into an AVI and then create a folder with the name of the file `25Hz_100fps_20c` and store
depth and Side-By-Side PNGs in that folder.

**Case 3:** Exporting a folder of SVOs into videos. Command to run: `$ ./svo_export.exe -v -d .`
Result: It will export all SVOs in the current working directory with each AVI having the same name as it's origin SVO.

**Case 4:** Export a folder of SVOs into full decompositions. Command to run: `./svo_export.exe -a -d .`
Result: It will export all SVOs in the current working directory with each AVI and associated folder with the same name
as it's origin SVO.