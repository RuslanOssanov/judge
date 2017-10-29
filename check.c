#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

int is_correct_test(char *name){
	int i = 0;
	if (strlen(name) != 7){
		return 1;
	}
	for (i = 0; i < 3; i++){
		if (name[i] < '0' || name[i] > '9'){
			return 1;
		}
	}
	if (name[i++] != '.' || name[i++] != 'd' || name[i++] != 'a' || name[i] != 't'){
		return 1;
	}
	return 0; // ok  // OK
}

int is_correct_problem(char *name){
	if (strlen(name) != 2){
		return 1;
	}
	int i = 0;
	for (i = 0; i < 2; i++){
		if (name[i] < '0' || name[i] > '9'){
			return 1;
		}
	}
	return 0; // ok // OK
}

int is_correct_user_problem(char *name){
	if (strlen(name) != 4){
		return 1;
	}
	int i = 0;
	if (name[0] < '0' || name[0] > '9'){
		return 1;
	}
	if (name[1] < '0' || name[1] > '9'){
		return 1;
	}
	if (name[2] != '.'){
		return 1;
	}
	if (name[3] != 'c'){
		return 1;
	}
	return 0; // ok // OK
}

int check_test(){
	DIR *dir_of_test, *dir_of_problem; 
	FILE *stream;
	struct dirent *entry;
	char *path_to_test = "test";
	char *path_to_problem = NULL;
	int i, count_of_problems = 0, count_of_tests = 0;

	dir_of_test = opendir(path_to_test);
    if (!dir_of_test) {
        perror("diropen");
        exit(1);
    };
    while ((entry = readdir(dir_of_test)) != NULL){
    	if (!(is_correct_problem(entry->d_name))){
    		count_of_problems++;
    	}
    }
    closedir(dir_of_test);

    stream = fopen("test/info.txt", "w");
    fprintf(stream, "%d\n", count_of_problems);

	for (i = 1; i <= count_of_problems; i++){
		path_to_problem = (char*)realloc(path_to_problem, 8 * sizeof(char));
		sprintf(path_to_problem,"test/%02d",i);
    	dir_of_problem = opendir(path_to_problem);
    	fprintf(stream, "%s ", &path_to_problem[5]);
    	while ( (entry = readdir(dir_of_problem)) != NULL){
	    	if (!(is_correct_test(entry->d_name))){
	    		count_of_tests++;
	    	}
	    }
	    fprintf(stream, "%d\n", count_of_tests);
	    count_of_tests = 0;
	    if (path_to_problem){
	    	free(path_to_problem);
	    	path_to_problem = NULL;
	    }
	    closedir(dir_of_problem);
    }
    fclose(stream);
    return count_of_problems; // OK // OK
}

int check_data(){ // OKAY
	DIR *dir_of_data, *dir_of_user;
	FILE *stream;
	struct dirent *entry, *entry2;
	int i, user_sol = 0, count_of_users = 0;
	const char *path_to_data = "data";
	char *path_to_user = NULL;
	stream = fopen("data/info.txt", "w");
	dir_of_data = opendir(path_to_data);
    if (!dir_of_data) {
        perror("diropen");
        exit(1);
    };
    while ((entry = readdir(dir_of_data)) != NULL){
    	user_sol = 0;
    	if (!(strcmp(entry->d_name,"..")) || !(strcmp(entry->d_name,".")) || !(strcmp(entry->d_name,"info.txt"))){
    		continue;
    	}
    	count_of_users++;
	    path_to_user = (char*)realloc(path_to_user, (6 + strlen(entry->d_name) * sizeof(char)));
	    strncpy(path_to_user, "data/", 5);
	    strncpy(&path_to_user[5], entry->d_name, strlen(entry->d_name));
	    path_to_user[5 + strlen(entry->d_name)] = '\0';
	    fprintf(stream, "%s ", entry->d_name);
	    fprintf(stream,"{");
	    dir_of_user = opendir(path_to_user);
	    while ((entry2 = readdir(dir_of_user)) != NULL){
	    	if (!(is_correct_user_problem(entry2->d_name))){
	    		if (user_sol == 0){
	    			fprintf(stream, "%s", entry2->d_name);
	    			user_sol++;
	    		} else {
	    			fprintf(stream, "  %s", entry2->d_name);
	    		}
	    	}
	    }
	    fprintf(stream, "}\n");
	    closedir(dir_of_user);
	    if (path_to_user){
	    	free(path_to_user);
	    	path_to_user = NULL;
	    }
	}
	fprintf(stream, "%d\n", count_of_users);
	closedir(dir_of_data);
    fclose(stream);
    return count_of_users;
}

