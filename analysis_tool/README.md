# Analysis Tool

The analysis tool is meant specifically to condense a video's Region of Interest (the area of the LED)
into pure numbers in a CSV file seperated as follows: (deltaTime, blue, green, red, bit). This is to ease
future processing steps since we have the relevant data condensed into 5 sets of `Float64` numbers rather
than having to process large video files at every step of the way.

### Commandline Options
The tool has the following command line options:
* **-s/--dataset**: This command line option tells the program that any further options need to be contextualised
as part of a greater dataset. This triggers the app to look for ground truth videos (LED ON and OFF). 
* **-d/--folder**: This command option sets the location of the folder where the analysis tool looks for the videos.
* **-f/--file**: The command option sets the location of the file the analysis tool opens and analyses.
* **-h/--help** Prints out all the options and command structure for the file.


### Possible use case
You have a video file called: `25Hz_100fps_10ph.avi` and want to generate the scalar mean for the region of interest.
All you have to do is: `$ ./analysis_tool.exe -f ./25Hz_100fps_10ph.avi`.

This will first open a window that asks for the ROI (LED Location) and then take the scalar mean of the ROI to generate
`25Hz_100fps_10ph.csv`.

You have a folder with the following video files and structure:

```
./ 
    5Hz_15fps_20c.avi
    15Hz_30fps_20c.avi
    30Hz_60fps_20c.avi
    50Hz_100fps_20c.avi
    25Hz_100fps_20c.avi
    1Hz_100fps_20c.avi
    on_100fps_20c.avi
    off_100fps_20c.avi
    analysis_tool.exe
```
And you want to deduce the bit values based on the two ground truth videos (`off` and `on`). You would do this by running:
`$ ./analysis_tool.exe -s -d .`.
This will tell the analysis tool to look in the current directory to look for the videos `-d .`, then consider them a dataset `-s`.
This will export a `.csv` file for every video including the two ground truths and for the exported videos, it also includes
a bit value based on thresholding.

### Sample CSV output

```
deltaTime,blue,green,red,bit
0.01,198.098,171.867,107.503,1
0.02,185.579,160.239,99.2207,0
0.03,185.577,160.368,99.2946,0
0.04,179.331,154.215,95.9737,0
0.05,179.176,154.14,96.0987,0
0.06,172.905,148.086,92.0129,0
0.07,172.597,147.857,91.8155,0
```