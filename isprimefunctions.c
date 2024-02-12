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
#define EXTENSION ".txt"		/* Extension of the file of factors */
#define FILENAME  "factorization_of_"	/* For the file of factors */
#define MINVALUE  7				/* Minimum value acceptable for testing */
#define STRING1   "quantity="	/* String used in the file of primes */
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


char* getfilename(char *firstpart, unsigned long number, char *extension)
{
	int size;			/* Size of an array */
	char *filename;		/* String for the file name */
	char *nstring;		/* String for number */

	size = (int)ceil( log10(number) ) + 2;	/* +2 for the terminating null     */
											/* character and to avoid overflow */

	nstring = (char*)calloc( size, sizeof(char) );

	if(nstring == NULL)
	{
		perror("\ngetfilename->calloc");

		return NULL;
	}

	if( ( sprintf(nstring, "%lu", number) ) < 0 )	/* Write the number in    */
	{												/* the string and test if */
		perror("\ngetfilename->sprintf");			/* it worked              */

		free(nstring);

		return NULL;
	}

	size += ( strlen(firstpart) + strlen(extension) );

	filename = (char*)calloc( size, sizeof(char) );

	if(filename != NULL)
	{
		strcpy(filename, firstpart);

		strcat(filename, nstring);					/* Concatenate the strings */

		strcat(filename, extension);
	}
	else
		perror("\ngetfilename->calloc");

	free(nstring);

	return filename;
}


unsigned long getfromarg(char *argument)
{
	unsigned long number;

	if( isdigit(*argument) )						/* Valid decimal number */
	{
		number = strtoul(argument, NULL, BASE);		/* Convert to unsigned long */

		if(number == ULONG_MAX)						/* Upper limit violation */
		{
			printf("\ngetfromarg->strtoul: the number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\ngetfromarg->argv: format error\n");

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
			printf("\ngetfromstdin->strtoul: the number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\ngetfromstdin->fgets: format error\n");

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
		
		printf("\ngetparameters->strstr: file format error\n");

		free(line);

		return 0;
	}

	lptr2 = strstr(line, STRING2);					/* Search for 2nd substring */

	if(lptr2 == NULL)								/* Substring missing */
	{
		
		printf("\ngetparameters->strstr: file format error\n");

		free(line);

		return 0;
	}

	lptr1 += strlen(STRING1);						/* Go to expected numeric */
													/* position               */

	if( !isdigit(*lptr1) )							/* Test if there is a */
	{												/* number             */
		
		printf("\ngetparameters->strstr: format error\n");

		free(line);

		return 0;
	}

	*quantity = strtoul(lptr1, NULL, BASE);			/* Get number */

	if(*quantity == 0 || *quantity == ULONG_MAX)	/* Illegal quantities */
	{
		printf("\ngetparameters->strtoul: number format error\n");

		free(line);

		return 0;
	}

	lptr2 += strlen(STRING2);						/* Go to 2nd expected */
													/* number             */

	if( !isdigit(*lptr2) )							/* Test 2nd number */
	{
		printf("\ngetparameters->strstr: format error\n");

		free(line);

		return 0;
	}

	*last = strtoul(lptr2, NULL, BASE);

	if(*last < MINVALUE || *last == ULONG_MAX)		/* Illegal values */
	{
		printf("\ngetparameters->strtoul: number format error\n");

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
				printf("\nloadlist->fgets: entry format error\n");

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

		printf("\nloadlist->file format error: missing entries\n");

		free(list);

		return NULL;

	}

	return list;
}


int numfactors(unsigned long number, unsigned long maxprime,
			   unsigned long *primes1, unsigned long *primes2,
			   unsigned long **factors, unsigned long **powers)
{
	unsigned long checknum;		/* To check if a factor is prime */
	unsigned long *facp;		/* Auxiliar pointers */
	unsigned long *powp;
	int size;					/* Size of arrays */

	size = 2 * (int)ceil( log10(ULONG_MAX) );			/* Always enough */

														/* Array for factors */
	*factors = (unsigned long*)calloc( size, sizeof(unsigned long) );

	if(*factors == NULL)
	{
		perror("\nnumfactors->calloc");

		return 0;
	}
														/* Array for powers */
	*powers = (unsigned long*)calloc( size, sizeof(unsigned long) );

	if(*powers == NULL)
	{
		perror("\nnumfactors->calloc");

		free(*factors);

		return 0;
	}

	facp = *factors;

	powp = *powers;

	while(number != 1 && *primes1 != 0)
	{
		if(number % *primes1 == 0)						/* If find a factor */
		{
			*facp = *primes1;							/* Copy the prime */

			while(number % *primes1 == 0)
			{
				number /= *primes1;

				(*powp)++;								/* Increase power */
			}

			facp++;										/* Next factor */

			powp++;										/* Next power */
		}

		primes1++;										/* Next prime number */
	}

	if(primes2 != NULL)
	{
		while(number != 1 && *primes2 != 0)
		{
			if(number % *primes2 == 0)
			{
				*facp = *primes2;

				while(number % *primes2 == 0)
				{
					number /= *primes2;

					(*powp)++;
				}

				facp++;

				powp++;
			}

			primes2++;
		}
	}

	if(number != 1)
	{												/* To test if it is prime */
		checknum = (unsigned long)ceil( sqrt(number) );

		if(checknum <= maxprime)					/* The number is prime */
		{
			*facp = number;

			(*powp)++;
		}
		else										/* Unknown */
		{
			printf("\nnumfactors->format error: missing prime numbers up to "
											   "the value %lu\n", checknum   );

			free(*factors);

			free(*powers);

			return -1;
		}
	}

	return 1;
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
		perror("\noverwritefile->fprintf");

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


int writefctrfile(unsigned long number,
				  unsigned long *factors, unsigned long *powers)
{
	char *filename;
	FILE *file;

	filename = getfilename(FILENAME, number, EXTENSION);

	if(!filename)
	{
		printf("\nwritefctrfile->getfilename: error naming factor file\n");

		return 0;
	}

	file = fopen(filename, "w");

	if(!file)
	{
		perror("\nwritefctrfile->fopen");

		free(filename);

		return 0;
	}

	free(filename);

	fprintf(file, "Prime factors of %lu:\n\n", number);

	while(*factors != 0)									/* Write factors */
	{
		fprintf(file, "%lu%s", *factors, (*(factors + 1) != 0) ? ", " : "\n");

		factors++;
	}

	fprintf(file, "\nRespective powers of the factors:\n\n");

	while(*powers != 0)										/* Write powers */
	{
		fprintf(file, "%lu%s", *powers, (*(powers + 1) != 0) ? ", " : "");

		powers++;
	}

	if( ferror(file) )							/* If an error occurred in */
	{											/* the process             */
		perror("\nwritefctrfile->fprintf");

		fclose(file);

		return 0;
	}

	fclose(file);

	return 1;
}