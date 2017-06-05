#include "Node.h"

#include <math.h>

Node* Node::zero = NULL;

int Node::atti(string key) {
	return atoi( atts[key].c_str() );
}

float Node::attf(string key) {
	if(atts.count(key)) {
		return atof( atts[key].c_str() );
	}
	return NAN;
}

void Node::renderJSON(std::ostream& to) {
	string q = "\"";

	to<<"{"<<endl;
	to<<q<<"atts"<<q<<": {"<<endl;

	map<string, string>::iterator att;
	for ( att = atts.begin(); att != atts.end(); att++ ) {
		to << q << att->first << q
			<< ':'
			<< q << att->second << q
			<< endl ;
	}
	to<<"},"<<endl;

	to<<q<<"subs"<<q<<": {"<<endl;
	vector<Node*>::iterator p;
	for(p = subs.begin(); p != subs.end(); p++) {
		(*p)->renderJSON(to);
	}


	to<<"}"<<endl;
	to<<"}"<<endl;
}
