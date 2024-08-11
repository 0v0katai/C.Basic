//
struct TBdispFillArea {
    int x1;
    int y1;
    int x2;
    int y2;
    unsigned char mode;
};

//
struct TShapeProps{
	int dx;
	int dy;
	int wx;
	int wy;
	int color;
	TBdispFillArea saved;
};