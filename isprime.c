/** by HelderJFLima **/

/*
*
* This program answers whether a number is prime or not. It is possible to enter
* the number via the command line or just call the program and wait for it to
* request it. If the number is composite, it is possible to factor it into prime
* numbers.
*
* For more information, look for the README file.
*
*/


#include <ctype.h>
#include <direct.h>				/* _getcwd */
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "isprimefunctions.h"

#define FILENAME "list_of_primes.txt"
#define GETDIR   _getcwd				/* Function to get the directory name */
#define MINVALUE 7						/* Minimum value acceptable for testing */


/* Determine whether a number is prime or not. */

int main(int argc, char *argv[])
{
	unsigned long testNum;			/* Number to test if it is prime */
	unsigned long limit;			/* Limit for testing a number */
	unsigned long quantity;			/* Quantity of prime numbers in memory */
	unsigned long last;				/* Last prime number in the list */
	unsigned long *lfactors = NULL;	/* List of factors */
	unsigned long *lpowers = NULL;	/* List of powers */
	unsigned long *listp = NULL;	/* List of primes */
	unsigned long *listad = NULL;	/* List of additional primes */
	int check;						/* To check a return value */
	int option = '0';				/* To be used by the user (yes or no) */
	int prime;						/* To be used by functions (yes or no) */
	char *dname;					/* Directory name */
	FILE *file;						/* File with prime numbers */

	if(argc == 1)
	{
		testNum = getfromstdin();						/* Get number */

		if(!testNum)									/* Error */
		{
			printf("\nThe number must be an integer "
								"from %lu to %lu\n\n", MINVALUE, ULONG_MAX - 1);

			return 1;
		}
	}
	else if(argc == 2)
	{
		testNum = getfromarg(*++argv);					/* Get number from */
														/* command-line    */

		if(!testNum)
		{
			printf("\nThe number must be an integer "
								"from %lu to %lu\n\n", MINVALUE, ULONG_MAX - 1);

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
		
		printf("\nmain->getparameters: error reading file\n\n");

		fclose(file);

		return 5;
	}

	limit = (unsigned long)ceil( sqrt(testNum) );		/* Test limit */

	if(limit <= last)			/* If it is possible to determine the result */
	{							/* with the numbers already in the list      */

		listp = loadlist(file, quantity);				/* Copy the numbers */

		if(listp == NULL)
		{
			printf("\nmain->loadlist: error loading list of prime numbers\n\n");

			fclose(file);

			return 6;
		}

		fclose(file);

		prime = isprime(testNum, listp);				/* Determine if the */
														/* number is prime  */
	}
	else
	{
		printf("\nThe available file may not have enough "
			     "prime numbers to determine the result.\n"
			   "\nDo you want this program to calculate the missing "
			     "prime numbers to reach the conclusion if necessary?\n"
			   "\nMay need to evaluate %lu numbers.\n", limit - last    );

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
				printf("\nmain->loadlist: error loading list "
										 "of prime numbers\n\n");

				return 7;
			}

			fclose(file);

			prime = isprime(testNum, listp);

			if(prime)									/* Unresolved */
			{
				listad = adlalloc(last, limit);			/* Allocate list for */
														/* extra primes      */
				if(listad == NULL)
				{
					printf("\nmain->adlalloc: error loading extra "
											 "list of prime numbers\n\n");

					free(listp);

					return 8;
				}

									/* Test if it is prime and update new list */
				prime = isprimextnd(testNum, last + 1, listp, limit, listad);

												/* Update quantity and last */
				updateparam(listad, &quantity, &last);

															/* Overwrite file */
				if( !overwritefile(FILENAME, quantity, last, listp, listad) )
				{
					printf("\nmain->overwritefile: error overwriting file\n\n");

					free(listp);

					free(listad);

					return 9;
				}
			}
		}
		else											/* If NO */
		{
			fclose(file);

			printf("\n\nProgram execution terminated by user\n\n");

			return 0;
		}
	}
														/* Output */
	printf("\n\nNumber %lu %s\n\n", testNum,
								    (prime) ? "is prime" : "is not prime");

	option = 'n';

	if(!prime)									/* If the number is composite */
	{
		printf("\nDo you want to factor %lu "
				 "into powers of prime numbers?\n\n", testNum);

		do
		{
			if( !isspace(option) )
				printf("\nAnswer (y/n): ");

			option = tolower( getchar() );
		}
		while(option != 'y' && option != 'n');

		if(option == 'n')								/* If NO */
			printf("\n\nProgram execution terminated by user\n\n");
	}

	if(option == 'y')									/* If YES */
	{													/* Factor the number */
		check = numfactors(testNum, last, listp, listad, &lfactors, &lpowers);

		if(!check)
		{
			printf("\nmain->numfactors: error when factoring number\n\n");

			free(listp);

			free(listad);

			return 10;
		}
		else if(check < 0)
		{
			printf("\nmain->numfactors: error when factoring number\n"
				   "\n\nThe list of prime numbers must reach a value "
					   "greater than or equal to\nthat stated above so that "
					   "it is possible to factor the number %lu\n\n", testNum);

			free(listp);

			free(listad);

			return 11;
		}
													/* Write the factor file */
		if( !writefctrfile(testNum, lfactors, lpowers) )
		{
			printf("\nmain->writefctrfile: error when writing "
										  "prime factors file\n\n");

			free(lfactors);

			free(lpowers);

			free(listp);

			free(listad);

			return 12;
		}

		dname = GETDIR(NULL, _MAX_DIR);					/* Get the name of the */
														/* current directory   */

		if(dname != NULL)								/* Directory location */
			printf("\n\nThe file with the prime factors was "
					   "saved in the directory:\n\n\n%s\n\n", dname);

		free(dname);
	}

	free(lfactors);

	free(lpowers);

	free(listp);

	free(listad);

	return 0;
}