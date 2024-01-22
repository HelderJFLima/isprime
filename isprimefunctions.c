/** by HelderJFLima **/

/* Functions used in the 'isprime' program */


#include <ctype.h>
#include <errno.h>				/* For error messages */
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isprimefunctions.h"

#define BASE	  10			/* Base used in strtoul function (stdlib.h) */
#define ENTRYSIZE 20			/* Maximum size to copy a file number */
#define MINVALUE  7				/* Minimum value acceptable for testing */
#define STRING1   "quantity="	/* String used in the file */
#define STRING2   ",last="		/* Ditto */


unsigned long* adlalloc(unsigned long minimum, unsigned long maximum)
{
	unsigned long size;				/* List size */
	unsigned long* list;

									/* According to the Prime Number Theorem */
	size = (unsigned long)ceil( 1.3 * (   ( maximum / log(maximum) )
										- ( minimum / log(minimum) ) ) );

	list = (unsigned long*)calloc( size, sizeof(unsigned long) );

	if(list == NULL)
		perror("\nadlalloc->calloc");

	return list;
}


unsigned long getfromarg(char *argument)
{
	unsigned long number;

	if( isdigit(*argument) )						/* Valid decimal number */
	{
		number = strtoul(argument, NULL, BASE);		/* Convert to unsigned long */

		if(number == ULONG_MAX)						/* Upper limit violation */
		{
			printf("\nThe number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\nFormat error\n");

		return 0;
	}

	return (number < MINVALUE) ? 0 : number;
}


unsigned long getfromstdin(void)
{
	unsigned long number;
	int length;										/* Length of string */
	char *string, *strp;

	length = 2 * (int)ceil( log10(ULONG_MAX) );		/* Twice the length of */
													/* a long int          */

	string = (char*)calloc( length, sizeof(char) );

	if(string == NULL)
	{
		perror("\ngetfromstdin->calloc");

		return 0;
	}

	printf("\nEnter the number to test: ");

	if( !fgets(string, length, stdin) )				/* Get a number */
	{
		perror("\ngetfromstdin->fgets");

		free(string);

		return 0;
	}

	strp = string;

	while( isblank(*strp) )							/* Jump blanks */
		strp++;

	if( isdigit(*strp) )							/* Valid decimal number */
	{
		number = strtoul(string, NULL, BASE);		/* Convert to unsigned long */

		free(string);

		if(number == ULONG_MAX)						/* Upper limit violation */
		{
			printf("\nThe number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\nFormat error\n");

		free(string);

		return 0;
	}

	return (number < MINVALUE) ? 0 : number;
}


int getparameters(FILE *file, unsigned long *quantity, unsigned long *last)
{
	int length;				/* String length */
	char *line;				/* String to copy the first line of the file */
	char *lptr1, *lptr2;	/* Auxiliar pointers */

	length = 2 * ( 2 * (int)ceil( log10(ULONG_MAX) )
				   + strlen(STRING1) + strlen(STRING2) );

	line = (char*)calloc( length, sizeof(char) );

	if(line == NULL)
	{
		perror("\ngetparameters->calloc");

		return 0;
	}

	if( !fgets(line, length, file) )				/* Get the first line */
	{
		perror("\ngetparameters->fgets");

		free(line);

		return 0;
	}

	lptr1 = strstr(line, STRING1);					/* Search for 1st substring */

	if(lptr1 == NULL || lptr1 != line)				/* Substring missing or in */
	{												/* wrong position          */
		printf("\nFile format error\n");

		free(line);

		return 0;
	}

	lptr2 = strstr(line, STRING2);					/* Search for 2nd substring */

	if(lptr2 == NULL)								/* Substring missing */
	{
		printf("\nFile format error\n");

		free(line);

		return 0;
	}

	lptr1 += strlen(STRING1);						/* Go to expected numeric */
													/* position               */

	if( !isdigit(*lptr1) )							/* Test if there is a */
	{												/* number             */
		printf("\nFormat error\n");

		free(line);

		return 0;
	}

	*quantity = strtoul(lptr1, NULL, BASE);			/* Get number */

	if(*quantity == 0 || *quantity == ULONG_MAX)	/* Illegal quantities */
	{
		printf("\nNumber format error\n");

		free(line);

		return 0;
	}

	lptr2 += strlen(STRING2);						/* Go to 2nd expected */
													/* number             */

	if( !isdigit(*lptr2) )							/* Test 2nd number */
	{
		printf("\nFormat error\n");

		free(line);

		return 0;
	}

	*last = strtoul(lptr2, NULL, BASE);

	if(*last < MINVALUE || *last == ULONG_MAX)		/* Illegal values */
	{
		printf("\nNumber format error\n");

		free(line);

		return 0;
	}

	free(line);

	return 1;
}


int isprime(unsigned long number, unsigned long *primes)
{
	enum boolean {FALSE, TRUE};
	int answer = TRUE;
	unsigned long limit;

	limit = (unsigned long)ceil( sqrt(number) );		/* Test limit */

	while(*primes <= limit && *primes)	/* The second term tests whether the */
	{									/* end of the list has been reached  */
		if(number % *primes == 0)
		{
			answer = FALSE;

			break;
		}
		else
			primes++;					/* Next prime */
	}

	return answer;
}


int isprimextnd(unsigned long number,
				unsigned long testd, unsigned long *primes,
				unsigned long limit, unsigned long *newlist)
{
	enum boolean {FALSE, TRUE};
	int answer = TRUE;
	
	while(testd <= limit)
	{
		if( isprime(testd, primes) )		/* If the test divisor is prime */
		{
			*newlist++ = testd;				/* Add to new list */

			if(number % testd == 0)
			{
				answer = FALSE;

				break;
			}
		}

		testd++;
	}

	return answer;
}


unsigned long* loadlist(FILE *file, unsigned long quantity)
{
	char entry[ENTRYSIZE];	/* String to copy a number */
	unsigned long *list;	/* List of prime numbers */
	unsigned long *lptr;	/* Auxiliar pointer */

												/* +1 for a terminating mark */
	list = (unsigned long*)calloc( quantity + 1, sizeof(unsigned long) );

	if(list == NULL)
	{
		perror("\nloadlist->calloc");

		return NULL;
	}

	lptr = list;

	while( quantity > 0 && !feof(file) )			/* Read file lines */
	{
		if( fgets(entry, ENTRYSIZE, file) )
		{
			if( !isdigit(*entry) )					/* If it is not a number */
			{
				printf("\nEntry format error\n");

				free(list);

				return NULL;
			}

			*lptr++ = strtoul(entry, NULL, BASE);	/* Convert to unsigned long */
		}
		else if( ferror(file) )						/* Error reading file */
		{
			perror("\nloadlist->fgets");

			free(list);

			return NULL;
		}

		quantity--;
	}

	if(quantity > 0)						/* Declared quantity and number of */
	{										/* lines read do not match         */

		printf("\nFile format error: missing entries\n");

		free(list);

		return NULL;

	}

	return list;
}


int overwritefile(char *filename, unsigned long quantity, unsigned long last,
									unsigned long *list1, unsigned long *list2)
{
	FILE *file;

	file = fopen(filename, "w");

	if(file == NULL)
	{
		perror("\noverwritefile->fopen");

		return 0;
	}
												/* First line of the file */
	fprintf(file, "%s%lu%s%lu", STRING1, quantity, STRING2, last);

	while(*list1 != 0)							/* Write the numbers of 1st */
		fprintf(file, "\n%lu", *list1++);		/* list in the file         */

	while(*list2 != 0)							/* The same with 2nd list */
		fprintf(file, "\n%lu", *list2++);

	if( ferror(file) )							/* If an error occurred in */
	{											/* the process             */
		perror("\noverwritefile");

		fclose(file);

		return 0;
	}

	fclose(file);

	return 1;
}


void updateparam(unsigned long *list,
				 unsigned long *quantity, unsigned long *last)
{
	unsigned long *lptr = list;		/* Auxiliar pointer */

	while(*lptr != 0)				/* Count the valid elements of the list */
	{
		(*quantity)++;				/* Add to quantity */

		lptr++;
	}

	if(*list != 0)					/* If the list is not empty */
		*last = *--lptr;			/* save the last value      */
}