#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct class_s
{
	int is_lower, is_upper, is_digit, len;
	char class[20];

}class_s;

void init_class(class_s *cl)
{
	cl->is_upper = 0;
	cl->is_lower = 0;
	cl->len = 0;
}



// version 5 : 
// 1. empty pattern
// 2. exact match
// 3. anchor ^
// 4. anchor $
// 5. metachar .
// 6. closure *
// 7. non greedy ?
// 8. character class []
// 9. metachar \d & \w


#define DEBUG 0

int match(char *pat, char *text);
int match_here(char *pat, char *text);
int match_star(char ch, char* pat, char* text);
int match_star_greedy(char ch, char* pat, char* text);
int match_plus(char ch, char* pat, char* text);
int match_plus_greedy(char ch, char* pat, char* text);
int match_ques(char ch, char *pat, char *text);
int match_word( char* pat, char* text );
int match_digit( char *pat, char *text );

//helper functions
int free_patterns(char **patterns, int m);
void extract_class_from_str(class_s *cl, char* str);
int is_char_in_class(char ch);
void reset_trackers();

class_s c_temp;
char temp_str[40];
int some_temp = 0;


int main()
{
	#if 1
	char text[4000];
	int m, boo;

	scanf("%s", text);
	scanf("%d", &m);

	char** patterns = (char**)malloc(m * sizeof(char*));
	for(int i = 0; i < m; ++i)
		patterns[i] = (char*)malloc(1000 * sizeof(char));
		
	for(int i = 0; i < m; ++i)
		scanf("%s", patterns[i]);

	for(int i = 0; i < m; ++i)
		printf("%d \n", match(patterns[i], text));

	free_patterns(patterns, m);

	#endif


	return 0;
}

int match(char *pat, char *text)
{
	if(*pat == '^')
	{
		return match_here(pat + 1, text);
	}
	do
	{	
		if(match_here(pat, text))
		{	
			return 1;
		}
		
	} 	while(*text++ != '\0');
	return 0;
}

int match_here(char *pat, char *text)
{

	//printf("Match here pat: %c text: %c start: %d end: %d\n", *pat, *text, start, end);
	// empty pattern
	if(*pat == '\0')
		return 1;

	if(pat[1] == '?')
	{
		return match_ques(pat[0], pat+2, text);
	}
	if(pat[1] == '*')
	{
		if(pat[2] == '?')
			return match_star(pat[0], pat+3, text);
		else
			return match_star_greedy(pat[0], pat+2, text);
	}

	if(pat[1] == '+')
	{
		if(pat[2] == '?')
			return match_plus(pat[0], pat+3, text);
		else
			return match_plus_greedy(pat[0], pat+2, text);
	}

	if(pat[0] == '$' && pat[1] == '\0')
		return *text == '\0';
	
	if( pat[0] == '\\' )
	{
        if( pat[1] == 'd' )
            return match_digit( pat + 2, text );

        else if( pat[1] == 'w' )
            return match_word( pat + 2, text );
    }

	if(*text != '\0' && (*pat == '.' || *pat == *text))
	{
		return match_here(pat + 1, text+1);
	}

	if(*pat == '[')
	{
		int x = 0;
		while(*(++pat) != ']')
		{
			temp_str[x++] = *pat;
		}
		temp_str[x] = '\0';

		//printf("%s\n", temp_str);	
		init_class(&c_temp);
		extract_class_from_str(&c_temp, temp_str);

		
		return match_here(pat, text);

	}

	if(*pat == ']' && is_char_in_class(*text))
	{	
		return match_here(pat+1, text+1);
	}

		
	return 0;
}

int match_star(char ch, char* pat, char* text)
{
	char endt = '\0';
	do
	{
		//printf("Star match here\n");
		some_temp = ch == ']' && is_char_in_class(*text);


		if(match_here(pat, text))
		{
			
			endt = *(text-1);
			//printf("%c\n", endt);
			return 1;
		}
		
			
	} while(*text != '\0' && (*text++ == ch || ch == '.' || some_temp ));

	return 0;
}

int match_star_greedy(char ch, char* pat, char* text)
{

	char endt = '\0';
	some_temp = ch == ']' && is_char_in_class(*text);

	do
	{	
		//printf("Star match here\n");
		some_temp = ch == ']' && is_char_in_class(*text);

		if(match_here(pat, text))
		{
			endt = *(text-1);


		}
		
			
	} while(*text != '\0' && (*text++ == ch || ch == '.' || some_temp ));

	if(endt != '\0')
	{
		//printf("%c\n", endt);
		return 1;
	}	

	return 0;
}


int match_plus(char ch, char* pat, char* text)
{

	char endt = '\0';
	do
	{
		text++;
		//printf("Star match here\n");
		some_temp = ch == ']' && is_char_in_class(*text);

		if(match_here(pat, text))
		{
			endt = *(text-1);
			//printf("%c\n", endt);
			return 1;
		}
		

	} while(*text != '\0' && (*text == ch || ch == '.' || some_temp ));

	return 0;
}

int match_plus_greedy(char ch, char* pat, char* text)
{
	char endt = '\0';
	some_temp = ch == ']' && is_char_in_class(*text);

	do
	{	
		text++;
		//printf("Star match here\n");
		some_temp = ch == ']' && is_char_in_class(*text);

		if(match_here(pat, text))
		{
			endt = *(text-1);
		}

			
	} while(*text != '\0' && (*text == ch || ch == '.' || some_temp ));

	if(endt != '\0')
	{
		//printf("%c\n", endt);
		return 1;
	}

	return 0;
}

int match_ques(char ch, char *pat, char *text)
{
	if(ch == *text || ch == '.')
	{	
		return match_here(pat, text + 1);
	}
	else
	{
		return match_here(pat, text);
	}
	
	//return match_here(pat, text);
}




//---------------------HELPER FUNCTIONS-----------------------------


void extract_class_from_str(class_s *cl, char* str)
{
	int length = strlen(str);
	int i = 0;


	while(i < length)
	{
		if(str[i] == 'a' && str[i+1] == '-' && str[i+2] == 'z')
		{
			cl->is_lower = 1;
			i+=3;
			//printf("ye1\n");
		}
		if(str[i] == 'A' && str[i+1] == '-' && str[i+2] == 'Z')
		{
			cl->is_upper = 1;
			i+=3;
			//printf("ye2\n");
		}
		if(str[i] == '0' && str[i+1] == '-' && str[i+2] == '9')
		{
			cl->is_digit = 1;
			i+=3;
			//printf("ye3\n");
		}

		else
		{
			//printf("ye4\n");
			cl->class[cl->len++] = str[i];
			i++;
		}
	}

}

int free_patterns(char **patterns, int m)
{
	for (int i = 0; i < m; i++)
		free(patterns[i]);
	
	free(patterns);
	
}

int is_char_in_class(char ch)
{
	if(c_temp.is_digit && isdigit(ch) || c_temp.is_lower && islower(ch) || c_temp.is_upper && isupper(ch))
		return 1;
	
	
	for (int i = 0; i < c_temp.len; i++)
	{
		if(c_temp.class[i] == ch)
			return 1;
	}
	
	return 0;
	
}

int match_word( char* pat, char* text )
{
    char words[] = "[a-zA-Z0-9_]*";
    return match_here( strcat( words, pat ), text );
}

int match_digit( char *pat, char *text )
{
    char dig[] = "[0-9]";
    return match_here( strcat( dig, pat ), text );
}


