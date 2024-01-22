/** by HelderJFLima **/

/*
*
* This program answers whether a number is prime or not. It is possible to enter
* the number via the command line or just call the program and wait for it to
* request it.
*
* For more information, look for the README file.
*
*/


#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "isprimefunctions.h"

#define FILENAME "list_of_primes.txt"
#define MINVALUE 7					/* Minimum value acceptable for testing */


/* Determine whether a number is prime or not. */

int main(int argc, char *argv[])
{
	unsigned long testNum;		/* Number to test if it is prime */
	unsigned long limit;		/* Limit for testing a number */
	unsigned long quantity;		/* Quantity of prime numbers in memory */
	unsigned long last;			/* Last prime number in the list */
	unsigned long *listp;		/* List of primes */
	unsigned long *listad;		/* List of additional primes */
	int answer;					/* To be used by functions (yes or no) */
	int option = '0';			/* To be used by the user (yes or no) */
	FILE *file;					/* File with prime numbers */

	if(argc == 1)
	{
		testNum = getfromstdin();						/* Get number */

		if(!testNum)									/* Error */
		{
			printf("\nThe number must be an integer ");

			printf("from %lu to %lu\n\n", MINVALUE, ULONG_MAX - 1);

			return 1;
		}
	}
	else if(argc == 2)
	{
		testNum = getfromarg(*++argv);					/* Get number from */
														/* command-line    */

		if(!testNum)
		{
			printf("\nThe number must be an integer ");

			printf("from %lu to %lu\n\n", MINVALUE, ULONG_MAX - 1);

			return 2;
		}
	}
	else
	{
		printf("\nWrong number of arguments\n\n");

		return 3;
	}

	file = fopen(FILENAME, "r");

	if(file == NULL)
	{
		perror("\nmain->fopen");

		return 4;
	}
														
	if( !getparameters(file, &quantity, &last) )		/* Read the first line */
	{													/* of the file         */
		printf("\nError reading file\n\n");

		fclose(file);

		return 5;
	}

	limit = (unsigned long)ceil( sqrt(testNum) );		/* Test limit */

	if(limit <= last)			/* If it is possible to determine the result */
	{							/* with the numbers already in the list      */

		listp = loadlist(file, quantity);				/* Copy the numbers */

		if(listp == NULL)
		{
			printf("\nError loading list of prime numbers\n\n");

			fclose(file);

			return 6;
		}

		fclose(file);

		answer = isprime(testNum, listp);				/* Determine if the */
														/* number is prime  */

		free(listp);
	}
	else
	{
		printf("\nThe available file may not have enough ");

		printf("prime numbers to determine the result.\n");

		printf("\nDo you want this program to calculate the missing ");

		printf("prime numbers to reach the conclusion if necessary?\n");

		printf("\nMay need to evaluate %lu numbers.\n", limit - last);

		do
		{
			if( !isspace(option) )						/* To avoid nonsense */
				printf("\nAnswer (y/n): ");

			option = tolower( getchar() );				/* User option */
		} 
		while(option != 'y' && option != 'n');

		if(option == 'y')								/* If YES */
		{
			listp = loadlist(file, quantity);

			if(listp == NULL)
			{
				printf("\nError loading list of prime numbers\n\n");

				return 7;
			}

			fclose(file);

			answer = isprime(testNum, listp);

			if(!answer)									/* It is not a prime */
														/* number            */
				free(listp);

			else										/* Unresolved */
			{
				listad = adlalloc(last, limit);			/* Allocate list for */
														/* extra primes      */
				if(listad == NULL)
				{
					printf("\nError loading extra list of prime numbers\n\n");

					free(listp);

					return 8;
				}

									/* Test if it is prime and update new list */
				answer = isprimextnd(testNum, last + 1, listp, limit, listad);

												/* Update quantity and last */
				updateparam(listad, &quantity, &last);

															/* Overwrite file */
				if( !overwritefile(FILENAME, quantity, last, listp, listad) )
				{
					printf("\nError overwriting file\n\n");

					free(listp);

					free(listad);

					return 9;
				}

				free(listp);

				free(listad);
			}
		}
		else											/* If NO */
		{
			fclose(file);

			printf("\nProgram execution terminated by user\n\n");

			return 0;
		}
	}
														/* Output */
	printf("\nNumber %lu %s\n\n", testNum,
								  (answer) ? "is prime" : "is not prime");

	return 0;
}