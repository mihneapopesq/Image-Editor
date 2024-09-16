//Copyright Popescu Mihnea-Gabriel 312CAb 2023-2024
typedef struct {
	int red;
	int green;
	int blue;
} rgb;

typedef struct {
	int row, column;
	int max;
	int x1, x2, y1, y2;
	char type[10];
	int load;
	int ascii;
	int message;
	int **mat;
	int **tmp;
	rgb **tmpr;
	rgb **RGB;
} image;
