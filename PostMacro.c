#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <stdbool.h>

int GetN_CSVs(char dir[])
{
	DIR *p;
	int length_list=0;
	struct dirent *pp;
	p = opendir(dir);

	if(p!=NULL)
	{
		while ((pp = readdir (p))!=NULL)
		{
			int l = strlen(pp -> d_name);
			if(strncmp(pp->d_name+l-4, ".csv", 4) == 0)
			{
				length_list++;
			}
		}
	}
	closedir(p);
	return length_list;
}

int getN_lines(char file[])
{
	FILE *fp;
	int count=0;
	
	fp = fopen(file,"r");

	if(fp == NULL)
	{
		printf("Could not open file %s",file);
	}
	for (char c = getc(fp); c!=EOF; c=getc(fp))
	{
		if (c == '\n')
		{
			count++;
		}
	}
	return count;
}

char** GET_FILENAMES(char dir[], int size)
{
	char **FILENAME = malloc (sizeof(char *) * size);
    DIR *p; struct dirent *pp; p = opendir(dir);
	int i=0;
	
	if(p!=NULL)
	{
		while ((pp = readdir(p))!=NULL)
		{
			int l = strlen(pp -> d_name);
			if(strncmp(pp->d_name+l-4,".csv", 4) == 0)
			{
				char filename[254];
				strncpy(filename,pp->d_name,254);
				filename[254] = '\0';
				FILENAME[i] = malloc(sizeof(char) * 255);
				strncpy(FILENAME[i],filename,254);
				i++;
				if(i==size)
				{
					closedir(p);
					return FILENAME;
				}
			}
		}
	}
	else
	{
		printf("ERROR: Invalid path name! \n");
	}
}

int PARSE_DATA(char **FILENAMES, int n_CSVs)
{
	int N_conditions=0, i = 0;
	
	for(i=0;i<n_CSVs;i++)
	{
		FILE * pCSV,* pNEW_CSV; 
		pCSV = fopen(FILENAMES[i],"r");
		int n_lines, j=0; bool found = false; char DONELIST[n_CSVs][255];

		memcpy(DONELIST[i],FILENAMES[i],strlen(FILENAMES[i]) - 9);
		DONELIST[i][strlen(FILENAMES[i]) - 9] = '\0';
		char output_name[sizeof(FILENAMES[i])/sizeof(char) + 11]  = "Output/";
		strcat(output_name,DONELIST[i]);
		strcat(output_name,".csv");
		n_lines = getN_lines(FILENAMES[i]);
		
		while(j <= i-1)
		{
			if(i >= 1 && strcmp(DONELIST[i],DONELIST[j])==0)
			{
				printf("%s --> %s\n",FILENAMES[i],DONELIST[j]);
   		        pNEW_CSV = fopen(output_name,"a");
				for(int k=0;k<n_lines; k++)
				{
					char line[1024];
					fgets(line,1024,pCSV);
					if(k!=0)
					{
						fputs(line,pNEW_CSV);
					}
				}
				fclose(pNEW_CSV);
				found = true;
				break;
			}
			j++;
		}
		if(!found)
		{
			printf("Saving data for %s\n", FILENAMES[i]);
			N_conditions++;
			pNEW_CSV = fopen(output_name,"w");
			for(int k=0;k<n_lines;k++)
			{
				char line[1024];
				fgets(line,1024,pCSV);
				if(k!=0)
				{
					fputs(line,pNEW_CSV);
				}
			}
			fclose(pNEW_CSV);
		}

		fclose(pCSV);
	}

	return N_conditions;
}

