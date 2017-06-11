#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include "../../vendor/kiwi/kiwi/kiwi.h"

using namespace std;

class Node {
	public:
		static Node* zero;
		Node* parent;
		Node* prev;
		Node* next;

		map<string,string> atts;
		vector<Node*> subs;

		map<string, kiwi::Variable> vars;

		void renderJSON(std::ostream& to);
		int atti(string key);
		uint32_t color(string key);
		float attf(string key);

		string ids() {
			if(parent) {
				return parent->ids() + "." + std::to_string(parent->idx(this));
			}
			return "0";
		}

		int idx(Node* child) {
			for(int i=0; i<subs.size(); i++) {
				if(child == subs[i]) { return i; }
			}
			return -1;
		}

		Node() : parent(NULL), prev(NULL), next(NULL) {
			initVars();
		}
		void initVars();

		float top() { return vars["top"].value(); }
		float right() { return vars["right"].value(); }
		float bottom() { return vars["bottom"].value(); }
		float left() { return vars["left"].value(); }

		vector<string> attsplit(string key, string delims) {
			return split( atts[key], delims);
		}

		Node* getNode(string name) {
			if(name == "p") { return parent; }
			if(name == "prev") { return prev; }
			if(name == "next") { return next; }
			return this;
		}

		vector<string> split(string val, string delims) {
			vector<string> result;
			size_t current;
			size_t next = -1;
			do {
				current = next + 1;
				next = val.find_first_of( delims, current );
				result.push_back( val.substr( current, next - current ) );
			}
			while (next != string::npos);
			return result;
		}



		void constrain(kiwi::Solver* solver);
		void addStay(kiwi::Solver* solver, string key, float val);

		Node* clone();
};
