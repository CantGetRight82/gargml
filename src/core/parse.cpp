#include "parse.h"

class SymbolPos {
	public:
	char symbol;
	size_t pos;
	SymbolPos( char symbol, size_t pos ) : symbol(symbol), pos(pos) {
	}
};

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

string tldrProp( char code ) {
	switch(code) {
		case 't': return "top";
		case 'l': return "left";
		case 'd': return "down";
		case 'r': return "right";
	}
	return "notldr";
}


bool tldr(Node* node, string key, string val) {
	int size = key.size();
	if(size>4) { return false; }
	for(int i=0; i<size; i++) {
		if(strchr("tldr", key[i]) == NULL) {
			return false;
		}
	}

	for(int i=0; i<size; i++) {
		string prop = tldrProp( key[i] );
		node->atts[ prop ] = val;
	}
	return true;
}

Node* parse(string str) {
	const char* buffer = str.c_str();
	size_t size = str.size();

	vector<SymbolPos> positions;
	const char* delimeters = "{}:;`";
	bool inString = false;
	int level = 0;
	for(int i=0; i<size; i++) {
		if(strchr(delimeters, buffer[i]) != NULL) {
			char c = buffer[i];
			positions.push_back( SymbolPos( c, i ) );
			if(c == '`') { inString = !inString; }
			else if(c == '{' && !inString) { level++; }
			else if(c == '}' && !inString) { level--; }
		}
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
				if(tldr(top, key, str)) {
				} else if(key.compare("size") == 0) {
					top->atts[ "width" ] = str;
					top->atts[ "height" ] = str;
				} else {
					top->atts[ key ] = str;
				}
			}
		}
	}
	return root;
}
