// ESE-549 Project 2 
// Abhishek S Yellanki
// 111095603

#include <iostream>
#include <fstream> 
#include <vector>
#include <time.h>
#include <map>
#include <set>
#include <list>
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

bool faultSortFunc(Fault a, Fault b);
vector<Fault> sortFaultList(vector<Fault> v);
void printFaultList(vector<Fault> l);


//--------------------------------------------------
// Add the function prototypes for any new functions you add here, between these lines.
int BUFF1(vector<char> *input_value)
{
    vector<char>::iterator it = input_value->begin();
	if (*it == LOGIC_ONE)
	    return 1;
	else
	    return 0;
}

int NOT1(vector<char> *input_value){
    vector<char>::iterator it = input_value->begin();
	if (*it == LOGIC_ONE)
	    return 0;
	else 
	    return 1;
}

int AND1(vector<char> *input_value){
      int count_X = 0, count_1 = 0, count_0 = 0, count_D = 0, count_DBAR = 0;
	    for (vector<char>::iterator it = input_value->begin(); it != input_value->end(); it++){
		if(*it == LOGIC_ZERO)
		     return 0;
	    }
	    return 1;	
}

int NAND1(vector<char> *input_value){
       	if (AND1(input_value) == 0)
	    return 1;
	else
	    return 0;
}

int OR1(vector<char> *input_value){
      	int count_1 = 0, count_X = 0, count_0 = 0, count_D = 0, count_DBAR = 0;
	    for (vector<char>::iterator it = input_value->begin(); it != input_value->end(); it++){
	   	if(*it == LOGIC_ONE)
		    return 1; 
	    }
	    return 0;
} 

int NOR1(vector<char> *input_value){
       	if (OR1(input_value) == 0)
	    return 1;
	else
	    return 0;
}

int XOR1(vector<char> *input_value){
 	vector<char>::iterator it = input_value->begin();
	    if (*it == *(it + 1))
		return 0;
	    else
		return 1;
}

int XNOR1(vector<char> *input_value){
	    if (XOR1(input_value) == 1)
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
	   case 10:
	     {
		return BUFF1(input_value);
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
    else if (a == "FANOUT")
	return 10;
    else
	return -1;
} 

map<string, bool> hashValues;	//keeps track of all the visited gates

// This function traverses the circuit using Recursion to get o/p of each gate:

char set_OutputGateValue(Gate * input_gate){
	vector<Gate *> InputGates = input_gate->get_gateInputs();
	vector<char> *gateValues = new vector<char>;
	string input_gateName = input_gate->gateTypeName();
	char newVal;	
	if(InputGates.size() == 0){
	    return input_gate->getValue();
	}
	else{	
	    for(int i = 0; i < InputGates.size(); i++){
		if(hashValues.find(InputGates[i]->get_outputName()) != hashValues.end()){
			newVal = InputGates[i]->getValue();
		}
		else{	
			newVal = set_OutputGateValue(InputGates[i]);
			hashValues[InputGates[i]->get_outputName()] = true;
		}

		gateValues->push_back(newVal); 		
	    }
	    input_gate->setValue(get_Output(gateType(input_gateName), gateValues));
	    return input_gate->getValue();
	}
}

//the function to compute gate values ends here

//The below function is used for deductive fault simulation

Fault outGateList(Gate* input_gate){
    Fault tempVar;
    tempVar.loc = input_gate;
    if(input_gate->getValue() == LOGIC_ZERO)
	tempVar.type = LOGIC_ONE;
    else
	tempVar.type = LOGIC_ZERO;
    return tempVar;
}   

//Implementation of AND gate - to find the fault list of the output of an AND gate.
//Since AND and NAND gates have '0' as the controlling values, the same function can be used for both. 
vector<Fault> AND2(vector< map<char, vector<Fault> > > faultmap, Gate* input_gate){
    vector<Fault> temp_FaultlistZero, temp_Faultlist;
    vector<Fault> temp_FaultlistOne;
    map<Gate *, bool> tempHashTableZero, tempHashTableOne;	
    vector<map<char, vector<Fault> > >::iterator it = faultmap.begin();
    int count_zero = 0, count_one = 0;
    for(it; it != faultmap.end(); it++){
	if((*it).find(LOGIC_ZERO) != (*it).end()){
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ZERO].begin();
	    count_zero++;
	    if(count_zero == 1){
	    	for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    tempHashTableZero[(*it2).loc] = true;
		    temp_FaultlistZero.push_back(*it2);
		}
	    }
	    else{
		temp_FaultlistZero.clear();
		for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    if(tempHashTableZero[(*it2).loc]) 
			temp_FaultlistZero.push_back(*it2);
		}
		tempHashTableZero.clear();
		for(int i = 0; i < temp_FaultlistZero.size(); i++){
		    tempHashTableZero[temp_FaultlistZero[i].loc] = true;
		}
	    }
	}
	else if ((*it).find(LOGIC_ONE) != (*it).end()){
	    count_one++;
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ONE].begin();
	    if(count_one == 1){
	    	for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    tempHashTableOne[(*it2).loc] = true;
		    temp_FaultlistOne.push_back(*it2);
		}
			
	    }
	    else{
		for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    if(!tempHashTableOne[(*it2).loc]){
			tempHashTableOne[(*it2).loc] = true;
			temp_FaultlistOne.push_back(*it2);
		    }
		}
	    }
	}
    }
	
    if(count_zero == 0){
	temp_FaultlistOne.push_back(outGateList(input_gate));
	return temp_FaultlistOne;    
    }
    else{
	temp_Faultlist.clear();
	for(vector<Fault>::iterator iter = temp_FaultlistZero.begin();iter != temp_FaultlistZero.end(); iter++){
	    if(!tempHashTableOne[(*iter).loc])
		temp_Faultlist.push_back(*iter);
	}
	temp_Faultlist.push_back(outGateList(input_gate));
	return temp_Faultlist;
    } 
}

