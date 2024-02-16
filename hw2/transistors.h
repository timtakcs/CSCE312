// we represent circuits of transistors as hypergraphs with wires connecting nodes
// each wire is represented by a distinct integer
// each node is represented by a distinct integer
// wires and nodes can have the same numbers, but there should be no mix-up because
// it is clear from the context when a number represents a wire and when a number
// represents a node

// types of nodes:
#define VSS		0	// constant 0
#define	VDD		1	// constant 1
#define P		2	// P switch
#define N		3	// N switch
#define INPUT_NODE	4	// input node
#define OUTPUT_NODE	5	// output node

// values nodes can have

#define ZERO		0	// node value is zero
#define ONE		1	// node value is one
#define BAD_ZERO	2	// bad zero
#define BAD_ONE		3	// bad one
#define SWITCH_OFF	4	// no value
#define UNKNOWN		5	// value not computed yet
#define INVALID		6	// more than one value

// These are the indices of the wires to a node.
// P and N nodes have all three; ZERO and ONE nodes have only OUTPUT wires;
// the other wires are ignored.

#define	INPUT	0
#define OUTPUT	1
#define CONTROL	2

typedef struct _wire wire;
typedef struct _node {
	int	type, index;
	int	my_wires[3];	// INPUT, OUTPUT and CONTROL
	int	output_value;	// value of output
} node;

typedef struct _nodenode {
	int	wired_to;	// INPUT, OUTPUT or CONTROL
	int	n;		// which node
	struct _nodenode *next;
} nodenode;

struct _wire {
	int	nnodes, index;
	nodenode	*nodenodes;
	int	value;	// value currently on the wire
};

typedef struct _circuit {
	int	nwires, nnodes, maxwires, maxnodes;
	wire	*wires;
	node	*nodes;
} circuit;

void initialize_circuit (circuit *c);
int new_node (circuit *c, int type);
int new_wire (circuit *c);
void set_node_value (circuit *c, int n, int val);
int get_node_value (circuit *c, int n);
int attach_node_to_wire (circuit *c, int n, int w, int port);
void evaluate_circuit (circuit *c);
void print_circuit (FILE *f, circuit *c);
void print_truth_table (circuit *c);