int this_user_have_this_problem(char *user, int problem){
	FILE *info;
	char *key_problem= NULL;
	char *word = NULL;
	char ch;
	int len;
	int flag = 0;
	key_problem = (char*)realloc(key_problem, 5 * sizeof(char));
	if (problem <= 9){
		key_problem[0] = '0';
		key_problem[1] = problem + '0';
	} else {
		key_problem[0] = problem / 10 + '0';
		key_problem[1] = problem % 10 + '0';
	}
	key_problem[2] = '.';
	key_problem[3] = 'c';
	key_problem[4] = '\0';

	info = fopen("data/info.txt", "r");
	ch = fgetc(info);
	while (ch != EOF){
		len = 0;
		if (ch == '{'){
			ch = fgetc(info);
			continue;
		}
		while (ch != ' ' && ch != '\n' && ch != '}'){
			word = (char*)realloc(word,(len +1) * sizeof(char));
			word[len++] = ch;
			ch = fgetc(info);
		}
		word = (char*)realloc(word,(len + 1) * sizeof(char));
		word[len] = '\0';
		if (strlen(word) != strlen(user)){
			ch = fgetc(info);
			continue;
		}
		if (!strncmp(word, user, strlen(user))){
			free(word);
			word = NULL;
			while (ch != '{'){
				ch = fgetc(info);
			}
			ch = fgetc(info);
			while (ch != '}'){
				len = 0;
				while (ch != ' ' && ch != '}'){
					word = (char*)realloc(word,(len +1) * sizeof(char));
					word[len++] = ch;
					ch = fgetc(info); 
				}
				word = (char*)realloc(word,(len + 1) * sizeof(char));
				word[len] = '\0';
				if (strlen(word) != strlen(key_problem)){
					free(word);
					word = NULL;
					if (ch == '}'){
						return 0;
					}
					ch = fgetc(info);
					continue;
				}
				if (!strncmp(word, key_problem, strlen(word))){
					free(word);
					return 1;
				}
				if (ch == '}'){
					if (word){
						free(word);
					}
					return 0;
				}
				ch = fgetc(info);
			}
			return 0;
		}
		ch = fgetc(info);
	}
	fclose(info);  // (так себе)
}

char *create_user_code(char *user, int problem){ // OKAY
	char *tmp = NULL;
	tmp = (char*)realloc(tmp,(11 + strlen(user)) * sizeof(char));
	strncpy(tmp,"data/", 5);
	strncpy(&tmp[5],user,strlen(user));
	tmp[5 + strlen(user)] = '/';
	sprintf(&tmp[6 + strlen(user)],"%02d", problem);
	strncpy(&tmp[8 + strlen(user)],".c",2);
	tmp[10 + strlen(user)] = '\0';
	return tmp;
}


char *count_of_tests_problem(int problem, int count_of_problems){
	char *key_problem= NULL;
	FILE *info;
	char *tmp = NULL;
	tmp = (char*)realloc(tmp,3 * sizeof(char));
	int i;
	int a,b;
	info = fopen("test/info.txt", "r");
	fscanf(info, "%d\n", &a);
	for (int i = 1; i <= count_of_problems; ++i)
	{
		fscanf(info, "%d %d\n", &a, &b);
		if (a == problem){
			fclose(info);
			sprintf(tmp,"%02d",b);
			return tmp;
		}
	}//OKAY
}


void judge(int count_of_problems){
	DIR *dir_of_data;
	FILE *stream;
	struct dirent *entry;
	int current_problem;
	int pid, status;
	int sum = 0;
	char *user_code = NULL;
	char *count_of_tests = NULL;
	const char *path_to_data = "data";
	stream = fopen("result.txt", "w");
	dir_of_data = opendir(path_to_data);
    if (!dir_of_data) {
        perror("diropen");
        exit(1);
    };
	while ((entry = readdir(dir_of_data)) != NULL){
    	if (!(strcmp(entry->d_name,"..")) || !(strcmp(entry->d_name,".")) || !(strcmp(entry->d_name,"info.txt"))){
    		continue;
    	}
    	fprintf(stream, "%-10s  ", entry->d_name);
		for (current_problem = 1; current_problem <= count_of_problems; current_problem++){
	    	if (this_user_have_this_problem(entry->d_name, current_problem)){
	    		user_code = create_user_code(entry->d_name, current_problem);
	    		count_of_tests = count_of_tests_problem(current_problem, count_of_problems);
	    		pid = fork();
	    		if (!pid){
	    			execlp("./judge", "./judge", user_code, count_of_tests, NULL);
	    		} else {
	    			wait(&status);
	    			sum += (status/256);
	    			fprintf(stream, "%3d  ", (status/256));
	    		}
	    	}
	    }
	    fprintf(stream, "%4d\n", sum);
	    sum = 0;
	}
	fclose(stream);
    closedir(dir_of_data);
    return;	
}


int main() {
    int count_of_problems;
    int count_of_users;


    count_of_problems = check_test();
    count_of_users = check_data();
    judge(count_of_problems);
	return 0;
}
