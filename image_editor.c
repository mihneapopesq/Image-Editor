//Copyright Popescu Mihnea-Gabriel 312CAb 2023-2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "struct.h"

//Alocare memorie pentru o matrice rgb
rgb **alloc_matrix_rgb(int n, int m)
{
	rgb **a;
	a = malloc(n * sizeof(rgb *));
	if (!a) {
		fprintf(stderr, "Eroare la alocarea memoriei.\n");
		exit(-1);
	}
	for (int i = 0; i < n; i++) {
		a[i] = malloc(m * sizeof(rgb));
		if (!a[i]) {
			fprintf(stderr, "Eroare la alocarea memoriei.\n");
			exit(-1);
			for (int j = i - 1; j >= 0; j--)
				free(a[j]);
			free(a);
			exit(-1);
		}
	}
	return a;
}

//Alocare memorie pentru o matrice de tip grayscale
int **alloc_matrix_gray(int n, int m)
{
	int **a;
	a = malloc(n * sizeof(int *));
	if (!a) {
		fprintf(stderr, "Eroare la alocarea memoriei.\n");
		exit(-1);
	}
	for (int i = 0; i < n; i++) {
		a[i] = malloc(m * sizeof(int));
		if (!a[i]) {
			fprintf(stderr, "Eroare la alocarea memoriei.\n");
			exit(-1);
			for (int j = i - 1; j >= 0; j--)
				free(a[j]);
			free(a);
			exit(-1);
		}
	}
	return a;
}

// LOAD ul GRAY pentru P2 si P5
void LOAD_GRAY(image *img, char type[], FILE *f)
{
	strcpy(img->type, type);
	if (strcmp(type, "P2") == 0) {
		fscanf(f, "%d%d%d", &img->column, &img->row, &img->max);
		img->mat = alloc_matrix_gray(img->row, img->column);
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++)
				fscanf(f, "%d", &img->mat[i][j]);
		}
	}
	if (strcmp(type, "P5") == 0) {
		fscanf(f, "%d%d%d", &img->column, &img->row, &img->max);
		fseek(f, 1, SEEK_CUR);
		img->mat = alloc_matrix_gray(img->row, img->column);
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++) {
				unsigned char value;
				fread(&value, sizeof(unsigned char), 1, f);
				img->mat[i][j] = (int)value;
			}
		}
	}
	img->x1 = 0;
	img->y1 = 0;
	img->x2 = img->column;
	img->y2 = img->row;
	fclose(f);
}

// LOAD ul RGB pentru P3 si P6
void LOAD_RGB(image *img, char type[], FILE *f)
{
	strcpy(img->type, type);
	if (strcmp(type, "P3") == 0) {
		fscanf(f, "%d%d%d", &img->column, &img->row, &img->max);
		img->RGB = alloc_matrix_rgb(img->row, img->column);
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++)
				fscanf(f, "%d%d%d", &img->RGB[i][j].red, &img->RGB[i][j].green,
					   &img->RGB[i][j].blue);
		}
	}
	if (strcmp(type, "P6") == 0) {
		fscanf(f, "%d%d%d", &img->column, &img->row, &img->max);
		img->RGB = alloc_matrix_rgb(img->row, img->column);
		// sarim cate un octet pentru a putea citi corect
		fseek(f, 1, SEEK_CUR);
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++) {
				unsigned char value;
				fread(&value, sizeof(unsigned char), 1, f);
				img->RGB[i][j].red = (unsigned int)value;
				fread(&value, sizeof(char), 1, f);
				img->RGB[i][j].green = (unsigned int)value;
				fread(&value, sizeof(char), 1, f);
				img->RGB[i][j].blue = (unsigned int)value;
			}
		}
	}
	img->x1 = 0;
	img->y1 = 0;
	img->x2 = img->column;
	img->y2 = img->row;
	fclose(f);
}

