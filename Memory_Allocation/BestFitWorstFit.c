#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define MAX 4000
#define MIN 8
#define LISTLENGTH 100
#define MEMORYCALLS 1000

typedef struct node{
	int size;
	struct node * next;
} node_t;

//Misses counts ammount of times when the program is out of memory.
int misses = 0;

//Allocated is total allocated memory in bytes.
long int allocated = 0;

//Random size for chunks in heap between defined MIN-MAX.
int request(){

	// k is log(MAX/MIN) 
	double k = log(((double) MAX)/MIN);

	// r is [0...k[ 
	double r = ((double)(rand()%(int) (k*10000))) / 10000;

	//size is [0..MAX[ 
	int size = (int)((double)MAX /exp(r));

	return size;
}

//Iterate through free list and print addresses and their free memory in bites.
void print_list(node_t * head){
	node_t * current = head;
	int index = 1;

	//Index is used to see ammount of blocks that were addressed.
	while (current != NULL){
		if(current->size > 0){ 
			printf("%d. Address: %p with size of: %d bytes.\n",index,current,current->size);
		}
		index++;
		current = current->next;
	} 
}

//Iterate through free list and calculates fragmentation size and total fragmentated chunks.
void print_stats(node_t * head){
	node_t * current = head;
	int ext_frag = 0;
	int ext_size = 0;

	//Fragmentated size is defined by minimum generated size.
	while (current != NULL){
		if(current->size > 0 & current->size < MIN){ 
			ext_frag++;
			ext_size += current->size;
		}
		current = current->next;
	} 
	printf("\nTotal fragmentated chunks: %d chunks.\n", ext_frag);
	printf("Total size of fragmentation: %d bytes.\n", ext_size);
}

//Adds a new chunk with a random size at the end of the free list, increases the allocated memory variable.
void push_end(node_t * head){
	volatile int size = request();
	node_t * current = head;
	
	while(current->next != NULL){
		current = current->next;
	}

	current->next = malloc(sizeof(node_t) + size);
	current->next->size = size;
	current->next->next = NULL;
	allocated += size;
}

//Adds a new chunk with a given size at the end of the free list, increases the allocated memory variable..
void push_new(node_t * head, int size){
	node_t * current = head;
	
	while(current->next != NULL){
		current = current->next;
	}

	current->next = malloc(sizeof(node_t) + size);
	current->next->size = size;
	current->next->next = NULL;
	allocated += size;
}

//Generates a chunk that can be used as head for the free list.
node_t * create_node(){
	node_t * head;
	volatile int size = request();
	head = malloc(sizeof(node_t) + size);
	allocated += size;

	head->size = size;
	head->next = NULL;

	return head;
}

//Creates a free list of defined list length with chunks of sizes between defined MIN and MAX.
node_t * create_list(){
	node_t * head = create_node();

	for(int i = 1; i < LISTLENGTH; i++){
		push_end(head);
	}

	return head;
}

//Best fit algortithm.
void bestFit(node_t * head, int fit){
	node_t * current = head;
	node_t * bestfit = head;

	//Free list iteration, finds smallest chunk that can hold given size to fit.
	while(current != NULL){
		if (current->size >= fit){
			if(bestfit->size >= current->size){
				bestfit = current;
			} else if(bestfit->size < fit){
				bestfit = current;
			}
		}

		current=current->next;
	} 
	//Reallocates the size of the smallest chunk to simulate memory being used.
	if(bestfit->size >= fit){
		realloc(bestfit, sizeof(node_t) + (bestfit->size)-fit);
		bestfit->size = (bestfit->size)-fit;
	}
	//If no large enough chunk is found, increases the out of memory counter and creates a new chunk to fit.
	else{ 
		misses++;
		//A new chunk of random size could be generated as well by using push_end(head).
		push_new(head,fit);
		bestFit(head,fit);
	}
}


//Worst fit algorithm.
void worstFit(node_t * head, int fit){
	node_t * current = head;
	node_t * worstfit = head;

	//Free list iteration, finds the biggest chunk that can hold the given size to fit.
	while (current != NULL){
		if(current->size >= fit){
			if(worstfit->size <= current->size){
				worstfit = current;
			} else if(worstfit-> size < fit){
				worstfit = current;
			}
		}
		current = current->next;
	}
	//Reallocates the size of the biggest chunk to simulate memory being used.
	if(worstfit->size >= fit){
		realloc(worstfit,sizeof(node_t) + (worstfit->size)-fit);
		worstfit->size= (worstfit->size) - fit;
	}
	//If no large enough chunk is found, increases the out of memory counter and creates a new chunk to fit.
	else {
		misses++;
		push_new(head,fit);
		worstFit(head,fit);
	}
}

//Creates a free list and iterates according to the memory call
void main(){
	node_t * head = create_list();
	node_t * current = head;

	printf("\n======== OLD FREE LIST ========\n");
	//Comment away print_list(head) to only show stats.
	print_list(head);
	printf("\nTotal allocated memory: %d bytes\n", allocated);
	printf("\n======== NEW FREE LIST ========\n");
	
	//Does the bestfit iteration over the list with given memory calls, the memory requests are between MIN - MAX.
	//Function call in for loop can be changed to do either bestFit or worstFit.
	for(int requests=0; requests < MEMORYCALLS; requests++){
		bestFit(head,request());
	}

	//Comment away print_list(head) to only show stats.
	print_list(head);
	print_stats(head);
	printf("Out of memory: %d times.\n",misses);
	printf("Total allocated memory: %d bytes\n", allocated);

	//Free the memory.
	printf("\nREMOVING LIST...");
	while(current != NULL){
		current = current->next;
		free(head);
		head = current;
	}
	if(current == NULL) printf("done! \n");
}