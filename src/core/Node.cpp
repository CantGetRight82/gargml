#include "Node.h"

#include <math.h>

using namespace kiwi;

class NodeVar {
	public:
		Node* node;
		string key;
		NodeVar(Node* node, string key) : node(node), key(key) {
		}
};

Node* Node::zero = NULL;

void Node::initVars() {
	string keys[] = { "top", "right", "bottom", "left", "width", "height" };
	for(string key : keys) {
		vars[key] = Variable(key);
	}
}

void Node::constrain(Solver* solver) {
	solver->addConstraint( vars["width"] == vars["right"] - vars["left"] );
	solver->addConstraint( vars["height"] == vars["bottom"] - vars["top"] );
	if(!parent) {
		float width = attf("width");
		float height = attf("height");

		addStay(solver, "top", 0);
		addStay(solver, "right", width);
		addStay(solver, "bottom", height);
		addStay(solver, "left", 0);

	} else {
		string keys[] = {
			"top", "right", "bottom", "left", "width", "height"
		};

		bool side = atts["flow"] == "side";

		for(string key : keys) {
			if(atts.count(key)) {
				vector<string> vals = attsplit(key," ");

				int dir = 1;
				vector<Term> terms;

				terms.push_back( Term(vars[key],-1) );
				float constant = 0;
				for(string val : vals) {
					if(val == "+") { dir = 1; continue; }
					if(val == "-") { dir = -1; continue; }
					if(val.find('.') != string::npos) {
						vector<string> nodeKey = split(val,".");
						Node* refNode = getNode(nodeKey[0]);
						Variable& var = refNode->vars[ nodeKey[1] ];
						terms.push_back( Term(var,dir) );
						dir = 1;
						continue;
					}

					constant = dir*stod(val);
					dir = 1;
				}

				Expression exp(terms, constant);
				kiwi::debug::dump( Constraint(exp, OP_EQ, strength::required ) );
				solver->addConstraint( Constraint(exp, OP_EQ, strength::required ) );


			}
		}
	}

	for( Node* n : subs) {
		n->constrain( solver );
	}

}

void Node::addStay(Solver* solver, string key, float val) {
	solver->addEditVariable( vars[key], strength::strong );
	solver->suggestValue( vars[key], val );
}

int Node::atti(string key) {
	return atoi( atts[key].c_str() );
}

uint32_t Node::color(string key) {
	string val = atts[key];
	map<string,uint32_t> namedColors ={
		{ "red", 0xFF0000 },
		{ "green", 0x00FF00 },
		{ "blue", 0x0000FF },
	};
	if(namedColors.count(val)) {
		return namedColors[val];
	}
	return strtoul(val.c_str(), NULL, 16);
	/* return strtoul( atts[key].c_str(), NULL, 10 ); */
}
float Node::attf(string key) {
	if(atts.count(key)) {
		return atof( atts[key].c_str() );
	}
	return NAN;
}
Node* Node::clone() {
	Node* result = new Node();
	result->atts = this->atts;

	vector<Node*>::iterator p;
	for(p = subs.begin(); p != subs.end(); p++) {
		result->subs.push_back( *p );
	}
	return result;
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