void LOAD(image *img, char filename[])
{
	if (img->mat) {
		for (int i = 0; i < img->row; i++)
			free(img->mat[i]);
		free(img->mat);
		img->mat = NULL;
		img->load = 0;
	}
	if (img->RGB) {
		for (int i = 0; i < img->row; i++)
			free(img->RGB[i]);
		free(img->RGB);
		img->RGB = NULL;
		img->load = 0;
	}
	FILE *f = fopen(filename, "r");
	if (!f) {
		printf("Failed to load %s\n", filename);
		return;
	}
	char type[5];
	img->mat = NULL;
	img->RGB = NULL;
	fgets(type, 5, f);
	type[2] = '\0';
	if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0)
		LOAD_GRAY(img, type, f);
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
		LOAD_RGB(img, type, f);
	img->load = 1;
	printf("Loaded %s\n", filename);
}

void SELECT(image *img, int x1, int x2, int y1, int y2)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (x1 == x2 || y1 == y2 || x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 ||
	    x1 > img->column || x2 > img->column || y1 > img->row ||
		y2 > img->row) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 > x2) {
		int aux = x2;
		x2 = x1;
		x1 = aux;
	}
	if (y1 > y2) {
		int aux = y2;
		y2 = y1;
		y1 = aux;
	}
	img->x1 = x1;
	img->x2 = x2;
	img->y1 = y1;
	img->y2 = y2;
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

void SELECT_ALL(image *img)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	img->x1 = 0;
	img->y1 = 0;
	img->x2 = img->column;
	img->y2 = img->row;
	printf("Selected ALL\n");
}

void SAVE_BINARY(image *img, char filename[])
{
	FILE *f = fopen(filename, "wb");
	if (strcmp(img->type, "P5") == 0 || strcmp(img->type, "P2") == 0)
		fprintf(f, "P5\n");
	if (strcmp(img->type, "P6") == 0 || strcmp(img->type, "P3") == 0)
		fprintf(f, "P6\n");
	fprintf(f, "%d %d\n%d\n", img->column, img->row, img->max);
	if (strcmp(img->type, "P5") == 0 || strcmp(img->type, "P2") == 0) {
		for (int i = 0; i < img->row; i++)
			for (int j = 0; j < img->column; j++)
				fwrite(&img->mat[i][j], sizeof(char), 1, f);
	} else if (strcmp(img->type, "P6") == 0 || strcmp(img->type, "P3") == 0) {
		for (int i = 0; i < img->row; i++)
			for (int j = 0; j < img->column; j++) {
				fwrite(&img->RGB[i][j].red, sizeof(char), 1, f);
				fwrite(&img->RGB[i][j].green, sizeof(char), 1, f);
				fwrite(&img->RGB[i][j].blue, sizeof(char), 1, f);
			}
	}
	fclose(f);
}

void SAVE_ASCII(image *img, char filename[])
{
	FILE *f = fopen(filename, "wt");
	if (strcmp(img->type, "P5") == 0 || strcmp(img->type, "P2") == 0)
		fprintf(f, "P2\n");
	if (strcmp(img->type, "P6") == 0 || strcmp(img->type, "P3") == 0)
		fprintf(f, "P3\n");
	fprintf(f, "%d %d\n%d\n", img->column, img->row, img->max);
	if (strcmp(img->type, "P5") == 0 || strcmp(img->type, "P2") == 0) {
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++)
				fprintf(f, "%d ", img->mat[i][j]);
			fprintf(f, "\n");
		}
	} else if (strcmp(img->type, "P6") == 0 || strcmp(img->type, "P3") == 0) {
		for (int i = 0; i < img->row; i++) {
			for (int j = 0; j < img->column; j++)
				fprintf(f, "%d %d %d ", img->RGB[i][j].red,
						img->RGB[i][j].green, img->RGB[i][j].blue);
			fprintf(f, "\n");
		}
	}
	fclose(f);
}

void SAVE(image *img, char filename[])
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->ascii == 0)
		SAVE_ASCII(img, filename);
	else
		SAVE_BINARY(img, filename);
	printf("Saved %s\n", filename);
}

