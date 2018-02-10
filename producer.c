/* PA1:    PRODUCER-CONSUMER
 * FILE:   producer.c
 * FOLDER: PA1
 *
 * PROGRAMMER:  jvj14
 * PROGRAM DESCRIPTION: Takes in file, move its contents to shared memory 
 *                      space and allows consumer to take its contents.
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
#define readF "r+"
#define writeF "w+"

char *input_buff;
char *pointer;
char readIn;
int count = 0;
int shmid;
FILE *input_file;
key_t key;
/*
 * FUNC: int producer()
 * DETAILS: Takes file argument "sample.txt" and is able to parse number of
 *          bytes to be sent to the shared memory segment.
 *          Once producer reaches the EOF. Program terminates.
 * NOTE: The sample file has ALREADY BEEN PRE-RUN through Professor Russell's
 *       "jumble" executable.
 *       Parts of the program's values can be changed. They are noted "Interchangeable".
 */
int producer(const char *b)
{
    printf("\nCURRENT INFORMATION PULLED FROM FILE\n");
    printf("%s\n\n", input_buff);
    if ( (void)(errno = 0), (key = ftok(b, 45 )) == -1 )
    {
        printf( "ERROR: ftok() failed  errno :  %s\n", strerror( errno ) );
        exit( 1 );
    }
    /*
     * If the sample text file has been REWRITTEN and has NOT been run through program,
     * you will be using this else if.
     */
    else if (errno = 0, (shmid = shmget( key, size , 0666 | IPC_CREAT | IPC_EXCL )) != -1 )
    {
        errno = 0;
        pointer = shmat( shmid, 0, 0 );
        if ( pointer == (void *)-1 ) {
            printf( "ERROR: [1] shmat() failed  errno :  %s\n", strerror( errno ) );
            exit(1);
        }
        else {
            printf( "\nProcess %d CREATES message for shared memory segment - attached at address %#lx.\n",
                   getpid(), (void *) pointer );
            memset(pointer, 0, size);
            while(1)
            {
                if(strlen(input_buff) >= size) { // // "size" is interchangeable.
                    sprintf( pointer, "%s", input_buff );
                    sleep(4);
                    break;
                }
                else {
                    printf("FULL in shared memory. Waiting for Consumer...\n");
                    sleep(2);
                }
            }
        }
        printf("\nSUCCESS!\n");
    }
    /*
     * If the sample text file is PREPOPULATED with values BEFORE ever running the
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
            printf( "Process %d FOUNDS existing message for shared memory segment - attached at address %#x.\n", getpid(), pointer );
            memset(pointer, 0, size);
            while(1)
            {
                if(strlen(input_buff) >= size) { // "size" is interchangeable.
                    sprintf( pointer, "%s", input_buff );
                    sleep(4);
                    break;
                }
                else {
                    printf("FULL in shared memory. Waiting for Consumer...\n");
                    sleep(2);
                }
            }
        }
        printf("\nSUCCESS!\n");
    }
    /*
     * If everything fails, this will run.
     */
    else {
        printf("\x1b[2;31mERROR: shmget() failed  errno :  %s\x1b[0m\n", strerror(errno));
        exit(1);
    }
    shmdt(pointer);
    return 0;
}

int main(int argc, const char *argv[])
{
    const char *b = argv[1];
    input_file = fopen(b, readF);
    input_buff = (char*) malloc(sizeof(char));
    while(1) {
        readIn = fgetc(input_file);
        if(readIn == EOF) {
            break;
        }
        else {
            input_buff[count] = readIn;
            count +=1;
            if(count == size) { // "size" is interchangeable.
                producer(b);
                count = 0;
            }
            else {
                continue;
            }
        }
    }
    printf("PRODUCER HAS REACHED EOF. ENDING.....\n");
}
