#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "transistors.h"

void initialize_wire (wire *w, int index) {
	w->nnodes = 0;
	w->nodenodes = NULL;
	w->index = index;
}

void initialize_node (node *n, int index) {
	n->type = VSS;
	n->index = index;
	n->my_wires[0] = -1;
	n->my_wires[1] = -1;
	n->my_wires[2] = -1;
}

// initialize each wire and node in the circuit

void initialize_circuit (circuit *c) {
	c->nwires = 0;
	c->nnodes = 0;
	c->maxwires = 20;
	c->maxnodes = 20;
	c->nodes = malloc (sizeof (node) * c->maxnodes);
	c->wires = malloc (sizeof (wire) * c->maxwires);
}

// get an integer representing a new node

int new_node (circuit *c, int type) {
	if (c->nnodes >= c->maxnodes) {
		fprintf (stderr, "allocating more nodes\n");
		c->maxnodes *= 2;
		node *n = malloc (sizeof (node) * c->maxnodes);
		assert (n);
		memcpy (n, c->nodes, sizeof (node) * c->nnodes);
		free (c->nodes);
		c->nodes = n;
	}
	initialize_node (&c->nodes[c->nnodes], c->nnodes);
	c->nodes[c->nnodes].type = type;
	return c->nnodes++;
}

// get an integer representing a new wire

int new_wire (circuit *c) {
	if (c->nwires >= c->maxwires) {
		fprintf (stderr, "allocating more wires\n");
		c->maxwires *= 2;
		wire *w = malloc (sizeof (wire) * c->maxwires);
		assert (w);
		memcpy (w, c->wires, sizeof (wire) * c->nwires);
		free (c->wires);
		c->wires = w;
	}
	initialize_wire (&c->wires[c->nwires], c->nwires);
	return c->nwires++;
}

// accessor functions for nodes

void set_node_value (circuit *c, int n, int val) {
	c->nodes[n].output_value = val;
}

int get_node_value (circuit *c, int n) {
	return c->nodes[n].output_value;
}

// attach node n to wire w at port port

#if 0
// for testing purposes
void insert_nodenode (nodenode **r, nodenode *p) {
	if (*r == NULL) {
		*r = p;
	} else insert_nodenode (&(*r)->next, p);
}
#else
void insert_nodenode (nodenode **r, nodenode *p) {
	p->next = *r;
	*r = p;
}
#endif

int attach_node_to_wire (circuit *c, int n, int w, int port) {
	wire	*p;
	node	*r;

	// tell the wire about the new node

	assert (w >= 0 && w < c->nwires);
	assert (n >= 0 && n < c->nnodes);
	p = &c->wires[w];
	nodenode *nn = malloc (sizeof (nodenode));
	nn->wired_to = port;
	nn->n = n;
	nn->next = NULL;
	insert_nodenode (&p->nodenodes, nn);
	p->nnodes++;

	// tell the node about the new wire

	r = &c->nodes[n];
	if (r->my_wires[port] != -1) {
		fprintf (stderr, "node %d already connected at port %d!\n", n, port);
		return 1;
	}
	if (r->my_wires[INPUT] == w 
	|| r->my_wires[OUTPUT] == w 
	|| r->my_wires[CONTROL] == w) {
		fprintf (stderr, "node %d already connected to wire %d!\n", n, w);
		return 1;
	}
	if (r->type == INPUT_NODE && port != OUTPUT) {
		fprintf (stderr, "node %d is input node, but port %d isn't output!\n", n, port);
	}
	if (r->type == OUTPUT_NODE && port != INPUT) {
		fprintf (stderr, "node %d is output node, but port %d isn't input!\n", n, port);
	}
	r->my_wires[port] = w;
	return 0;
}

int rlevel; // used to measure time (sort of)

void evaluate_node (circuit *, int);

// find the value on a wire

void evaluate_wire (circuit *c, int w) {
	wire	*r;
	int	n, b, value;

	// if we have recursed too many times, e.g. with a cycle or a huge circuit, give up 

	if (rlevel > 1000) {
		printf ("maximum recursion level reached!\n");
		return;
	}
	rlevel++;

	// get this wire

	r = &c->wires[w];

	// do we already know the value?  then we are done

	if (r->value != UNKNOWN) {
		rlevel--;
		return;
	}

	// start out believing the value is not known

	value = UNKNOWN;

	// go through all of the nodes incident on this wire

	nodenode *p;
	for (p=r->nodenodes; p; p=p->next) {

		// get this node

		n = p->n;

		// if this node could be producing output for this wire...

		if (p->wired_to == OUTPUT) {

			// evaluate the node

			evaluate_node (c, n);

			// get the value of this node

			b = c->nodes[n].output_value;

			// maybe this node is a switch that is turned off
			// or an unknown value; ignore

			if (b == SWITCH_OFF || b == UNKNOWN) continue;

			// can't have two different values on the wire at the same time

			if (value == UNKNOWN || value == b) 
				value = b;
			else {
				fprintf (stderr, "wire %d gets more than one value, %d and %d!\n", w, value, b);
				value = INVALID;
			}
		}
	}

	// whatever value we got, stick it on this wire

	r->value = value;
	rlevel--;
}
	
// find the value on the output of a node