void CROP(image *img)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		img->tmp = alloc_matrix_gray(img->y2 - img->y1, img->x2 - img->x1);
		for (int i = img->y1; i < img->y2; i++) {
			for (int j = img->x1; j < img->x2; j++)
				img->tmp[i - img->y1][j - img->x1] = img->mat[i][j];
		}
		if (img->mat) {
			for (int i = 0; i < img->row; i++)
				free(img->mat[i]);
			free(img->mat);
		}
		img->mat = alloc_matrix_gray(img->y2 - img->y1, img->x2 - img->x1);
		for (int i = 0; i < img->y2 - img->y1; ++i)
			for (int j = 0; j < img->x2 - img->x1; ++j)
				img->mat[i][j] = img->tmp[i][j];
	}
	if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0) {
		img->tmpr = alloc_matrix_rgb(img->y2 - img->y1, img->x2 - img->x1);
		for (int i = img->y1; i < img->y2; i++) {
			for (int j = img->x1; j < img->x2; j++)
				img->tmpr[i - img->y1][j - img->x1] = img->RGB[i][j];
		}
		if (img->RGB) {
			for (int i = 0; i < img->row; i++)
				free(img->RGB[i]);
			free(img->RGB);
		}
		img->RGB = alloc_matrix_rgb(img->y2 - img->y1, img->x2 - img->x1);
		for (int i = 0; i < img->y2 - img->y1; ++i)
			for (int j = 0; j < img->x2 - img->x1; ++j)
				img->RGB[i][j] = img->tmpr[i][j];
	}
	img->row = img->y2 - img->y1;
	img->column = img->x2 - img->x1;
	img->x1 = 0;
	img->y1 = 0;
	img->x2 = img->column;
	img->y2 = img->row;
	img->load = 1;
	printf("Image cropped\n");
}

void clamp_max(int *n, int maxx)
{
	if ((*n) > maxx)
		(*n) = maxx;
}

void clamp_min(int *n, int minn)
{
	if ((*n) < minn)
		(*n) = minn;
}

void EDGE(image *img, int *okapply, int *error)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		(*error) = 1;
		return;
	}
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		printf("Easy,Charlie Chaplin\n");
		img->message = 1;
		return;
	}
	int edge[3][3] = {{-1, -1, -1}, {-1, 8, -1,}, {-1, -1, -1}};
	rgb **aux = alloc_matrix_rgb(img->row, img->column);
	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++) {
			aux[i][j].red = img->RGB[i][j].red;
			aux[i][j].green = img->RGB[i][j].green;
			aux[i][j].blue = img->RGB[i][j].blue;
		}
	}
	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (!(i > 0 && j > 0 && j < img->column - 1 && i < img->row - 1))
				continue; // evit marginile
			aux[i][j].red =
				img->RGB[i - 1][j - 1].red * edge[0][0] +
				img->RGB[i - 1][j].red * edge[0][1] +
				img->RGB[i - 1][j + 1].red * edge[0][2] +
				img->RGB[i][j - 1].red * edge[1][0] +
				img->RGB[i][j + 1].red * edge[1][2] +
				img->RGB[i + 1][j - 1].red * edge[2][0] +
				img->RGB[i + 1][j].red * edge[2][1] +
				img->RGB[i + 1][j + 1].red * edge[2][2] +
				img->RGB[i][j].red * edge[1][1];
			aux[i][j].green =
				img->RGB[i - 1][j - 1].green * edge[0][0] +
				img->RGB[i - 1][j].green * edge[0][1] +
				img->RGB[i - 1][j + 1].green * edge[0][2] +
				img->RGB[i][j - 1].green * edge[1][0] +
				img->RGB[i][j].green * edge[1][1] +
				img->RGB[i][j + 1].green * edge[1][2] +
				img->RGB[i + 1][j - 1].green * edge[2][0] +
				img->RGB[i + 1][j].green * edge[2][1] +
				img->RGB[i + 1][j + 1].green * edge[2][2];
			aux[i][j].blue =
				img->RGB[i - 1][j - 1].blue * edge[0][0] +
				img->RGB[i - 1][j].blue * edge[0][1] +
				img->RGB[i - 1][j + 1].blue * edge[0][2] +
				img->RGB[i][j - 1].blue * edge[1][0] +
				img->RGB[i][j].blue * edge[1][1] +
				img->RGB[i][j + 1].blue * edge[1][2] +
				img->RGB[i + 1][j - 1].blue * edge[2][0] +
				img->RGB[i + 1][j].blue * edge[2][1] +
				img->RGB[i + 1][j + 1].blue * edge[2][2];
				clamp_max(&aux[i][j].red, 255);
				clamp_min(&aux[i][j].red, 0);
				clamp_max(&aux[i][j].green, 255);
				clamp_min(&aux[i][j].green, 0);
				clamp_max(&aux[i][j].blue, 255);
				clamp_min(&aux[i][j].blue, 0);
		}
	}
	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			img->RGB[i][j].red = aux[i][j].red;
			img->RGB[i][j].green = aux[i][j].green;
			img->RGB[i][j].blue = aux[i][j].blue;
		}
	}
	for (int i = 0; i < img->row; i++)
		free(aux[i]);
	free(aux);
	(*okapply) = 1;
	printf("APPLY EDGE done\n");
}

