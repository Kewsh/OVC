#include "main.h"

/*
 * returns the maximum of two given numbers
 */
int getMax(int a, int b){
	return (a > b)? a : b;
}

/*
 * LCS for X[0..m-1], Y[0..n-1]
 */
void lcs(const char *X, const char *Y, int m, int n){
	int L[m+1][n+1];
	int i, j;
	for (i=0; i<=m; i++){
		for (j=0; j<=n; j++){
			if (i == 0 || j == 0)
				L[i][j] = 0;
			else if (X[i-1] == Y[j-1])
				L[i][j] = L[i-1][j-1] + 1;
			else
				L[i][j] = getMax(L[i-1][j], L[i][j-1]);
		}
	}

	free(loc_arr1);
	free(loc_arr2);
	int index = L[m][n];
	length = L[m][n];
	loc_arr1 = malloc(L[m][n]*sizeof(int));
	loc_arr2 = malloc(L[m][n]*sizeof(int));

	i = m, j = n;
	while (i > 0 && j > 0){
		if (X[i-1] == Y[j-1]){
			loc_arr1[index-1] = i-1;
			loc_arr2[index-1] = j-1;
			i--; j--; index--;
		}
		else if (L[i-1][j] > L[i][j-1])
			i--;
		else
			j--;
	}
}

/*
 * __diff__
 */
void diff(const char *path1, const char *path2, const char *path3){

  	char *file1, *file2, c, c1, c2;
	int i, j, k, len1, len2, temp1 = 0, temp2 = 0, flag1 = 0, flag2 = 0;
	int L, R, left1, right1, left2, right2, loc2, diffy, temp3, len, temp4;
	FILE *fptr1, *fptr2, *fptr3;

	fptr1 = fopen(path1, "r");
	fptr2 = fopen(path2, "r");
	fptr3 = fopen(path3, "w");

	while(1){
		c1 = '\0', c2 = '\0';
		if (temp1 == 1 && temp2 == 1)
			break;
		file1 = malloc((MAX_PICK_SIZE + 1)*sizeof(char));
		file2 = malloc((MAX_PICK_SIZE + 1)*sizeof(char));
		if (temp1 == 0){
			for (i = 0; i < MAX_PICK_SIZE; i++){
				c1 = fgetc(fptr1);
				if (c1 == EOF){
					flag1 = 1;
					break;
				}
				file1[i] = c1;
			}
			file1[i] = '\0';
		}
		else
			file1[0] = '\0';
		if (flag1 == 1)
			temp1 = 1;
		if (temp2 == 0){
			for (j = 0; j < MAX_PICK_SIZE; j++){
				c2 = fgetc(fptr2);
				if (c2 == EOF){
					flag2 = 1;
					break;
				}
				file2[j] = c2;
			}
			file2[j] = '\0';
		}
		else
			file2[0] = '\0';
		if (flag2 == 1)
			temp2 = 1;
		// passing the file pieces to LCS function
		len1 = strlen(file1);
		len2 = strlen(file2);
		lcs(file1, file2, len1, len2);

		// here starts the process of printing the differences into the third file
		if (length == 0){																					 // if there is no common subsequence
			fprintf(fptr3, "+%d ", len2);
			for (i = 0; i < len2; i++){
				fputc(file2[i], fptr3);
			}
		}
		else{																								 // if there is a common subsequence
			L = 0, R = 1, i = 1, loc2 = 0, temp3 = 0, temp4 = 0, len = 0;
			left1 = -1, right1 = loc_arr1[0], left2 = -1, right2 = loc_arr2[0];

			while (L < length){
				if ((right1 - left1) >= 2 || (right2 - left2) >= 2){
					if (temp3 == 1){
						if (temp4 == 1){
							len--;
							temp4 = 0;
						}
						fprintf(fptr3, ".%d\n", len);
						len = 0;
					}
					if (temp3 == 0 && left1 != -1)
						fprintf(fptr3, ".0\n");
					else if (temp3 == 1)
						temp3 = 0;
					loc2 = left2+1;
					if ((diffy = right1 - left1 - 1) >= 1)
						fprintf(fptr3, "-%d\n", diffy);
					if ((diffy = right2 - left2 - 1) >= 1){
						fprintf(fptr3, "+%d ", diffy);
						while (diffy > 0){
							c2 = file2[loc2++];
							fputc(c2, fptr3);
							diffy--;
						}
						fputc('\n', fptr3);
					}
				}
				else{
					if (left1 == -1)
						temp4 = 1;
					temp3 = 1;
					len++;
				}
				L++, R++;
				left1 = right1, right1 = loc_arr1[i], left2 = right2, right2 = loc_arr2[i++];
			}
			if (temp3 == 1)
				fprintf(fptr3, ".%d\n", len);
			else
				fprintf(fptr3, ".0\n");
			if (left1 != len1-1){
				diffy = len1 - left1 - 1;
				fprintf(fptr3, "-%d\n", diffy);
			}
			if (left2 != len2-1){
				diffy = len2 - left2 - 1;
				loc2 = left2+1;
				fprintf(fptr3, "+%d ", diffy);
				while (diffy > 0){
					c2 = file2[loc2++];
					fputc(c2, fptr3);
					diffy--;
				}
				fputc('\n', fptr3);
			}
		}
		free(file1);
		free(file2);
	}

  fclose(fptr1);
  fclose(fptr2);
  fclose(fptr3);
}