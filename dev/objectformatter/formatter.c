#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[]){
	FILE *input_file; 
	FILE *output_file;

	input_file = fopen(argv[1], "rb");

	char buffer[255] = "";
	int n = 0;
	while(argv[1][n] != '.' && argv[1][n] != '\0'){
		buffer[n] = argv[1][n];
		n++;
	}
	buffer[n] = '\0';
	strcat(buffer, ".zbj");
	output_file = fopen(buffer, "wb");

	char line[256];
	float v1, v2, v3;
	int f1, f2, f3;
	int l1, l2;
	int counter = 0;

	while(fgets(line, sizeof(line), input_file)){
		if(line[0] == 'v' && line[1] == ' '){
			if(sscanf(line, "v %f %f %f", &v1, &v2, &v3) == 3){
				if(counter == 0){
					fprintf(output_file, "v\n");
					counter = 1;
				}
				fprintf(output_file, "%f %f %f\n", v1, v2, v3);
			}
		}else if(line[0] == 'f' && line[1] == ' '){
			if(sscanf(line, "f %d %d %d", &f1, &f2, &f3) == 3){
				if(counter == 1){
					fprintf(output_file, "f\n");
					counter = 2;
				}
				fprintf(output_file, "%d %d %d\n", f1, f2, f3);
			}
		}else if(line[0] == 'l' && line[1] == ' '){
			if(sscanf(line, "l %d %d", &l1, &l2) == 2){
				if(counter == 1){
					fprintf(output_file, "l\n");
					counter = 2;
				}
				fprintf(output_file, "%d %d\n", l1, l2);
			}
		}
	}

	fclose(input_file);
	fclose(output_file);

	return 0;
}