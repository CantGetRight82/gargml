
#include <iostream>
#include <sstream>

#include <assert.h> 
#include "../src/core/ExpressionPart.h"
#include "../vendor/kiwi/kiwi/kiwi.h"

using namespace std;
using namespace kiwi;

void testSolutions();
map<string,double> solution(string statements, bool trace);

int main() {
	/*
	 * supported:
	 *
	 * constant: 100
	 * negative constant: -100
	 * var: p.top
	 * var with factor: p.top*2
	 * negative var: -p.top (becomes pos var with negated factor: p.top*-1)
	 * negative var with negative factor: -p.top*-2 (becomes p.top*2)
	 * var combined with constant: p.top*2 + 100
	 */
	map<string,double> vars;

	vars = solution("a: 100; b: a", 1);
	assert( vars["a"] == 100 );
	assert( vars["b"] == 100 );

	vars = solution("a: 100; b: a-20", 1);
	assert( vars["a"] == 100 );
	assert( vars["b"] == 80 );

	vars = solution("a: 100; b: a*2-20", 1);
	assert( vars["a"] == 100 );
	assert( vars["b"] == 180 );
	return 0;
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


string trimSpace(string str) {
	return str.substr( str.find_first_not_of(' ') );
}

Variable getVar( map<string,Variable>& vars, string key) {
	if(vars.count(key) == 0) {
		vars[key] = Variable(key);
	}
	return vars[key];
}

map<string,double> solution(string statements, bool trace) {
	vector<string> lines = split(statements, ";");
	map<string,Variable> vars;

	Solver solver;
	for(string line : lines) {
		//trim space before

		int colon = line.find_first_of(':');
		string fromKey = trimSpace(line.substr(0,colon));
		string expression = trimSpace( line.substr(colon+1) );

		Variable from = getVar(vars, fromKey );
		vector<ExpressionPart> parts = ExpressionPart::parse( expression );

		if(parts.size() == 1 && parts[0].var == "") {
			solver.addEditVariable( from, strength::strong );
			solver.suggestValue( from, parts[0].number() );
		} else {
			RelationalOperator op = OP_EQ;
			double strength = strength::required;

			double constant = 0;
			vector<Term> terms;
			terms.push_back( Term(from, -1) );
			for(int i=0; i<parts.size(); i++) {
				if(parts[i].var.size()) {
					switch(parts[i].var[0]) {
						case '<': op = OP_LE; break;
						case '>': op = OP_GE; break;
						default:
						terms.push_back( Term( getVar(vars,parts[i].var), parts[i].number() ) );
						break;
					}
				} else {
					constant = parts[i].number();
				}
			}

			Constraint c = Constraint( Expression(terms,constant), op, strength);
			kiwi::debug::dump(c);
			solver.addConstraint(c);
		}

	}

	solver.updateVariables();

	map<string, double> result;
	for(auto& it : vars) {
		result[ it.first ] = it.second.value();
		if(trace) {
			cout<<it.first<<" : "<<it.second.value()<<endl;
		}
	}

	return result;
}