//Implementation of OR gate - to find the fault list of the output of an OR gate.
//Since OR and NOR gates have '1' as the controlling values, the same function can be used for both.
vector<Fault> OR2(vector< map<char, vector<Fault> > > faultmap, Gate* input_gate){
    vector<Fault> temp_FaultlistZero, temp_Faultlist;
    vector<Fault> temp_FaultlistOne;
    map<Gate *, bool> tempHashTableZero, tempHashTableOne;	
    vector<map<char, vector<Fault> > >::iterator it = faultmap.begin();
    int count_zero = 0, count_one = 0;
    for(it; it != faultmap.end(); it++){
	if((*it).find(LOGIC_ONE) != (*it).end()){
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ONE].begin();
	    count_one++;
	    if(count_one == 1){
	    	for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    tempHashTableOne[(*it2).loc] = true;
		    temp_FaultlistOne.push_back(*it2);
		}
	    }
	    else{
		temp_FaultlistOne.clear();
		for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    if(tempHashTableOne[(*it2).loc]) 
			temp_FaultlistOne.push_back(*it2);
		}
		tempHashTableOne.clear();
		for(int i = 0; i < temp_FaultlistOne.size(); i++){
		    tempHashTableOne[temp_FaultlistOne[i].loc] = true;
		}
	    }
	}
	else{
	    count_zero++;
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ZERO].begin();
	    if(count_zero == 1){
	    	for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    tempHashTableZero[(*it2).loc] = true;
		    temp_FaultlistZero.push_back(*it2);
		}
	    }
	    else{
		for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    if(!tempHashTableZero[(*it2).loc]){
			tempHashTableZero[(*it2).loc] = true;
			temp_FaultlistZero.push_back(*it2);
		    }
		}
	    }
	}
    }
    if(count_one == 0){
	temp_FaultlistZero.push_back(outGateList(input_gate));
	return temp_FaultlistZero;    
    }
    else{
	for(vector<Fault>::iterator iter = temp_FaultlistOne.begin();iter != temp_FaultlistOne.end(); iter++){
	    if(!tempHashTableZero[(*iter).loc])
		temp_Faultlist.push_back(*iter);
	}
	temp_Faultlist.push_back(outGateList(input_gate));
	return temp_Faultlist;
    } 
}