void SHARPEN(image *img, int *okapply, int *error)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		(*error) = 1;
		return;
	}
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		printf("Easy,Charlie Chaplin\n");
		img->message = 1;
		return;
	}
	int sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
	rgb **aux = alloc_matrix_rgb(img->row, img->column);
	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++) {
			aux[i][j].red = img->RGB[i][j].red;
			aux[i][j].green = img->RGB[i][j].green;
			aux[i][j].blue = img->RGB[i][j].blue;
		}
	}
	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (!(i > 0 && j > 0 && j < img->column - 1 && i < img->row - 1))
				continue; // evit marginile
			aux[i][j].red =
				img->RGB[i - 1][j - 1].red * sharpen[0][0] +
				img->RGB[i - 1][j].red * sharpen[0][1] +
				img->RGB[i - 1][j + 1].red * sharpen[0][2] +
				img->RGB[i][j - 1].red * sharpen[1][0] +
				img->RGB[i][j + 1].red * sharpen[1][2] +
				img->RGB[i + 1][j - 1].red * sharpen[2][0] +
				img->RGB[i + 1][j].red * sharpen[2][1] +
				img->RGB[i + 1][j + 1].red * sharpen[2][2] +
				img->RGB[i][j].red * sharpen[1][1];
			aux[i][j].green =
				img->RGB[i - 1][j - 1].green * sharpen[0][0] +
				img->RGB[i - 1][j].green * sharpen[0][1] +
				img->RGB[i - 1][j + 1].green * sharpen[0][2] +
				img->RGB[i][j - 1].green * sharpen[1][0] +
				img->RGB[i][j].green * sharpen[1][1] +
				img->RGB[i][j + 1].green * sharpen[1][2] +
				img->RGB[i + 1][j - 1].green * sharpen[2][0] +
				img->RGB[i + 1][j].green * sharpen[2][1] +
				img->RGB[i + 1][j + 1].green * sharpen[2][2];
			aux[i][j].blue =
				img->RGB[i - 1][j - 1].blue * sharpen[0][0] +
				img->RGB[i - 1][j].blue * sharpen[0][1] +
				img->RGB[i - 1][j + 1].blue * sharpen[0][2] +
				img->RGB[i][j - 1].blue * sharpen[1][0] +
				img->RGB[i][j].blue * sharpen[1][1] +
				img->RGB[i][j + 1].blue * sharpen[1][2] +
				img->RGB[i + 1][j - 1].blue * sharpen[2][0] +
				img->RGB[i + 1][j].blue * sharpen[2][1] +
				img->RGB[i + 1][j + 1].blue * sharpen[2][2];
			clamp_max(&aux[i][j].red, 255);
			clamp_min(&aux[i][j].red, 0);
			clamp_max(&aux[i][j].green, 255);
			clamp_min(&aux[i][j].green, 0);
			clamp_max(&aux[i][j].blue, 255);
			clamp_min(&aux[i][j].blue, 0);
		}
	}
	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++) {
			img->RGB[i][j].red = aux[i][j].red;
			img->RGB[i][j].green = aux[i][j].green;
			img->RGB[i][j].blue = aux[i][j].blue;
		}
	for (int i = 0; i < img->row; i++)
		free(aux[i]);
	free(aux);
	(*okapply) = 1;
	printf("APPLY SHARPEN done\n");
}

