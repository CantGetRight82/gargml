#include "gd.h"
#include "../core/parse.h"
#include "../core/file.h"
#include "render.h"

using namespace std;




int main(int c, const char** argv) {
	if(c<3) {
		cerr<<"Usage: gdgarg <garfile> <pngfile> [<target-width>] [<target-height>]"<<endl;
		return 1;
	}

	const char* infile = argv[1];
	const char* outfile = argv[2];

	char* buffer = newRead( infile );
	if(!buffer) {
		cerr<<"Read error, does garfile "<<argv[1]<<" exist?"<<endl;
		return 1;
	}

	Node* root = parse(buffer);
	delete buffer;

	/* root->renderJSON(cout); */


	int width = root->atti("width");
	int height = root->atti("height");

	if( min(width,height) <= 0) {
		cerr<<"Original design width and height must be specified in root"<<endl;
	}
	if( max(width,height) > 4096) {
		cerr<<"Original design width and height cannot be above 4096"<<endl;
	}
	
	gdImagePtr im;
	FILE *pngout;
	int black;
	int white;

	im = gdImageCreateTrueColor(width, height);

	
	gdImageFill(im,0,0,0xFFFFFF);

	render(im, root);


	pngout = fopen(outfile, "wb");
	gdImagePng(im, pngout);

	fclose(pngout);

	gdImageDestroy(im);
}
