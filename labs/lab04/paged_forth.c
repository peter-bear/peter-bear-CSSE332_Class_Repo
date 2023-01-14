#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include "forth/forth_embed.h"

// if the region requested is already mapped, things fail
// so we want address that won't get used as the program
// starts up
#define STACKHEAP_MEM_START 0xf9f8c000

// the number of memory pages will will allocate to an instance of forth
#define NUM_PAGES 20

// the max number of pages we want in memort at once, ideally
#define MAX_PAGES 3


#define ACTIVE 1
#define NEVER_MAP 2
#define SWAP_OUT 3


int priority[MAX_PAGES];
int page_states[NUM_PAGES];
int active_pages[MAX_PAGES];
int fds[NUM_PAGES];
int page_num_cnt = 0;

static void handler(int sig, siginfo_t *si, void *unused)
{
    void* fault_address = si->si_addr;

    printf("in handler with invalid address %p\n", fault_address);

    int page_size = getpagesize();
    int page_num = ((void*) fault_address - (void*) STACKHEAP_MEM_START)/page_size;
    if(page_num < 0 || page_num > NUM_PAGES) {
        printf("address not within expected page!\n");
        exit(2);
    }

    // in your code you'll have to compute a particular page start and
    // map that, but in this example we can just map the same page
    // start all the time

    void* page_start_address = (void*)STACKHEAP_MEM_START + (page_size * page_num);
    if(page_num_cnt < MAX_PAGES){

        printf("mapping page %d\n", page_num);
        void* result = mmap((void*) page_start_address,
                            page_size,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS,
                            -1,
                            0);
        if(result == MAP_FAILED) {
            perror("map failed");
            exit(1);
        }

        for(int i=0;i<MAX_PAGES;i++){
            priority[i] = priority[i] - 1;
        }

        page_states[page_num] = ACTIVE;
        priority[page_num_cnt] = MAX_PAGES;
        active_pages[page_num_cnt] = page_num;

        page_num_cnt++;

    }else{
        // If we are, we will first write our oldest page onto disk, then unmap it from memory.
        int old_page;
        int old_page_index;

        // decrease priority for all
        for(int i=0;i<MAX_PAGES;i++){
            priority[i] = priority[i] - 1;
        }

        for(int i=0;i<MAX_PAGES;i++){
            if(priority[i] == 0){
                old_page = active_pages[i];
                old_page_index = i;
                break;
            }
        }

        // printf("a %d b %d c %d\n", priority[0], priority[1], priority[2]);
        
        char filename[11];
        sprintf(filename,"page_%d.dat",old_page);
        fds[old_page] = open(filename, O_RDWR | O_CREAT, S_IRWXU);
        int fd = fds[old_page];
        if(fd < 0) {
            perror("error loading linked file");
            exit(25);
        }
        
        void* unmap_page_addr = (page_size * old_page) + (void*)STACKHEAP_MEM_START;        
        write(fd, unmap_page_addr, page_size);
        

        printf("unmapping page %d\n", old_page);
        int munmap_result = munmap((void*)unmap_page_addr, page_size);
        if(munmap_result < 0) {
            perror("munmap failed");
            exit(6);
        }
        
        page_states[old_page] = SWAP_OUT;
       
        // Then we will mmap the newly required page into memory. If it’s a page we will loaded before, we will load that page from the file instead.
        int new_page_state = page_states[page_num];
        if(new_page_state == NEVER_MAP){
            printf("mapping page %d\n", page_num);
            void* result = mmap((void*) page_start_address,
                    page_size,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS,
                    -1,
                    0);
            if(result == MAP_FAILED) {
                perror("map failed");
                exit(1);
            }
            priority[old_page_index] = MAX_PAGES;
            page_states[page_num] = ACTIVE;
            active_pages[old_page_index] = page_num;
        }else if(new_page_state == SWAP_OUT){
            printf("mapping page %d\n", page_num);
            void* result = mmap((void*) page_start_address,
                    page_size,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_FIXED | MAP_SHARED,
                    fds[page_num], 0);
            if(result == MAP_FAILED) {
                perror("map failed");
                exit(1);
            }
            
            priority[old_page_index] = MAX_PAGES;
            page_states[page_num] = ACTIVE;
            active_pages[old_page_index] = page_num;
        }else {
			printf("cannot replace active page\n");
			exit(5);
		}

        close(fd);
    }


}

// init each page's state
void init_page_states(){
    for(int i=0;i<NUM_PAGES; i++){
        page_states[i] = NEVER_MAP;
        fds[i] = -1;
    }
}


int main() {

    //TODO: Add a bunch of segmentation fault handler setup here for
    //PART 1 (plus you'll also have to add the handler your self)
    init_page_states();

    static char stack[SIGSTKSZ];
    
    stack_t ss = {
                  .ss_size = SIGSTKSZ,
                  .ss_sp = stack,
    };
    
    sigaltstack(&ss, NULL);
    struct sigaction sa;
    
    // SIGINFO tells sigaction that the handler is expecting extra parameters
    // ONSTACK tells sigaction our signal handler should use the alternate stack
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("error installing handler");
        exit(3);
    }
    
    struct forth_data forth;
    char output[200];
    

    // the return stack is a forth-specific data structure if we
    // wanted to, we could give it an expanding memory segment like we
    // do for the stack/heap but I opted to keep things simple
    int returnstack_size = getpagesize() * 2;
    void* returnstack = mmap(NULL, returnstack_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_ANON | MAP_PRIVATE, -1, 0);

    
    // initializing the stack/heap to a unmapped memory pointer we
    // will map it by responding to segvs as the forth code attempts
    // to read/write memory in that space    
    int stackheap_size = getpagesize() * NUM_PAGES;

    // TODO: Modify this in PART 1
    /*
    void* stackheap = mmap(NULL, stackheap_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_ANON | MAP_PRIVATE, -1, 0);
    */

    void *stackheap = (void*) STACKHEAP_MEM_START;
    
    initialize_forth_data(&forth,
                          returnstack + returnstack_size, //beginning of returnstack
                          stackheap, //begining of heap
                          stackheap + stackheap_size); //beginning of stack

    // this code actually executes a large amount of starter forth
    // code in jonesforth.f.  If you screwed something up about
    // memory, it's likely to fail here.
    load_starter_forth_at_path(&forth, "forth/jonesforth.f");

    printf("finished loading starter forth\n");
    
    // now we can set the input to our own little forth program
    // (as a string)
    int fresult = f_run(&forth,
                        " : USESTACK BEGIN DUP 1- DUP 0= UNTIL ; " // function that puts numbers 0 to n on the stack
                        " : DROPUNTIL BEGIN DUP ROT = UNTIL ; " // funtion that pulls numbers off the stack till it finds target
                        " FOO 5000 USESTACK " // 5000 integers on the stack
                        " 2500 DROPUNTIL " // pull half off
                        " 1000 USESTACK " // then add some more back
                        " 4999 DROPUNTIL " // pull all but 2 off
                        " . . " // 4999 and 5000 should be the only ones remaining, print them out
                        " .\" finished successfully \" " // print some text */
                        ,
                        output,
                        sizeof(output));
    
    if(fresult != FCONTINUE_INPUT_DONE) {
        printf("forth did not finish executing sucessfully %d\n", fresult);
        exit(4);
    }
    printf("OUTPUT: %s\n", output);    
    printf("done\n");
    return 0;
}