void APPLY_FILTER(image *img, int filter[][3], double div)
{
	double rsum = 0, gsum = 0, bsum = 0;
	rgb **aux = alloc_matrix_rgb(img->row, img->column);

	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++) {
			aux[i][j].red = img->RGB[i][j].red;
			aux[i][j].green = img->RGB[i][j].green;
			aux[i][j].blue = img->RGB[i][j].blue;
		}
	}
	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (!(i > 0 && j > 0 && j < img->column - 1 && i < img->row - 1))
				continue; // evit marginile
			rsum =
				img->RGB[i - 1][j - 1].red * filter[0][0] +
				img->RGB[i - 1][j].red * filter[0][1] +
				img->RGB[i - 1][j + 1].red * filter[0][2] +
				img->RGB[i][j - 1].red * filter[1][0] +
				img->RGB[i][j + 1].red * filter[1][2] +
				img->RGB[i + 1][j - 1].red * filter[2][0] +
				img->RGB[i + 1][j].red * filter[2][1] +
				img->RGB[i + 1][j + 1].red * filter[2][2] +
				img->RGB[i][j].red * filter[1][1];
			gsum =
				img->RGB[i - 1][j - 1].green * filter[0][0] +
				img->RGB[i - 1][j].green * filter[0][1] +
				img->RGB[i - 1][j + 1].green * filter[0][2] +
				img->RGB[i][j - 1].green * filter[1][0] +
				img->RGB[i][j].green * filter[1][1] +
				img->RGB[i][j + 1].green * filter[1][2] +
				img->RGB[i + 1][j - 1].green * filter[2][0] +
				img->RGB[i + 1][j].green * filter[2][1] +
				img->RGB[i + 1][j + 1].green * filter[2][2];
			bsum =
				img->RGB[i - 1][j - 1].blue * filter[0][0] +
				img->RGB[i - 1][j].blue * filter[0][1] +
				img->RGB[i - 1][j + 1].blue * filter[0][2] +
				img->RGB[i][j - 1].blue * filter[1][0] +
				img->RGB[i][j].blue * filter[1][1] +
				img->RGB[i][j + 1].blue * filter[1][2] +
				img->RGB[i + 1][j - 1].blue * filter[2][0] +
				img->RGB[i + 1][j].blue * filter[2][1] +
				img->RGB[i + 1][j + 1].blue * filter[2][2];
			aux[i][j].red = round(rsum / div);
			aux[i][j].green = round(gsum / div);
			aux[i][j].blue = round(bsum / div);
			clamp_max(&aux[i][j].red, 255);
			clamp_min(&aux[i][j].red, 0);
			clamp_max(&aux[i][j].green, 255);
			clamp_min(&aux[i][j].green, 0);
			clamp_max(&aux[i][j].blue, 255);
			clamp_min(&aux[i][j].blue, 0);
		}
	}
	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++) {
			if (!(i > 0 && j > 0 && j < img->column - 1 && i < img->row - 1))
				continue;// evit marginile
			img->RGB[i][j].red = aux[i][j].red;
			img->RGB[i][j].green = aux[i][j].green;
			img->RGB[i][j].blue = aux[i][j].blue;
		}
	for (int i = 0; i < img->row; i++)
		free(aux[i]);
	free(aux);
}

void BLUR(image *img, int *okapply, int *error)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		(*error) = 1;
		return;
	}
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		printf("Easy,Charlie Chaplin\n");
		img->message = 1;
		return;
	}
	int blur[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
	(*okapply) = 1;
	APPLY_FILTER(img, blur, 9.0);
	printf("APPLY BLUR done\n");
}

void GAUSSIAN_BLUR(image *img, int *okapply, int *error)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		(*error) = 1;
		return;
	}
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		printf("Easy,Charlie Chaplin\n");
		img->message = 1;
		return;
	}
	int gaussian[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	(*okapply) = 1;
	APPLY_FILTER(img, gaussian, 16.0);
	printf("APPLY GAUSSIAN_BLUR done\n");
}

