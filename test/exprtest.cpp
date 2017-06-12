
#include <iostream>
#include <sstream>

#include "../src/core/ConstraintParser.h"

using namespace std;
using namespace kiwi;

	string toString( const Constraint& cn ) {
		stringstream out;

		typedef std::vector<Term>::const_iterator iter_t;

		iter_t begin = cn.expression().terms().begin();
		iter_t end = cn.expression().terms().end();
		for( iter_t it = begin; it != end; ++it )
		{
			out << it->coefficient() << " * ";
			out << it->variable().name() << " + ";
		}
		out << cn.expression().constant();
		switch( cn.op() )
		{
			case OP_LE:
				out << " <= 0 ";
				break;
			case OP_GE:
				out << " >= 0 ";
				break;
			case OP_EQ:
				out << " == 0 ";
				break;
			default:
				break;
		}
		out << " | strength = " << cn.strength();
		return out.str();
	}

int main() {

	map<string,string> io = {
		{ "< prev.left*2 + 100", "100 == 0  | strength = 1.001e+09" }
	};
	for(auto it = io.begin(); it != io.end(); ++it ) {
		string in = it->first;
		string out = it->second;

		Constraint result = ConstraintParser::parse(in);
		string str = toString(result);
		/*
		if(str != out) {
			cout<<"FAIL"<<endl<<endl
				<<in<<endl
				<<"should be"<<endl
				<<"'"<<out<<"'"<<endl
				<<"but is"<<endl
				<<"'"<<str<<"'"<<endl;
			return 1;

		} else {
			cout<<"SUCCES: "<<in<<endl;
		}
		*/
		
	}
	return 0;
}