void evaluate_node (circuit *c, int n) {
	node	*p;
	int	a, b, s;

	// see if we've gone too far

	if (rlevel > 1000) {
		fprintf (stderr, "maximum recursion level reached!\n");
		return;
	}
	rlevel++;

	// get this node

	p = &c->nodes[n];

	// do we already know the value?  then we're done
	if (p->output_value != UNKNOWN) {
		rlevel--;
		return;
	}

	// what kind of node is this?

	switch (p->type) {
	case VDD:
		p->output_value = ONE; // easy
		break;
	case VSS:
		p->output_value = ZERO; // easy
		break;
	case INPUT_NODE:  // we should already know this!
		fprintf (stderr, "unknown input!\n");
		exit (1);
	case OUTPUT_NODE:
		// an output node's value is the value on its input wire
		evaluate_wire (c, p->my_wires[INPUT]);
		p->output_value = c->wires[p->my_wires[INPUT]].value;
		break;
	case P:
		// a p-type transistor

		// get the values on the input and control wires

		evaluate_wire (c, p->my_wires[CONTROL]);
		evaluate_wire (c, p->my_wires[INPUT]);
		a = c->wires[p->my_wires[INPUT]].value;
		s = c->wires[p->my_wires[CONTROL]].value;
		if (s == 0) {
			// P switch is on
			if (a == ZERO || a == BAD_ZERO) 
				b = BAD_ZERO;
			else
				b = a;
		} else {
			b = SWITCH_OFF;
		}
		p->output_value = b;
		break;
	case N:
		// an n-type transistor

		evaluate_wire (c, p->my_wires[INPUT]);
		evaluate_wire (c, p->my_wires[CONTROL]);
		a = c->wires[p->my_wires[INPUT]].value;
		s = c->wires[p->my_wires[CONTROL]].value;
		if (s == 1) {
			// P switch is on
			if (a == ONE || a == BAD_ONE) 
				b = BAD_ONE;
			else
				b = a;
		} else {
			b = SWITCH_OFF;
		}
		p->output_value = b;
		break;
	default: fprintf (stderr, "??? node %d,%d\n", n, p->type);
		exit (1);
	}
	rlevel--;
}

// evaluate every output node in the circuit to come up with values

void evaluate_circuit (circuit *c) {
	int	i;

	rlevel = 0;
	// set all the node and wire values to UNKNOWN

	for (i=0; i<c->nnodes; i++)
		if (c->nodes[i].type != INPUT_NODE) 
			c->nodes[i].output_value = UNKNOWN;
	for (i=0; i<c->nwires; i++) c->wires[i].value = UNKNOWN;

	// find the value of each output node

	for (i=0; i<c->nnodes; i++)
		if (c->nodes[i].type == OUTPUT_NODE) evaluate_node (c, i);
}

// print the circuit in a human-readable (sort-of) form

void print_circuit (FILE *f, circuit *c) {
	int	i;
	node	*p;
	wire	*r;

	for (i=0; i<c->nnodes; i++) {
		p = &c->nodes[i];
		fprintf (f, "%d: ", i);
		switch (p->type) {
			case INPUT_NODE: fprintf (f, "I"); break;
			case OUTPUT_NODE: fprintf (f, "O"); break;
			case P: fprintf (f, "P"); break;
			case N: fprintf (f, "N"); break;
			case VDD: fprintf (f, "VDD"); break;
			case VSS: fprintf (f, "VSS"); break;
			default: fprintf (f, "???"); break;
		}
		fprintf (f, "\t%d %d %d\n", 
			p->my_wires[0], p->my_wires[1], p->my_wires[2]);
	}
	for (i=0; i<c->nwires; i++) {
		r = &c->wires[i];
		fprintf (f, "%d: ", i);
		nodenode *p;
		for (p=r->nodenodes; p; p=p->next) {
			fprintf (f, "(%d,%d) ", p->n, p->wired_to);
		}
		fprintf (f, "\n");
	}
}

// go through every possible combination of input values, showing the corresponding output values for this circuit

void print_truth_table (circuit *c) {
#define MAX_IO_NODES	20
	// find the inputs
	unsigned int pattern;
	int i;
	int input_nodes[MAX_IO_NODES], ninputs = 0, output_nodes[MAX_IO_NODES], noutputs = 0;
	for (i=0; i<c->nnodes; i++) {
		if (c->nodes[i].type == INPUT_NODE) {
			if (ninputs >= MAX_IO_NODES) {
				fprintf (stderr, "too many input nodes!\n");
				exit (1);
			}
			input_nodes[ninputs++] = i;
		} else if (c->nodes[i].type == OUTPUT_NODE) {
			if (noutputs >= MAX_IO_NODES) {
				fprintf (stderr, "too many output nodes!\n");
				exit (1);
			}
			output_nodes[noutputs++] = i;
		}
	}
	printf ("found %d input nodes, %d output nodes\n", ninputs, noutputs);
	for (i=ninputs-1; i>=0; i--) printf ("%2d ", input_nodes[i]);
	printf (" | ");
	for (i=noutputs-1; i>=0; i--) printf ("%2d ", output_nodes[i]);
	printf ("\n----------------\n");
	for (pattern=0; pattern<1<<ninputs; pattern++) {
		for (i=0; i<c->nnodes; i++)
			set_node_value (c, i, UNKNOWN);
		for (i=0; i<c->nwires; i++)
			c->wires[i].value = UNKNOWN;
		for (i=ninputs-1; i>=0; i--) {
			if (pattern & (1<<i)) {
				set_node_value (c, input_nodes[i], ONE);
				printf (" 1 ");
			} else {
				set_node_value (c, input_nodes[i], ZERO);
				printf (" 0 ");
			}
		}
		evaluate_circuit (c);
		printf ("   ");
		for (i=noutputs-1; i>=0; i--) {
			printf (" %d", get_node_value (c, output_nodes[i]));
		}
		printf ("\n");
	}
}