void EXIT(image *img)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		free(img);
		return;
	}
	if (!img->mat && !img->RGB) {
		printf("No image loaded\n");
		free(img);
		return;
	}
	if (img->mat) {
		for (int i = 0; i < img->row; i++)
			free(img->mat[i]);
		free(img->mat);
	}
	if (img->RGB) {
		for (int i = 0; i < img->row; i++)
			free(img->RGB[i]);
		free(img->RGB);
	}
	free(img);
}

void HISTOGRAM(image *img, int x, int y)
{
	if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0) {
		printf("Black and white image needed\n");
		return;
	}
	if (y < 0 || y > 256) {
		printf("Invalid set of parameters\n");
		return;
	}
	int cy = y;
	while (cy % 2 == 0 && cy > 0)
		cy = cy / 2;
	if (cy == 0) {
		printf("Invalid set of parameters\n");
		return;
	}
	// voi apela calloc pentru a putea seta elementele la 0
	int *frecv = calloc(256, sizeof(int));
	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++)
			frecv[img->mat[i][j]]++;
	}
	int max = 0;
	int p = 0;
	// voi apela calloc pentru a putea seta elementele la 0
	int *histogram = calloc(y, sizeof(int));
	int divide = 256 / y; // number of bins
	for (int i = 0; i < y; i++) {
		for (int j = p; j < (p + divide); j++)
			histogram[i] = histogram[i] + frecv[j];
		if (histogram[i] > max)
			max = histogram[i];
		p = p + divide;
	}
	for (int i = 0; i < y; i++) {
		int ans;
		ans = (int)(1.0 * histogram[i] * x / max);
		printf("%d  |   ", ans);
		for (int j = 0; j < ans; j++)
			printf("*");
		printf("\n");
	}
	free(histogram);
	free(frecv);
}

void EQUALIZE(image *img)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0) {
		printf("Black and white image needed\n");
		return;
	}
	int *frecv = calloc(256, sizeof(int));
	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++)
			frecv[img->mat[i][j]]++;
	}
	for (int i = 0; i < img->row; i++) {
		for (int j = 0; j < img->column; j++) {
			int s = 0;
			for (int k = 0; k <= img->mat[i][j]; k++)
				s = s + frecv[k];
			img->mat[i][j] = (int)round((double)255 * s /
										(img->row * img->column));
		}
	}
	free(frecv);
	printf("Equalize done\n");
}

void ROTATE_gray(image *img)
{
	int **aux = alloc_matrix_gray(img->column, img->row);
	// punem valorile in aux dar rotite
	for (int i = 0; i < img->row; i++)
		for (int j = 0; j < img->column; j++)
			aux[j][img->row - i - 1] = img->mat[i][j];
	for (int i = 0; i < img->row; i++)
		free(img->mat[i]);
	free(img->mat);
	img->mat = alloc_matrix_gray(img->column, img->row);
	for (int i = 0; i < img->column; i++)
		for (int j = 0; j < img->row; j++)
			img->mat[i][j] = aux[i][j];
	for (int i = 0; i < img->column; i++)
		free(aux[i]);
	free(aux);
}

void ROTATE_rgb(image *img)
{
	rgb **aux = alloc_matrix_rgb(img->column, img->row);
	// punem valorile in aux dar rotite
	for (int i = 0; i < img->row; i++)
		for (int j = 0; j < img->column; j++)
			aux[j][img->row - i - 1] = img->RGB[i][j];
	for (int i = 0; i < img->row; i++)
		free(img->RGB[i]);
	free(img->RGB);
	img->RGB = alloc_matrix_rgb(img->column, img->row);
	for (int i = 0; i < img->column; i++)
		for (int j = 0; j < img->row; j++)
			img->RGB[i][j] = aux[i][j];
	for (int i = 0; i < img->column; i++)
		free(aux[i]);
	free(aux);
}

void ROTATE_FULL(image *img)
{
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0)
		ROTATE_gray(img);
	if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0)
		ROTATE_rgb(img);
	//actualizam dimensiunile
	int aux = img->row;
	img->row = img->column;
	img->column = aux;
	//actualizam selectia
	img->x1 = 0;
	img->x2 = img->column;
	img->y1 = 0;
	img->y2 = img->row;
}