//Implementation of XOR gate - Computes union and intersection separately and then subtracts both the sets:
//This implementation works for both XOR and XNOR gates as the fault deduction process is the same for both.
vector<Fault> XOR2(vector< map<char, vector<Fault> > > faultmap, Gate* input_gate){
    vector<Fault> temp_FaultlistOne, temp_FaultlistTwo, temp_Faultlist;
    map<Gate *, bool> tempHashTableOne, tempHashTableTwo;	
    vector<map<char, vector<Fault> > >::iterator it = faultmap.begin();
    int count = 0;
    //This loop computes the union of all the inputs;
    for(it; it != faultmap.end(); it++){
            if((*it).find(LOGIC_ZERO) != (*it).end()){
		vector<Fault>::iterator it2 = (*it)[LOGIC_ZERO].begin();
	    	for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    if(!tempHashTableOne[(*it2).loc]){
		    	tempHashTableOne[(*it2).loc] = true;
		    	temp_FaultlistOne.push_back(*it2);
		    }
		}
	    }
	    if((*it).find(LOGIC_ONE) != (*it).end()){
		vector<Fault>::iterator it3 = (*it)[LOGIC_ONE].begin();
	    	for(it3; it3 != (*it)[LOGIC_ONE].end(); it3++){
		    if(!tempHashTableOne[(*it3).loc]){
		    	tempHashTableOne[(*it3).loc] = true;
		    	temp_FaultlistOne.push_back(*it3);
		    }
		}
	    }
    }

    //This loop computes the intersection of all the inputs.
    it = faultmap.begin();
    for(it; it != faultmap.end(); it++){
	if((*it).find(LOGIC_ZERO) != (*it).end()){
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ZERO].begin();
	    count++;
	    if(count == 1){
	    	for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    tempHashTableTwo[(*it2).loc] = true;
		    temp_FaultlistTwo.push_back(*it2);
		}
	    }
	    else{
		temp_FaultlistTwo.clear();
		for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++){
		    if(tempHashTableTwo[(*it2).loc]) 
			temp_FaultlistTwo.push_back(*it2);
		}
		tempHashTableTwo.clear();
		for(int i = 0; i < temp_FaultlistTwo.size(); i++){
		    tempHashTableTwo[temp_FaultlistTwo[i].loc] = true;
		}
	    }
	}
	else if((*it).find(LOGIC_ONE) != (*it).end()){
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ONE].begin();
	    count++;
	    if(count == 1){
	    	for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    tempHashTableTwo[(*it2).loc] = true;
		    temp_FaultlistTwo.push_back(*it2);
		}
	    }
	    else{
		temp_FaultlistTwo.clear();
		for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++){
		    if(tempHashTableTwo[(*it2).loc]) 
			temp_FaultlistTwo.push_back(*it2);
		}
		tempHashTableTwo.clear();
		for(int i = 0; i < temp_FaultlistTwo.size(); i++){
		    tempHashTableTwo[temp_FaultlistTwo[i].loc] = true;
		}
	    }
	}    
    }

    for(vector<Fault>::iterator iter = temp_FaultlistOne.begin();iter != temp_FaultlistOne.end(); iter++){
	if(!tempHashTableTwo[(*iter).loc])
	    temp_Faultlist.push_back(*iter);
    }
    temp_Faultlist.push_back(outGateList(input_gate));
    return temp_Faultlist;
} 

//Implementation of BUFFER, NOT gate, FANOUT - returns the fault list at the output of the gate. 
vector<Fault> BUFF2(vector< map<char, vector<Fault> > > faultmap, Gate* input_gate){
	vector<Fault> temp_Faultlist;
	vector< map<char, vector<Fault> > >::iterator it = faultmap.begin();
	for(it; it != faultmap.end(); it++){
	if((*it).find(LOGIC_ZERO) != (*it).end()){
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ZERO].begin();
	    for(it2; it2 != (*it)[LOGIC_ZERO].end(); it2++)
		temp_Faultlist.push_back(*it2);
	}
	else{
	    vector<Fault>::iterator it2 = (*it)[LOGIC_ONE].begin();
	    for(it2; it2 != (*it)[LOGIC_ONE].end(); it2++)
		temp_Faultlist.push_back(*it2);
	}
    }
	temp_Faultlist.push_back(outGateList(input_gate));
	return temp_Faultlist;
}

