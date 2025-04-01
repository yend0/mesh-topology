## __Mesh Topology__

## Instructions
### Build the project with cmake
Create a directory **build** in the root of the project or in a convenient place. Go to this directory. Write:
```
cmake [Path to CmakeLists.txt]
``` 
When cmake generates the necessary files, write ``make` while in the build directory.
### Executing the program
To run the program, you need to write in the terminal: 
```
./app-server
```
P.S.: if the program failed to start child processes, you should run it with administrator rights.

### Tests
To run the tests, you need to run the required test binaries that were built by the builder.
Example:
```
./app-test-compression
``` 
