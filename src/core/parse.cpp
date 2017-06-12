#include "parse.h"
#include "SymbolPos.h"

#include "../../vendor/kiwi/kiwi/kiwi.h"
#include "../../vendor/kiwi/kiwi/debug.h"


string trimBefore( const char* buffer, vector<SymbolPos>& positions, int i) {
	int end = positions[i].pos-1;
	int start = i>0 ? positions[i-1].pos+1 : 0;
	while( start < end && isspace(buffer[start]) ) { start++; }
	while( end > start && isspace(buffer[end]) ) { end--; }
	if(end>=start) {
		size_t size = end-start+1;
		return string( buffer+start, size);
	}
	return "";

}


bool trbl(Node* node, string key, string val) {
	int size = key.size();
	if(size>4) { return false; }
	for(int i=0; i<size; i++) {
		if(strchr("trbl", key[i]) == NULL) {
			return false;
		}
	}

	for(int i=0; i<size; i++) {
		switch(key[i]) {
			case 't': node->atts["top"] = val; break;
			case 'r': node->atts["right"] = val; break;
			case 'b': node->atts["bottom"] = val; break;
			case 'l': node->atts["left"] = val; break;
		}
	}
	return true;
}

Node* parse(string str) {
	const char* buffer = str.c_str();
	vector<SymbolPos> positions = SymbolPos::parse( str, "{}:;`" );

	int level = 0;
	bool inString = false;
	for(SymbolPos sp : positions) {
		char c = sp.symbol;
		if(c == '`') { inString = !inString; }
		else if(c == '{' && !inString) { level++; }
		else if(c == '}' && !inString) { level--; }

	}

	if(level != 0) {
		throw "{ and } characters do not match up";
	}



	vector<Node*> stack;
	Node* root = NULL;
	Node* top = NULL;
	string key;
	for(int i=0; i<positions.size(); i++) {
		char symbol = positions[i].symbol;

		string str = trimBefore(buffer, positions, i);
		if(symbol == '{') {
			Node* node;
			Node* clone = NULL;
			if(str.find('<') != string::npos) {
				clone = top->subs.back();
			}

			if(clone) {
				node = clone->clone();
			} else {
				node = new Node();
			}

			node->parent = top;
			if(top) {
				if(top->subs.size()) {
					node->prev = top->subs.back();
					node->prev->next = node;
				}
				top->subs.push_back( node );
			}
			stack.push_back( node );
			top = node;
			if(i==0) {
				root = node;
			}
		} else if(symbol == '}') {
			if(stack.size()) {
				stack.pop_back();
				top = stack.back();
			}
		} else if(symbol == ':') {
			key = str;
		} else if(symbol == ';') {
			if(key.size() && str.size()) {
				if(trbl(top, key, str)) {
				} else if(key.compare("size") == 0) {
					top->atts[ "width" ] = str;
					top->atts[ "height" ] = str;
				} else {
					top->atts[ key ] = str;
				}
			}
		}
	}
	try {

		kiwi::Solver* solver = new kiwi::Solver();
		root->constrain( solver ); 
		solver->updateVariables();
	} catch(kiwi::UnsatisfiableConstraint e) {
		cout<<e.what()<<endl;
		kiwi::debug::dump( e.constraint() );
	} catch(const char* e) {
		cout<<e<<endl;
	}
	return root;
}