void ROTATE_gray_selected(image *img)
{
	int **aux = alloc_matrix_gray(img->y2 - img->y1, img->x2 - img->x1);
	// punem valorile in aux dar rotite
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			aux[i][j] = img->mat[i + img->y1][j + img->x1];
	int **aux2 = alloc_matrix_gray(img->y2 - img->y1, img->x2 - img->x1);
	// rotim auxul 90 de grade si punem datele in aux2
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			aux2[j][(img->y2 - img->y1) - i - 1] = aux[i][j];
	// copiem valorile rotite in matricea originala
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			img->mat[i + img->y1][j + img->x1] = aux2[i][j];
	for (int i = 0; i < (img->y2 - img->y1); i++)
		free(aux[i]);
	free(aux);
	for (int i = 0; i < (img->y2 - img->y1); i++)
		free(aux2[i]);
	free(aux2);
}

void ROTATE_rgb_selected(image *img)
{
	rgb **aux = malloc((img->y2 - img->y1) * sizeof(rgb *));
	for (int i = 0; i < (img->y2 - img->y1); i++)
		aux[i] = malloc((img->x2 - img->x1) * sizeof(rgb));
	// punem valorile in aux dar rotite
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			aux[i][j] = img->RGB[i + img->y1][j + img->x1];
	rgb **rotated_aux = malloc((img->y2 - img->y1) * sizeof(rgb *));
	for (int i = 0; i < (img->y2 - img->y1); i++)
		rotated_aux[i] = malloc((img->x2 - img->x1) * sizeof(rgb));
	// rotim auxul 90 de grade si punem datele in aux2
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			rotated_aux[j][(img->y2 - img->y1) - i - 1] = aux[i][j];
	// copiem valorile rotite in matricea originala
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			img->RGB[i + img->y1][j + img->x1] = rotated_aux[i][j];
	for (int i = 0; i < (img->y2 - img->y1); i++)
		free(aux[i]);
	free(aux);
	for (int i = 0; i < (img->y2 - img->y1); i++)
		free(rotated_aux[i]);
	free(rotated_aux);
}

void ROTATE_SELECTED(image *img)
{
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0)
		ROTATE_gray_selected(img);
	if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0)
		ROTATE_rgb_selected(img);
}

int verify_angle(int angle)
{
	if (angle == 90 || angle == -90 || angle == 180 || angle == -180 ||
		angle == 270 || angle == -270 || angle == 360 || angle == -360 ||
		angle == 0)
		return 1;
	return 0;
}

