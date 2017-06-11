#include "gd.h"


void render(gdImagePtr im, Node* node) {
	uint32_t stroke = node->color("stroke");

	gdImageRectangle( im,
			node->left(),
			node->top(),
			node->right()-1,
			node->bottom()-1,
			stroke);

	cout<<
			node->left()<<" "<<
			node->top()<<" "<<
			node->right()-1<<" "<<
			node->bottom()-1<<" "<<endl;

	vector<Node*>& subs = node->subs;
	vector<Node*>::iterator p;
	for(p = subs.begin(); p != subs.end(); p++) {
		render( im, *p );
	}


}
