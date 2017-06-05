#include "gd.h"

void render(gdImagePtr im, Node* node) {


	gdImageRectangle( im,
			node->left(),
			node->top(),
			node->right()-1,
			node->down()-1,
			0);


	vector<Node*>& subs = node->subs;
	vector<Node*>::iterator p;
	for(p = subs.begin(); p != subs.end(); p++) {
		render( im, *p );
	}


}
