
// Name: Abhishek S Yellanki
// ID : 111095603



#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include <time.h>
#include "parse_bench.tab.h"
#include "ClassCircuit.h"
#include "ClassGate.h"

using namespace std;

// Just for the parser
extern "C" int yyparse();
extern FILE *yyin; // Input file for parser

// Our circuit. We declare this external so the parser can use it more easily.
extern Circuit* myCircuit;

vector<char> constructInputLine(string line);


//----------------
// If you add functions, please add the prototypes here.

int BUFF1(vector<char> *input_value)
{
    vector<char>::iterator it = input_value->begin();
	if (*it == '1')
	    return 1;
	else if (*it == 'X')
	    return 4;
	else if (*it == 'D')
	    return 2;
	else if (*it == 'B')
	    return 3;
	else
	    return 0;
}

int NOT1(vector<char> *input_value){
    vector<char>::iterator it = input_value->begin();
	if (*it == 'X')
	    return 4;
	else if (*it == 'D')
	    return 3;
	else if (*it == 'B')
	    return 2;
	else if (*it == '1')
	    return 0;
	else 
	    return 1;
}

int AND1(vector<char> *input_value){
      int count_X = 0, count_1 = 0, count_0 = 0, count_D = 0, count_DBAR = 0;
	    for (vector<char>::iterator it = input_value->begin(); it != input_value->end(); it++){
		if(*it == '1')
		    count_1 += 1;
		else if (*it == 'X')
		    count_X += 1;
		else if (*it == 'D')
		    count_D += 1;
		else if (*it == 'B')
		    count_DBAR += 1;
		else if (*it == '0')
		    count_0 += 1;
	    }   
	    	if(count_0 > 0)
		    return 0;
		else if(count_X > 0)
		    return 4;
		else if(count_D > 0 && count_DBAR > 0)
		     return 0;
		else if(count_D > 0)
		     return 2;
		else if (count_DBAR > 0)
		     return 3;
		else
		     return 1;
		
}

int NAND1(vector<char> *input_value){
       	if (AND1(input_value) == 0)
	    return 1;
	else if (AND1(input_value) == 1)
	    return 0;
	else if (AND1(input_value) == 2)
	    return 3;
	else if (AND1(input_value) == 3)
	    return 2;
	else
	    return 4;
}

int OR1(vector<char> *input_value){
      	int count_1 = 0, count_X = 0, count_0 = 0, count_D = 0, count_DBAR = 0;
	    for (vector<char>::iterator it = input_value->begin(); it != input_value->end(); it++){
	    	if(*it == '1')
		    count_1 += 1;
		else if (*it == 'X')
		    count_X += 1;
		else if (*it == 'D')
		    count_D += 1;
		else if (*it == 'B')
		    count_DBAR += 1;
		else 
		    count_0 += 1;
	    }   
	    	if(count_1 > 0)
		    return 1;
		else if(count_X > 0)
		     return 4;
		else if(count_D > 0 && count_DBAR > 0)
		     return 1;
		else if (count_D > 0)
		     return 2;
		else if (count_DBAR > 0)
		     return 3;
		else
		     return 0;
		} 
} 

int NOR1(vector<char> *input_value){
       	if (OR1(input_value) == 0)
	    return 1;
	else if (OR1(input_value) == 1)
	    return 0;
	else if (OR1(input_value) == 2)
	    return 3;
	else if (OR1(input_value) == 3)
	    return 2;
	else
	    return 4;
}

int XOR1(vector<char> *input_value){
 	vector<char>::iterator it = input_value->begin();
	    if(*it == 'X' || *(it + 1) == 'X' )
		return 4;
	    else if ((*it == 'D' && *(it + 1) == 'B') || (*it == 'B' && *(it + 1) == 'D'))
		return 1;
	    else if (*it == *(it + 1))
		return 0;
	    else
		return 1;
}

int XNOR1(vector<char> *input_value){
	//vector<char>::iterator it = input_value->begin();
	    if(XOR1(input_value) == 4)
		return 4;
	    else if (XOR1(input_value) == 3)
		return 2;
	    else if (XOR1(input_value) == 2)
		return 3;
	    else if (XOR1(input_value) == 1)
		return 0;
	    else
		return 1;
}

int get_Output(int gateType, vector<char> *input_value){
switch(gateType){
   case 0:
      { 
	// NAND Gate implementation:
	return NAND1(input_value);
	break;
      }
   case 1: 
      {  
	// NOR Gate implementation:
	return NOR1(input_value);	
	break;
      }
   case 2:
     {
	// AND Gate implementation:
	return AND1(input_value);
	break;
      }
   case 3:
     {
	// OR Gate implementation:
	return OR1(input_value);
     break;
     }
   case 4:
     {
	// XOR Gate implementation:
	return XOR1(input_value);
	break;
     }
   case 5:
     {		
	// XNOR Gate implementation:
	return XNOR1(input_value);
	break;
     }	
   case 6:
     {
	// BUFF Gate implementation:
	return BUFF1(input_value);  
	break;
     }
   case 7:
     {
	// NOT Gate implementation:
	return NOT1(input_value);
	break;
     }
   default:
	cout << "Wrong Gate" << endl;
}	
}

int gateType(string a){
    if (a == "NAND")
	return 0;
    else if (a == "NOR")
	return 1;
    else if (a == "AND")
	return 2;
    else if (a == "OR")
	return 3;
    else if (a == "XOR")
	return 4;
    else if (a == "XNOR")
	return 5;
    else if (a == "BUFF")
	return 6;
    else if (a == "NOT")
	return 7;
    else
	return -1;
} 
		

// This function traverses the circuit using Recursion to get o/p of each gate:

