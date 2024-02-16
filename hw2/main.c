#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "transistors.h"

// this main function basically just calls make_circuit to make the
// circuit, prints the circuit, then tests it with print_truth_table

extern void make_circuit (circuit *);

int main (void) {
	circuit	c;

	// a new circuit

	initialize_circuit (&c);

	// make the circuit using make_circuit.c

	make_circuit (&c);

	// print the circuit to standard output

	print_circuit (stdout, &c);

	// print the truth table (hopefully)

	print_truth_table (&c);

	// done

	return 0;
}
