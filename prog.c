#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<search.h>

#define MAX 65535
void get_hash(FILE *fp)
{
	char buf[MAX];
	char *name;
	ENTRY tmp;

	hcreate(65535);
	while (fgets(buf,MAX,fp)) {
		char *ptr;
		name = strtok(buf,"#");
		ptr = malloc(strlen(name));
		name[strlen(name)-1] = '\0';
		strcpy(ptr,name);
		printf("name:%s\n",name);
		tmp.key = ptr;
		tmp.data = NULL;
		hsearch(tmp, ENTER);
	}
}



void get_name(char *buf,char **ret,char **year)
{
	char *ptr,*ptr2;
	char *name;

	while (1) {
		char val;
		ptr = strchr(buf, '(');
		val = *(ptr+1) - '0';
		if ((ptr != buf) &&
		    (((val >= 0) && (val <= 9)) ||
		     (*(ptr+1) == '?'))){
			break;
		}
		buf = ptr+1;
	}
	strncpy(*year,
		ptr,
		5);
	ptr2 = *year;
	*(ptr2+5) = ')';
	*(ptr2+6) = '\0';
	name = malloc(ptr-buf+2);
	memcpy(name,
	       buf,
	       ptr-buf-1);
	name[ptr-buf] = '\0';
	if (name[0] == '"') {
		name[ptr-buf-2] = '\0';
		ptr = name+1;
	} else {
		ptr = name;
	}

	strcpy(*ret,
	       ptr);
	free(name);
}

char *get_keyword(char *buf)
{
	char *ptr = strrchr(buf,'\t');
	return (ptr+1);
}

int check_profane(char *word)
{
	FILE *prof_fp;
	char buf[MAX];
	int ret = 0;

	prof_fp = fopen("bad-words.txt","r");
	while (fgets(buf,MAX,prof_fp)) {
		buf[strlen(buf)-1] = '\0';
		if (strstr(word,buf)) {
			printf("profane:%s\n",buf);
			ret = 1;
			break;
		}
	}
	fclose(prof_fp);
	return ret;
}

int main(int argc,char **argv)
{
	FILE *fp;
	FILE *name_fp;
	char buf[MAX];
	int count = 0;
	char *token = " \t";
	char *name,*year,*keyword;
	ENTRY tmp;
	char dir[MAX];
	char filename[MAX];

	fp = fopen("keywords.list","r");
	name_fp = fopen(argv[1],"r");
	get_hash(name_fp);
	strcpy(dir,argv[2]);
	strcpy(filename,"rm -rf ");
	strcat(filename,dir);
	strcat(filename,"*");
	system(filename);

	name = malloc(MAX);
	year = malloc(15);

	while (fgets(buf, MAX, fp)) {
		char *ptr;
		FILE *new_fp;
		get_name(buf,&name,&year);
		if (name[0] == '\0')
			continue;
		keyword = get_keyword(buf);
		tmp.key = name;
		if (hsearch(tmp, FIND)) {
			if (check_profane(keyword))
				continue;
			while(ptr = strchr(name,' '))
				*ptr = '_';
			while(ptr = strchr(name,'/'))
				*ptr = '_';
			strcpy(filename,dir);
			strcat(filename,name);
			strcat(filename,"_");
			strcat(filename,year);
			strcat(filename,".txt");
			printf("name:%s\n",filename);
			new_fp = fopen(filename,"a+");
			fwrite(keyword,strlen(keyword),1,new_fp);
			fclose(new_fp);
		}
	}
	fclose(fp);
	return 0;
}
