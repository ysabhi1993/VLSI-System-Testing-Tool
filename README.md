# VLSI-Testing-Tool
Designed a VLSI Testing tool using C++.

## Getting Started
The aim of this project is to create a VLSI simulation and test tool that detects if the circuit is faulty. If it is, test vectors are generated that detect all the faults.

### Prerequisites
Visual Studio/ Visual Studio Code

### Project Phases
* Fault-free Circuit Simulation: Here the logical output at each gate of the VLSI circuit are generated. Each gate instantiation stores its respective output.
* Simulate the Circuit to store all possible faults: This phase simulates the circuit and returns a list of all possible faults at each fan-in and fan-out location.
* Implementing PODEM: The Final Phase of the project is to implement the Algorithm that generates test vector to detect a stuck-at fault at any of the fan-in or fan-out locations.

### Optimization
* Initially the code tested for each fault. It was optimized to detect all possible faults with each test vector and count them as detected from the next fault simulation cycle.
* Fault-free Circuit Simulation algorithm was enhanced by using a different technique which generates output based on events.
* These two optimizations decresed the running time by nearly 60%.

### Testing
We tested the circuit with three different circuit sizes. 
* One with 96 gates - detected all the faults in 33sec.
* One with 432 gates - detected 99.3 % faults in 4min 45sec.
* One with 5535 gates - detected 98.6 % faults in 12min 36sec. 

 
### Author
**Abhishek S Yellanki** - coded the Fault-free circuit simulation algorithm and test vector generation algorithm(PODEM).
