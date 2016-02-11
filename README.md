# SSG-C-
Segments Summary Graphs C++ Implementation 

The code is written for Academic Purposes in Intelligent Systems Laboratory - Bogazici University. 

### NONQT-SSG BRANCH 

This branch uses just the summary segments graphs of master and it is independent of QT(standalone version of master). 

### USAGE

First, add all header and source files into your project. 
Then, 

```c++
vector<vector<Nodesig> > ns_vector; // Create a vector 

while(1)
{
    frame = getFrameFunctionOfYourChoice(); // get current frame from camera
    seg_track.processImagesOnline(frame, ns_vec); // Run SSG code
    Mat M = seg_track.M; // Contains all segments and coherency information
}
```

### Platform 
Tested on both: 
- Windows
- Ubuntu