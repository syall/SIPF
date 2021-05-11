#include "GateNode.hpp"
#include "QASMparser.h"
#include <cassert>
#include <cstring>
#include <iostream>
using namespace std;

/** 
  * Parses qasm file and build dependence graph
  * @param qasmFileName Input: the filepath for the circuit
  * @param firstGates Output (passed byref): The roots of the dependence graph
  * @param numQubits Output (passed byref): The number of contiguous (logical) qubits in the circuit
  * @param numGates Output (passed byref): The number of gates in the circuit
  */
void buildDependencyGraph(string qasmFileName, set<GateNode*> & firstGates, int & numQubits, int & numGates) {
	//parse qasm
	QASMparser* parser = new QASMparser(qasmFileName);
	parser->Parse();
	std::vector<std::vector<QASMparser::gate> > layersOrg = parser->getLayers();
	int maxQubits = (int)parser->getNqubits();
	std::vector<QASMparser::gate> gates;
	for (unsigned int i = 0; i < layersOrg.size(); i++) {
		std::vector<QASMparser::gate> layer = layersOrg.at(i);
		for (unsigned int j = 0; j < layer.size(); j++) {
			gates.push_back(layer.at(j));
		}
	}
	numGates = gates.size();
	delete parser;
	
	//build dependence graph
	GateNode ** lastGatePerQubit = new GateNode*[maxQubits];
	for(int x = 0; x < maxQubits; x++) {
		lastGatePerQubit[x] = 0;
	}
	for(unsigned int x = 0; x < gates.size(); x++) {
		GateNode * v = new GateNode;
		v->control = gates.at(x).control;
		v->target = gates.at(x).target;
		v->name = gates.at(x).type;
		v->controlChild = 0;
		v->targetChild = 0;
		v->controlParent = 0;
		v->targetParent = 0;
		
		if(v->control >= numQubits) {
			numQubits = v->control + 1;
		}
		if(v->target >= numQubits) {
			numQubits = v->target + 1;
		}
		
		assert(v->control != v->target);
		
		//set parents, and adjust lastGatePerQubit
		if(v->control >= 0) {
			v->controlParent = lastGatePerQubit[v->control];
			if(v->controlParent) {
				if(lastGatePerQubit[v->control]->control == v->control) {
					lastGatePerQubit[v->control]->controlChild = v;
				} else {
					lastGatePerQubit[v->control]->targetChild = v;
				}
			}
			lastGatePerQubit[v->control] = v;
		}
		if(v->target >= 0) {
			v->targetParent = lastGatePerQubit[v->target];
			if(v->targetParent) {
				if(lastGatePerQubit[v->target]->control == v->target) {
					lastGatePerQubit[v->target]->controlChild = v;
				} else {
					lastGatePerQubit[v->target]->targetChild = v;
				}
			}
			lastGatePerQubit[v->target] = v;
		}
		
		//if v is a root gate, add it to firstGates
		if(!v->controlParent && !v->targetParent) {
			firstGates.insert(v);
		}
	}
	
	assert(numQubits <= maxQubits);
	
	delete [] lastGatePerQubit;
}

/**
  * Parses the coupling map
  * @param filename Input: The filepath for the coupling map
  * @param edges Output (passed byref): The set of edges in the coupling map
  * @param numPhysicalQubits Output (passed byref): The number of qubits in the coupling map
  */
void buildCouplingMap(string filename, set<pair<int, int> > & edges, int & numPhysicalQubits) {
	std::fstream myfile(filename, std::ios_base::in);
	unsigned int numEdges;
	
	myfile >> numPhysicalQubits;
	myfile >> numEdges;
	for(unsigned int x = 0; x < numEdges; x++) {
		int a, b;
		myfile >> a;
		myfile >> b;
		pair<int, int> edge = make_pair(a,b);
		edges.insert(edge);
	}
}