void ROTATE(image *img, int angle)
{
	if (img->load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (verify_angle(angle) == 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (((img->x2 - img->x1) != (img->y2 - img->y1)) && img->x1 != 0 &&
		img->x2 != img->column && img->y1 != 0 && img->y2 != img->row) {
		printf("The selection must be square\n");
		return;
	}
	if (verify_angle(angle) == 1) {
		if (angle == 90 || angle == -270) {
			if (img->x1 == 0 && img->x2 == img->column && img->y1 == 0 &&
				img->y2 == img->row)
				ROTATE_FULL(img);
			else
				ROTATE_SELECTED(img);
		}
		if (angle == 180 || angle == -180) {
			// daca unghiul e de 180 de grade o rotim de 2 ori la dreapta
			if (img->x1 == 0 && img->x2 == img->column && img->y1 == 0 &&
				img->y2 == img->row) {
				ROTATE_FULL(img);
				ROTATE_FULL(img);
			} else {
				ROTATE_SELECTED(img);
				ROTATE_SELECTED(img);
			}
		}
		if (angle == -90 || angle == 270) {
			// daca unghiul e de 270 o rotim de 3 ori la dreapta
			if (img->x1 == 0 && img->x2 == img->column && img->y1 == 0 &&
				img->y2 == img->row) {
				ROTATE_FULL(img);
				ROTATE_FULL(img);
				ROTATE_FULL(img);
			} else {
				ROTATE_SELECTED(img);
				ROTATE_SELECTED(img);
				ROTATE_SELECTED(img);
			}
		}
		printf("Rotated %d\n", angle);
	}
}

void FILTER(image *img, char line[], int *okapply, int *error)
{
	if (strcmp(line, "EDGE") == 0)
		EDGE(img, &(*okapply), &(*error));
	if (strcmp(line, "SHARPEN") == 0)
		SHARPEN(img, &(*okapply), &(*error));
	if (strcmp(line, "BLUR") == 0)
		BLUR(img, &(*okapply), &(*error));
	if (strcmp(line, "GAUSSIAN_BLUR") == 0)
		GAUSSIAN_BLUR(img, &(*okapply), &(*error));
}

void verify_invalid_parameter(image *img, int okapply, int error)
{
	if (okapply == 0) {
		if (img->message == 0 && error == 0)
			printf("APPLY parameter invalid\n");
		img->message = 0;
	}
}

void verify_ascii_save(image *img, char ascii[], char command[])
{
	if (strcmp(ascii, "ascii") == 0) {
		img->ascii = 0;
	} else {
		img->ascii = 1;
		strcpy(command, ascii);
	}
}

void get_word_after_apply(char line[], int *ind)
{
	char c;
	do {
		scanf("%c", &c);
		if (c != ' ')
			line[(*ind)++] = c;
	} while (c != '\n');
}

int main(void)
{
	char command[15], filename[100], ascii[10], t, line[100];
	image *a = malloc(1 * sizeof(image));
	int x1 = 0, x2, y1 = 0, y2, okapply = 0, error = 0, x, y, z, angle;
	a->ascii = 0; a->load = 0;
	scanf("%s", command);
	while (strcmp("EXIT", command) != 0) {
		if (strcmp("LOAD", command) == 0) {
			scanf("%s", filename); LOAD(a, filename);
		} else if (strcmp("SELECT", command) == 0) {
			scanf("%s", command);
			if (strcmp(command, "ALL") == 0) {
				SELECT_ALL(a);
			} else {
				x1 = atoi(command);
				fgets(line, 1000, stdin);
				if (sscanf(line, "%d%d%d", &y1, &x2, &y2) == 3)
					SELECT(a, x1, x2, y1, y2);
				else
					printf("Invalid command\n");
			}
		} else if (strcmp("HISTOGRAM", command) == 0) {
			fgets(line, 1000, stdin);
			if (a->load == 0) {
				printf("No image loaded\n");
			} else {
				int numberofel = sscanf(line, "%d%d%d", &x, &y, &z);
				if (numberofel == 2)
					HISTOGRAM(a, x, y);
				else
					printf("Invalid command\n");
			}
		} else if (strcmp("EQUALIZE", command) == 0) {
			EQUALIZE(a);
		} else if (strcmp("ROTATE", command) == 0) {
			scanf("%d", &angle);
			ROTATE(a, angle);
		} else if (strcmp("CROP", command) == 0) {
			CROP(a);
		} else if (strcmp("APPLY", command) == 0) {
			char line[100] = {'\0'};
			int ind = 0;
			get_word_after_apply(line, &ind);
			if (line[0] == '\0') {
				printf("Invalid command\n");
			} else {
				if (ind > 1)
					line[ind - 1] = '\0';
				if (a->load == 0 && strcmp(line, "\n") == 0) {
					printf("No image loaded\n");
				} else {
					if (a->load == 1 && strcmp(line, "\n") == 0) {
						printf("Invalid command\n");
					} else {
						okapply = 0; error = 0;
						FILTER(a, line, &okapply, &error);
						verify_invalid_parameter(a, okapply, error);
					}
				}
			}
		} else if (strcmp("SAVE", command) == 0) {
			scanf("%s", filename); scanf("%s", ascii);
			verify_ascii_save(a, ascii, command);
			SAVE(a, filename);
		} else {
			if (strcmp(command, "EXIT") != 0) {
				printf("Invalid command\n");
				scanf("%c", &t);
				while (t != '\n')
					scanf("%c", &t);
			}
		}
		if (a->ascii == 0)
			scanf("%s", command);
		a->ascii = 0;
	}
	EXIT(a);
	return 0;
}
