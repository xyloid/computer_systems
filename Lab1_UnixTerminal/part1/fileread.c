// Compile with: gcc fileread.c -o fileread
// Run with: ./fileread

#include <stdio.h>
#include <stdlib.h>

int main(){

    // A pointer to a file
    FILE* myFile;

    // We point our file pointer to a file
    // The filepath is 'relative' to where we are
    // i.e. the './' means start searching for files here.
    // So we look for our 'data.txt' that has the Red Sox data.
    myFile = fopen("./data.txt","r");

    // We should check if we successfully opened the file next.
    // Because this is a 'file pointer' we can check if the
    // operation was successful against NULL.
    if(NULL == myFile){
        fprintf(stderr,"data.txt not found, did you run data.sh?");
        fprintf(stderr,"note: stderr argument means we write to a special 'error' output stream.");
        fprintf(stderr,"Program terminating now...");
        exit(1);
    }

    // If we have successfully opened our file, we can now read each line.
    // We will read each line, until we reach the end of the file (EOF).
    char buffer[255]; // At most, each line in file can be 255 characters
    // So we will now can each line from a file(fscanf),
    // and store it as a character string(%s) in buffer each iteration.
    while(fscanf(myFile, "%s", buffer) != EOF){
        printf("Red Sox wins are: %s\n",buffer);
    }

    // Finally, we will close our file
    fclose(myFile);

    return 0;
}