void set_OutputGateValue(vector<Gate*> input_gates){
    vector<char> *gateValues = new vector<char>;
      for (int i = 0; i < input_gates.size(); i++){	
	vector<Gate*> inputGates = input_gates[i]->get_gateInputs();
	string input_gateName = input_gates[i]->gateTypeName();
	if (inputGates.size() == 0){
	        gateValues->push_back(input_gates[i]->getValue());
	}
	else{	
		set_OutputGateValue(inputGates);
		input_gates[i]->setValue(get_Output(gateType(input_gateName), gateValues));
      	}
	
      }
	return;

}

		  
//-----------------


int main(int argc, char* argv[]) {

  // Check the command line input and usage
  if (argc != 4) {
    cout << "Usage: ./logicsim circuit_file input_vectors output_location" << endl;
    return 1;
  }

  // Parse the bench file and initialize the circuit. (Using C style for our parser.)
  FILE *benchFile = fopen(argv[1], "r");
  if (benchFile == NULL) {
    cout << "ERROR: Cannot read file " << argv[1] << " for input" << endl;
    return 1;
  }
  yyin=benchFile;
  yyparse();
  fclose(benchFile);

  myCircuit->setupCircuit(); 
  cout << endl;

  
  // Setup the input vector file 
  vector<vector<char> > inputValues;
  ifstream inputStream; 
  inputStream.open(argv[2] );
  string inputLine;
  if (!inputStream.is_open()) {
    cout << "ERROR: Cannot read file " << argv[2] << " for input" <<    endl;
    return 1;
  }

  // Setup the output text file
  ofstream outputStream;
  outputStream.open(argv[3]);
  if (!outputStream.is_open()) {
    cout << "ERROR: Cannot open file " << argv[3] << " for output" << endl;
    return 1;
  }

  // Print the circuit structure. This is just so you can see how it works. Feel free to
  // comment this out. This function will also print any set logic values, so this can help
  // you in debugging.
  // myCircuit->printAllGates();    

  
  // Try to read a line of inputs from the file.
  while(getline(inputStream, inputLine)) {

    
    // clear logic values of my circuit
    myCircuit->clearGateValues();

    // set new logic values
    myCircuit->setPIValues(constructInputLine(inputLine));

    // ---------------------------------------------
    // Write your code here!
    
    // Compute the logic values of all gates in the circuit. When you
    // have calculated the logic value for a gate, use the setValue funtion
    // on it.

    // If you define other functions, place them at the bottom of this file 
    // and place the prototypes in the marked region above.
    //
    // Do *not* edit any of the other source files (ClassGate, ClassCircuit, etc.).

    // If you think there's something missing from the API that you need to solve
    // the problem, perhaps I missed something. Please let me know and we can discuss.
  
	vector<Gate*> circuitPOs = myCircuit->getPOGates(); 	
		set_OutputGateValue(circuitPOs);

	
    // Getting started:
    //   So now you have myCircuit, a pointer to a Circuit object, and you can
    //   get a vector<Gate*> of the circuit's input or output gates by running
    //   vector<Gate*> circuitPOs = myCircuit->getPOGates();
    //     and
    //   vector<Gate*> circuitPIs = myCircuit->getPIGates();

    // Once you have a Gate*, you can use its functions to go to its predecessors
    // and successors. 
    //
    // Here are two ideas for how you can set the circuit values in the gate:
    // 
	
    // Option 1: Do a breadth first traversal of the gates from the inputs.
    // Keep a queue of gates to examine. Start by putting all PI gates on
    // the queue. Then iteratively pop a Gate* from the queue, and check to
    // see if its input values are set. If they aren't put this Gate* back onto
    // the end of the queue. If they are set, calculate this gate's new
    // value (based on its gate type and the values of its inputs). Then
    // add this gate's successors (the gates that this gate's output goes to)
    // into the queue. Repeat until the queue is empty.
   
    // Option 2: You can write a recursive function starting from the POs.
    // If this gate's input values are unknown, recurse on the inputs to
    // assign their values. Eventually your recursive calls finish and
    // this gate's input values are known. Then set its output value
    // and return.

    // Feel free to delete these long comments when you don't need them anymore.
    
    
    // stop writing your code here.
    // ---------------------------------------------
    
	

    // Write the results we just simulated to the output file
    vector<Gate*> outputGates = myCircuit->getPOGates();
    for (int i=0; i < outputGates.size(); i++) {
      outputStream << outputGates[i]->printValue();
    }
    outputStream << endl;       
  }
  
  // close the input and output streams
  outputStream.close();
  inputStream.close();

  return 0;
}


// A function to help read in the input values from the text file. 
// You don't need to touch this.
vector<char> constructInputLine(string line) {
  
  vector<char> inputVals;
  
  for (int i=0; i<line.size(); i++) {
    if (line[i] == '0') 
      inputVals.push_back(LOGIC_ZERO);
    
    else if (line[i] == '1') 
      inputVals.push_back(LOGIC_ONE);

    else if ((line[i] == 'X') || (line[i] == 'x'))
      inputVals.push_back(LOGIC_X);
   
    else if ((line[i] == 'D') || (line[i] == 'd'))
      inputVals.push_back(LOGIC_D);

    else if ((line[i] == 'B') || (line[i] == 'b'))
      inputVals.push_back(LOGIC_DBAR);

    else {
      cout << "ERROR: Do not recognize character " << line[i] << " in line " << i+1 << " of input vector file. Setting to X" << endl;
      inputVals.push_back(LOGIC_X);
    }
  }
  
  return inputVals;

}

////////////////////////////////////////////////
// Please place any functions you add here, between these bars.


////////////////////////////////////////////////

