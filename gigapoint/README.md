# Gigapoint

Version: 1.1.0

Gigapoint is an Omegalib module that can visualize potree data. Gigapoint can be built as an Omegalib module (default) or a standalone  app.

[Screenshots](http://www.toaninfo.com/work/2017-gigapoint.html)

**Contact**: Toan Nguyen ([http://monash.edu/mivp](http://monash.edu/mivp))

## Standalone app

Tested with MacOS 10.12.4

### Compile
```
cd gigapoint/app
mkdir build
cd build
cmake ..
make
```

### Run

Create a soft link or copy "gigapoint_resource" to build/working directory and run

```
./gigapoint path/to/configfile.cfg
```

## Omegalib module

Tested with Omegalib v13.1 on MacOS and OpenSUSE 12.3

### Compile

```
cd gigapoint
mkdir build
cd build
cmake ..
make
```
output is gigapoint.so

### Run

- Create a soft link or copy "gigapoint_resource" and gigapoint.so to working directory
- Create a python script to run. For example: run_gigapoint.py:

```
import gigapoint
gp = gigapoint.initialize()
gp.initPotree("gigapoint_resource/config/gigapoint_sample_local.json")
```

where <i>gigapoint_sample_local.json</i> is a configuration file that store options.

Please check sample scripts in "omegalib_module_test".


## Configuration

Input parematers are stored in a json file. A sample configuration file (other configuration files can be found in "gigapoint_resource/config":

<pre>
{
	"version": 1
	<b>"dataDir": "/path/to/potree_data",</b>
	"shaderDir": "/path/to/custom/shaders",
	"visiblePointTarget": 5000000,
	"minNodePixelSize": 100,
	"material": "rgb",
	"elevationDirection": 2,
	"elevationRange": [0, 0.5],
	"pointScale": [0.01,0.01,0.5],
	"pointSizeRange": [2, 400],
	"sizeType": "adaptive",
	"quality": "circle",
	"numReadThread": 6,
	"preloadToLevel": 4,
	"maxNodeInMem": 100000,
	"maxLoadSize": 300,
	"cameraSpeed": 10,
	"cameraPosition": [-90.478,-18.9424,466],
	"cameraTarget": [-90.478,-18.9424,464],
	"cameraUp": [0, 0, 1],
	"cameraOrientation": [0.63, 0.52,-0.34,-0.45],
	"filter": "edl",
	"filterEdl": [0.4, 1.4]
}
</pre>

<b>*bold: mandatory</b>

- version (int): version 1 uses cameraOrientation, other versions use cameraTarget parameter instead
- <b>dataDir (string)</b>: directory stores potree data. Defaults to current directory "./"
- shaderDir (string): points to your custom shaders (point.vert, point.frag, edl.vert, edl.frag). Defaults to "gigapoint_resource/shaders"
- visiblePointTarget (integer): target number of visible points on each rendering node. Defaults to 1000000 points
- minNodePixelSize (integer): octree nodes that have less than this value will be ignored. Defaults to 100
- material {"rgb", "elevation"}. Defaults to "rgb"
- elevationDirection ({0, 1, 2} for x, y, z axes respectively
- elevationRange (float array[2]): cutoff elevation range (z direction). Defaults to [0, 1]
- pointScale (float array[3]): [point scale value, min value, max value]. Defaults to [0.1, 0.01, 1.0]
- pointSizeRange (float array 2]: [minimum point size on screen, maximum point size on screen]. Defaults to [2, 50]
- sizeType {"fixed", "adaptive"}. Defaults to "adaptive"
- quality {"square", "circle", "sphere"} . Defaults to "square"
- numberReadThread (integer): number of loading threads. Defaults to 2
- preLoadToLevel (integer): preload potree data to this level. Defaults to 5
- maxNodeInMem (integer): maximum nodes to store in RAM. Defaults to 50000
- maxLoadSize (integer): maximum number of nodes store in loading queue. Defaults to 300
- cameraSpeed (float): Omegalib camera speed. Defaults to 10
- cameraPosition (float array[3]): inital position (x, y, z) of camera. Defaults to [0, 0, 0]
- cameraTarget (float array[3]): lookat point. Defaults to [0, 0, -2]. This parameter is used in version > 1
- cameraUp (float array[3]): up vector. Defaults to [0, 0, 1]
- cameraOrientation (float array[4]): inital orientation (quaterion) of camera. Defaults to [1, 0, 0, 0]. This parameter is used in version 1
- filter {"none", "edl"}: edl = Eye Dome Lighting. Defaults to "none"
- filterEdl (float array[2]): EDL options [EDL strength, EDL radius]. Defaults to [1.0, 1.4]