//get the output faults list:
vector<Fault> get_outputFaultlist(int gateType, vector<map<char, vector<Fault> > > faultmap, Gate* input_gate){
	switch(gateType){
	   case 0:
	      { 
		// AND/NAND Gate implementation:
		return AND2(faultmap, input_gate);
		break;
	      }
	   case 1: 
	      {  
		// OR/NOR Gate implementation:
		return OR2(faultmap, input_gate);	
		break;
	      }
	   case 2:
	      { 
		// AND/NAND Gate implementation:
		return AND2(faultmap, input_gate);
		break;
	      }
	   case 3: 
	      {  
		// OR/NOR Gate implementation:
		return OR2(faultmap, input_gate);	
		break;
	      }
	   case 4:
	     {
		// XOR/XNOR Gate implementation:
		return XOR2(faultmap, input_gate);
		break;
	     }
	   case 5:
	     {
		// XOR/XNOR Gate implementation:
		return XOR2(faultmap, input_gate);
		break;
	     }	
	   case 6:
	     {
		// BUFF/FANOUT Gate implementation:
		return BUFF2(faultmap, input_gate);  
		break;
	     }
	   case 7:
	     {
		// NOT Gate implementation:
		return BUFF2(faultmap, input_gate);
		break;
	     }
	   case 10:
	     {
		// BUFF/FANOUT Gate implementation:
		return BUFF2(faultmap, input_gate);  
		break;
	     }
	   default:
		cout << "Wrong Gate" << endl;
	}	
}


//the function check what type the output gate is and its inputs and then determines the fault list for it.
map<string, bool> hashmapFaults;
vector<Fault> get_faultList(Gate * input_gate){
	vector<Gate *> InputGates = input_gate->get_gateInputs();
	vector<Fault> gateValues;
	string input_gateName = input_gate->gateTypeName();
	vector<Fault> newVal;
	map<char, vector<Fault> > hashFaults;
	vector<map<char, vector<Fault> > > store_FaultList;
	
	if(InputGates.size() == 0){
	    return input_gate->get_detectableFaults();
	}
	else{	 
	    for(int i = 0; i < InputGates.size(); i++){
				
		if(hashmapFaults.find(InputGates[i]->get_outputName()) != hashmapFaults.end()){
			newVal = InputGates[i]->get_detectableFaults();
		}
		else{	
			newVal = get_faultList(InputGates[i]);
			hashmapFaults[InputGates[i]->get_outputName()] = true;
		}
		
		gateValues.clear();
		vector<Fault>::iterator it = newVal.begin();
		for(it; it != newVal.end(); it++){
		    gateValues.push_back(*it); 		
		}	
		hashFaults.clear();
		hashFaults[InputGates[i]->getValue()] = gateValues;
                store_FaultList.push_back(hashFaults);
	    }	    
	    input_gate->set_detectableFaults(get_outputFaultlist(gateType(input_gateName), store_FaultList, input_gate));
	    return input_gate->get_detectableFaults();
	}
}


//-------------------------------------------------