void Make_Master_CSV(int n_out,bool thresh, double threshold)
{
	FILE * pMASTER_CSV; FILE * pTHRESHOLD;
	pMASTER_CSV = fopen("Output/MASTER_NUCLEAR_INTENSE.csv","w");
	
	if(thresh)
	{	
		pTHRESHOLD = fopen("Output/MASTER_THRESHOLD_DATA.csv","w");
	}
	
	char **OUT = GET_FILENAMES("Output",n_out);
	if(!OUT)
	{
		printf("ERROR: cannot allocate array of output CSV files \n");
		exit(1);
	}
	int o = n_out;
	for(int i=0; i<o;i++)
	{
		if(strncmp(OUT[i],"MASTER",6)==0)
		{
			n_out++;
		}
		free(OUT[i]);
	}

	free(OUT);
	char **OUTLIST = GET_FILENAMES("Output",n_out);

	int max = 0, pos = 0;
	for(int i=0; i < n_out; i++)
	{
		char filename[262] = "Output/";
		strcat(filename,OUTLIST[i]);
		if(strncmp(OUTLIST[i],"MASTER",6 !=0 ))
		{
			int lines = getN_lines(filename);
			if(max < lines)
			{
				max = lines; pos=i;
			}
		}
	}


	for(int i=0; i<n_out; i++)
	{
		if(strncmp(OUTLIST[i],"MASTER",6) !=0)
		{
        	fputs(OUTLIST[i],pMASTER_CSV);
			fputs(",",pMASTER_CSV);
			if(thresh)
			{
				fputs(OUTLIST[i],pTHRESHOLD);
				fputs(",",pTHRESHOLD);
			}
		}
	}
	fputs("\n",pMASTER_CSV);
	if(thresh){fputs("\n",pTHRESHOLD);}
	char filename[263]; double values[n_out][max];
	for(int i=0; i <= max; i++)
	{
		strcpy(filename,"Output/");
		for(int j=0; j < n_out; j++)
		{
			strcpy(filename,"Output/");
          	strcat(filename,OUTLIST[j]);
			if(strncmp(OUTLIST[j],"MASTER",6) !=0)
			{
				FILE * pCSV; pCSV = fopen(filename,"r"); 
				char line[1024]; int k = 0;
				while(fgets(line,sizeof(line),pCSV) != NULL)
				{
					if(k == i)
					{
						char * pline; pline = strtok(line,","); pline = strtok(NULL,",");
						fputs(pline,pMASTER_CSV); fputs(",",pMASTER_CSV);
						if(thresh)
						{
							values[j][i] = strtod(pline,&pline);
						}
						break;
					}
					else
					{
						k++;
					}
				}
				fclose(pCSV);
			}
		}
		fputs("\n",pMASTER_CSV);
	}

	fclose(pMASTER_CSV);

	if(thresh)
	{
		printf("%s","\nCalculated Threshold Data:\n");
		for(int i=0; i<n_out; i++)
		{
			if(strncmp(OUTLIST[i],"MASTER",6)!=0)
			{
				double n_over=0, n_total=0, percentage=0;
				for(int j=0; j < max; j++)
				{
					if(values[i][j] > threshold && values[i][j] < 10000 && values[i][j] > 10) //change I values 
					{
						n_over++;
						n_total++;
					}
					else if(values[i][j] < 10000 && values[i][j] > 10)
					{
						n_total++;
					}
				}
				percentage = ((double)n_over/n_total)*100;
				char perc[20]; sprintf(perc,"%f",percentage); 
				fputs(perc,pTHRESHOLD); fputs(",",pTHRESHOLD);
				printf("%-30s : %lf\n",OUTLIST[i],percentage);
			}
		}
		fclose(pTHRESHOLD);
	}
}

int main()
{		
	int len = GetN_CSVs(".");
	char **FILENAMES = GET_FILENAMES(".",len);
	if(!FILENAMES)
	{
		printf("Unable to allocate array\n");
		return 1;
	}

	mkdir("Output",0777);
	int n_conditons = PARSE_DATA(FILENAMES,len) ; bool BGT = false;
	printf("\n%d different conditons found\n",n_conditons);

	for(int i =0; i<len; i++)
		free(FILENAMES[i]);
	free(FILENAMES);

	printf("\nDo you want to calculate percent positve cells? (y/n) >> ");
	char a;	scanf("%c",&a); double threshold = 0;
	
	if(a == 'y')
	{
		BGT = true;
		printf("\nEnter threshold value >> ");
		scanf("%lf",&threshold);
	}
	else
	{
		BGT = false;
	}

	Make_Master_CSV(n_conditons,BGT, threshold);	

	printf("\nDone! Check Output folder to see calculated data.\n");

	return 0;
}
