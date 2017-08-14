/*
 * guess.c - guessing game in C
 *
 * This is written to demonstrate this language versus the same program
 * written in other languages.
 *
 * COMPILE:
 *	 gcc -o guess guess.c
 *
 * 25-Oct-2003	Brendan Gregg	Created this.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char *scorefile = "highscores_c";

int
main()
{
	int guess = -1;
	int num = 0;
	int answer;
	char buffer[256];
	char reply[256];
	char name[256];
	char *ptr;
	FILE *outfile;
	FILE *infile;

	(void) fprintf(stdout,
	    "guess.c - Guess a number between 1 and 100\n\n");

	/* Generate random number */
	srand(time(0));
	answer = rand() % 100 + 1;

	/* Play game */
	while (guess != answer) {
		num++;
		(void) fprintf(stdout, "Enter guess %d: ", num);
		(void) fgets(reply, 256, stdin);
		guess = atoi(reply);
		if (guess < answer)
			(void) fprintf(stdout, "Higher...\n");
		if (guess > answer)
			(void) fprintf(stdout, "Lower...\n");
	}
	(void) fprintf(stdout, "Correct! That took %d guesses.\n\n", num);

	/* Save high score */
	(void) fprintf(stdout, "Please enter your name: ");
	(void) fgets(name, 256, stdin);
	ptr = (char *)strchr(name, 10);
	if (*ptr == '\n')
		*ptr = 0;
	if (!(outfile = fopen(scorefile, "a"))) {
		(void) fprintf(stderr, "ERROR: Can't write to %s\n",
		    scorefile);
		exit(1);
	}
	(void) fprintf(outfile, "%s %d\n", name, num);
	(void) fclose(outfile);


	/* Print high scores */
	(void) fprintf(stdout, "\nPrevious high scores,\n");
	if (!(infile = fopen(scorefile, "r"))) {
		(void) fprintf(stderr, "ERROR: Can't read from %s\n",
		    scorefile);
		exit(2);
	}
	while (fgets(buffer, 256, infile))
		(void) fprintf(stdout, "%s", buffer);
	(void) fclose(infile);

	return (0);
}