int main(int argc, char* argv[]) {

  // Check the command line input and usage
  if (argc != 4) {
    cout << "Usage: ./logicsim circuit_file input_vectors output_loc" << endl;
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
  inputStream.open(argv[2]);
  string inputLine;
  if (!inputStream.is_open()) {
    cout << "ERROR: Cannot read file " << argv[2] << " for input" << endl;
    return 1;
  }

  // Setup the output text file
  ofstream outputStream;
  outputStream.open(argv[3]);
  if (!outputStream.is_open()) {
    cout << "ERROR: Cannot open file " << argv[3] << " for output" << endl;
    return 1;
  }
  
  // Try to read a line of inputs from the file.
  while(getline(inputStream, inputLine)) {
    
    // Clear logic values and detectable faults in my circuit
    myCircuit->clearGateValues();

    // Set new logic values
    myCircuit->setPIValues(constructInputLine(inputLine));

    // Initialize the fault lists for the PIs of the circuit.
    // If the PI == 0, then it can detect sa1; if the PI == 1, it can detect sa0
    vector<Gate*> piGates = myCircuit->getPIGates();	
    for (int i=0; i < piGates.size(); i++) {
      if (piGates[i]->getValue() == LOGIC_ZERO) {
        Fault f = {piGates[i], FAULT_SA1};
        piGates[i]->add_detectableFault(f);
      }
      else if (piGates[i]->getValue() == LOGIC_ONE) {
	     Fault f = {piGates[i], FAULT_SA0};
	     piGates[i]->add_detectableFault(f);
      }
    }

    // ------------------------------------------------------------
    // Add your logic and deductive fault simulation code here.
	vector<Gate*> circuitPOs = myCircuit->getPOGates(); 	
	vector<Gate *>::iterator it = circuitPOs.begin();
	for(it; it != circuitPOs.end(); it++)
		set_OutputGateValue(*it);
	hashValues.clear();//clears the hashmap

	for(it = circuitPOs.begin(); it != circuitPOs.end(); it++)
		get_faultList(*it);
	hashmapFaults.clear();

    // Stop writing your code here!
    //
    // ---------------------------------------------------------------------------
    // 
    // The following code stores the circuit's output values and detectable faults
    // for each test vector.
    // Do not change any code from here until the end of the main() function.
	
    
    // For each test vector, print the outputs and then the faults detectable at each gate.
    outputStream << "--" << endl;
    vector<Gate*> outputGates = myCircuit->getPOGates();
    for (int i=0; i < outputGates.size(); i++) {
      outputStream << outputGates[i]->printValue();
    }
    outputStream << endl;
      
	  
    for (int i=0; i<myCircuit->getNumberGates(); i++) {
      vector<Fault> v = myCircuit->getGate(i)->get_detectableFaults();
	
      // I am sorting each fault list so the result is independent of the order
      // you insert them into the lists.
      v=sortFaultList(v);
      outputStream << myCircuit->getGate(i)->get_outputName() << ": ";
      outputStream << "{";
    	
      if (v.size() > 1) {		
        for (int j=0; j<v.size()-1; j++) {
          outputStream << v[j].loc->get_outputName() << "/" << (int)(v[j].type) << ", ";
        }
        outputStream << (v[v.size()-1]).loc->get_outputName() << "/" << (int)((v[v.size()-1]).type) << "}" << endl;
      }
      else if (v.size() == 1)  {
        outputStream << v[0].loc->get_outputName() << "/" << (int)(v[0].type) << "}" << endl;
      }    	
      else if (v.size() == 0)
        outputStream << "}" << endl;
    }
  }
  
  // close the input and output streams
  outputStream.close();
  inputStream.close();
  
  
  return 0;
}

// Just used to parse in the values from the 
vector<char> constructInputLine(string line) {
  
  vector<char> inputVals;
  
  for (int i=0; i<line.size(); i++) {
    if (line[i] == '0') 
      inputVals.push_back(LOGIC_ZERO);
    
    else if (line[i] == '1') 
      inputVals.push_back(LOGIC_ONE);

    else if ((line[i] == 'X') || (line[i] == 'x')) {
      cout << "ERROR: Project 2 (faultsim) does not support X inputs." << endl;
      assert(false);		
      //inputVals.push_back(LOGIC_X);
    }

    else if (line[i] == 13) // ignore Windows-style newlines
      ;
   
    else {
      cout << "ERROR: Do not recognize character " << line[i] << " in line " << i+1 << " of input vector file." << endl;
      assert(false);
      //inputVals.push_back(LOGIC_X);
    }
  }
  
  return inputVals;

}

// A function to sort a fault list based on the name of the gate.
// I am sorting the Fault lists before outputting so that every correct answer
// will produce the same faults in the same order.
vector<Fault> sortFaultList(vector<Fault> v) {
  sort(v.begin(), v.end(), faultSortFunc);
  return v;
}

// A helper function for sortFaultList().
bool faultSortFunc(Fault a, Fault b) { 
  return (a.loc->get_outputName() < b.loc->get_outputName());
}


// A compact way to print a fault list. This is just here in case it is helpful for 
// you when debugging your code.
void printFaultList(vector<Fault> l) {
  cout << "{";
  if (l.size() > 1) {		
    for (int i=0; i<l.size()-1; i++) {
      cout << l[i].loc->get_outputName() << "/" << (int)(l[i].type) << ", ";
    }
    cout << (l[l.size()-1]).loc->get_outputName() << "/" << (int)((l[l.size()-1]).type) << "}" << endl;
  }
  else if (l.size() == 1) 
    cout << l[0].loc->get_outputName() << "/" << (int)(l[0].type) << "}" << endl;

  else if (l.size() == 0)
    cout << "}" << endl;
		
}



////////////////////////////////////////////////////////////////////////////
// Place any new functions you add here, between these two bars.



////////////////////////////////////////////////////////////////////////////
