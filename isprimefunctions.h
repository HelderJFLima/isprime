/** by HelderJFLima **/

/* Header of functions used in the 'isprime' program */


/* Allocate space for an array of numbers large enough to contain all prime
** numbers between 'minimum' and 'maximum'; return a pointer to the list or
** NULL if there is an error.
*/
unsigned long* adlalloc(unsigned long minimum, unsigned long maximum);


/* Get a command-line argument and convert it to an unsigned long; return 0 if
** an error occurs or the number is out of range.
*/
unsigned long getfromarg(char *argument);


/* Get a string from stdin and convert it to an unsigned long; return 0 if an
** error occurs or the number is out of range.
*/
unsigned long getfromstdin(void);


/* Read the first line of the file with prime numbers and get the two expected
** parameters (quantity, last); save the parameter values in the informed
** variables; return 0 if an error occurs and 1 otherwise.
*/
int getparameters(FILE *file, unsigned long *quantity, unsigned long *last);


/* Check if a number is prime, based on an array of primes; return 1 if it is 
** prime and 0 otherwise; the correct answer depends on the completeness and
** consistency of the array.
*/
int isprime(unsigned long number, unsigned long *primes);


/* Check if a number is prime, based on an array of primes, and updates the
** missing prime numbers to determine the result, saving them to a new list;
** return 1 if it is prime and 0 otherwise; the correct answer depends on the
** completeness and consistency of the given array.
*/
int isprimextnd(unsigned long number,
				unsigned long testd, unsigned long *primes,
				unsigned long limit, unsigned long *newlist);


/* Read a file of numbers and save them in an allocated array with enough space
** to contain them; the last element of the array is zero to mark its end;
** returns the pointer to the vector or NULL if there is a problem.
*/
unsigned long* loadlist(FILE *file, unsigned long quantity);


/* Rewrite the prime number file with the updated values; returns 0 if an error
** occurs and 1 otherwise.
*/
int overwritefile(char *filename, unsigned long quantity, unsigned long last,
								   unsigned long *list1, unsigned long *list2);


/* Update the parameters used in the prime number file based in the contents of
** the new list.
*/
void updateparam(unsigned long *list,
				 unsigned long *quantity, unsigned long *last);