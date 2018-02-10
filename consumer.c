/* PA1:    PRODUCER-CONSUMER
 * FILE:   consumer.c
 * FOLDER: PA1
 *
 * PROGRAMMER: jvj14
 * PROGRAM DESCRIPTION: Takes in data from shared memory,
 *                      output to buffer, swaps each char and then to file.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#define size 1024
#define write_PATH "output.txt"
#define readF "r+"
#define writeF "w+"

char c_temp = 0;
char *pointer;
char *output_buff;
int count = 0;
int i;
int length;
int l_Half;
int shmid;
FILE *output_file;
key_t key;
/*
 *  FUNC: int consumer()
 *  DETAILS: Takes in the same sample.txt file as authentication
 *           for shared memory space.
 *  NOTE: 1st "else if" responds if you rewrote the sample.txt file.
 *        Message should look similar to producer's first "else if"
 *        2nd "else if" responds if you run the the sample.txt unmodified.
 *        Message should look similar to producer's second "else if"
 *        IMPORTANT! ONCE YOU RUN "make runconsumer", YOU DON'T NEED TO RERUN.
 *        IN-ORDER TO TERMINATE, CTRL + C
 */
int consumer(const char *b)
{
    output_file = fopen(write_PATH, writeF); // creates an output file
    output_buff = (char*) malloc(sizeof(char)*1+1);
    if ( errno = 0, (key = ftok(b, 45 )) == -1 ) {
        printf( "ERROR: ftok() failed  errno :  %s\n", strerror( errno ) );
        exit( 1 );
    }
    /*
     * If the sample text file has been REWRITTEN and has NOT been run through this
     * program, you will be using this else if.
     */
    else if (errno = 0, (shmid = shmget( key, size, 0666 | IPC_CREAT | IPC_EXCL )) != -1 ) {
        errno = 0;
        pointer = shmat( shmid, 0, 0 );
        if ( pointer == (void *)-1 ) {
            printf( "ERROR: [1] shmat() failed  errno :  %s\n", strerror( errno ) );
            exit( 1 );
        }
        else {
            memset(pointer, 0, size); // Cleans pointer
            printf( "Process %d ACCEPTS message in created shared memory segment; attached at address %#lx.\n", getpid(), (void *)pointer );
            while(1)
            {
                if(strlen(pointer) != 0) {
                    sprintf(output_buff, "%s",pointer);
                    length = strlen(output_buff);
                    l_Half = length/2;
                    for(i =0; i<l_Half; i++) { // swapping is done here
                        c_temp = output_buff[i];
                        output_buff[i] = output_buff[length-1 - i];
                        output_buff[length-1 - i] = c_temp;
                    }
                    printf("CURRENT BUFFER CONTAINS\n\n");
                    printf("%s\n", output_buff);
                    memset(pointer, 0 ,size); // Cleans pointer
                    fprintf(output_file, "%s", output_buff);
                    sleep(4);
                    continue;
                }
                else {
                    printf("\nEMPTY in shared memory. Waiting for Producer....\n");
                    count += 1;
                    if(count == 5) { // IF LEFT "WAITING", TERMINATE.
                        fclose(output_file);
                        break;
                    }
                    sleep(2);
                }
            }
        }
    }
    /*
     * If the sample text file is PREPOPULATED with values BEFORE ever running this
     * program, you will be using this else if.
     */
    else if (errno = 0, (shmid = shmget( key, 0, 0 )) != -1 ) {
        errno = 0;
        pointer = shmat( shmid, 0, 0 );
        if ( pointer == (void *)-1 ) {
            printf( "ERROR: [2] shmat() failed  errno :  %s\n", strerror( errno ) );
            exit( 1 );
        }
        else {
            memset(pointer, 0, size); // Cleans pointer
            printf( "Process %d ACCEPTS message from shared memory segment; attached at address %#x.\n", getpid(), pointer );
            while(1)
            {
                if(strlen(pointer) != 0) {
                    sprintf(output_buff, "%s",pointer);
                    length = strlen(output_buff);
                    l_Half= length/2;
                    for(i =0; i<l_Half; i++) { // swapping is done here
                        c_temp = output_buff[i];
                        output_buff[i] = output_buff[length-1 - i];
                        output_buff[length-1 - i] = c_temp;
                    }
                    printf("CURRENT OUTPUT CONTAINS");
                    printf("\n%s\n\n", output_buff);
                    memset(pointer, 0 ,size); // Cleans pointer after use.
                    fprintf(output_file, "%s", output_buff);
                    sleep(4);
                    continue;
                }
                else {
                    printf("\nEMPTY in shared memory. Waiting for producer........\n");
                    count += 1;
                    if(count == 5) { // IF LEFT "WAITING", TERMINATE.
                        fclose(output_file);
                        break;
                    }
                    sleep(2);
                }
            }
        }
    }
    /*
     * If everything fails, this will run.
     */
    else {
        printf( "\x1b[2;31mERROR: shmget() failed  errno :  %s\x1b[0m\n", strerror( errno ) );
        exit( 1 );
    }
    shmdt(pointer);
    return 0;
}
/*
 * Simply calls the same argument file as producer, triggering consumer.
 */
int main(int argc, char const *argv[])
{
   const char *b = argv[1]; // This is the file arguement
   consumer(b);
   printf("\nCONSUMER HAS BEEN WAITING TOO LONG. ENDING........\n");
}
