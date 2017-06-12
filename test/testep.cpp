#include <iostream>
#include <assert.h> 
#include "../src/core/ExpressionPart.h"

using namespace std;

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
	 *
	 *
	 */

	vector<ExpressionPart> parts;

	parts = ExpressionPart::parse("100");
	assert( parts.size() == 1 );
	assert( parts[0].number() == 100 );

	parts = ExpressionPart::parse("-100");
	assert( parts.size() == 1 );
	assert( parts[0].number() == -100 );

	parts = ExpressionPart::parse("- 100");
	assert( parts.size() == 1 );
	assert( parts[0].number() == -100 );

	parts = ExpressionPart::parse(" p.top ");
	assert( parts.size() == 1 );
	assert( parts[0].var == "p.top" );
	assert( parts[0].number() == 1 );

	parts = ExpressionPart::parse("-p.top ");
	assert( parts.size() == 1 );
	assert( parts[0].var == "p.top" );
	assert( parts[0].number() == -1 );

	parts = ExpressionPart::parse("-p.top*2");
	assert( parts.size() == 1 );
	assert( parts[0].var == "p.top" );
	assert( parts[0].number() == -2 );

	parts = ExpressionPart::parse("-p.top*-3");
	assert( parts.size() == 1 );
	assert( parts[0].var == "p.top" );
	assert( parts[0].number() == 3 );

	parts = ExpressionPart::parse("p.top*2 + 100");
	assert( parts.size() == 2 );
	assert( parts[0].var == "p.top" );
	assert( parts[0].number() == 2 );
	assert( parts[1].var == "" );
	assert( parts[1].number() == 100 );
}

