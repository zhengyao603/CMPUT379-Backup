#include <stdio.h>
#include <stdlib.h>


int main(){
	FILE *data;
	FILE *out1;
	FILE *out2;
	FILE *plot1;
	FILE *plot2;

	int index = 0;
	int array[1000000] = {0};

	double mean = 0;
	double variance = 0;

	// open the data file
	if ((data = fopen("data.txt", "r")) == NULL){
		printf("fail to open file 'data.txt'\n");
		exit(-1);
	}

	// open the out1 file
	if ((out1 = fopen("out1.txt", "w")) == NULL){
		printf("fail to open file 'out1.txt'\n");
		exit(-1);
	}

	// open the out2 file
	if ((out2 = fopen("out2.txt", "w")) == NULL){
		printf("fail to open file 'out2.txt'\n");
		exit(-1);
	}

    // keep reading
	for (;;){
		int temp;
		int if_end = fscanf(data, "%d", &temp);
		index += 1;
		mean += temp;
		array[temp - 1] += 1;
		if (if_end < 0){
			break;
		}
		fprintf(out1, "%d, %d\n", index, temp);
	}

	fclose(data);
	fclose(out1);

	mean = mean / index;

    // record data for drawing histogram
	for (unsigned int i = 0; i < 1000000; i++){
		if (array[i] != 0){
			variance += array[i] * ((i + 1) - mean) * ((i + 1) - mean);
			fprintf(out2, "%d, %d\n", i + 1, array[i]);
		}
	}
	fclose(out2);

	variance = variance / index;
	printf("mean: %f\n", mean);
	printf("variance: %f\n", variance);


	// open 'gnuplot'
	if ((plot1 = popen("gnuplot", "w")) == NULL){
		printf("fail to open 'gnuplot'\n");
		exit(-1);
	}

    // plotting diagram
	fprintf(plot1, "set terminal png size 900,400\n");
	fprintf(plot1, "set output 'plot1.png'\n");
	fprintf(plot1, "set datafile separator ','\n");
	fprintf(plot1, "plot 'out1.txt' using 1:2 title 'working set size' with linespoints\n");
	pclose(plot1);

	// open 'gnuplot'
	if ((plot2 = popen("gnuplot", "w")) == NULL){
		printf("fail to open 'gnuplot'\n");
		exit(-1);
	}

    // plotting diagram
	fprintf(plot2, "set terminal png size 900,400\n");
	fprintf(plot2, "set output 'plot2.png'\n");
	fprintf(plot2, "set datafile separator ','\n");
	fprintf(plot2, "plot 'out2.txt' using 1:2 title 'working set size' with boxes\n");
	pclose(plot2);

	return 0;
}