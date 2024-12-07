#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

int main() 
{ 
    int fd1[2];  // Pipe for P1 -> P2
    int fd2[2];  // Pipe for P2 -> P1
  
    char fixed_str[] = "howard.edu"; 
    char input_str[100]; 
    char second_input[100];
    pid_t p; 
  
    if (pipe(fd1) == -1 || pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed\n"); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "Fork Failed\n"); 
        return 1; 
    } 
  
    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of fd1
        close(fd2[1]);  // Close writing end of fd2
  
        // Write input string to child (P2)
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close writing end of fd1 after sending
        
        // Wait for concatenated string from child (P2)
        char concat_str[200];
        read(fd2[0], concat_str, sizeof(concat_str));
        close(fd2[0]); // Close reading end of fd2 after receiving
  
        printf("Concatenated string from P2: %s\n", concat_str);

        // Prompt for second input
        printf("Enter another string to concatenate: ");
        scanf("%s", second_input);

        // Append second input to received string
        strcat(concat_str, second_input);

        // Print final result
        printf("Final concatenated string in P1: %s\n", concat_str);
    } 
  
    // Child process (P2)
    else
    { 
        close(fd1[1]);  // Close writing end of fd1
        close(fd2[0]);  // Close reading end of fd2
  
        // Read string from parent (P1)
        char concat_str[100];
        read(fd1[0], concat_str, sizeof(concat_str));
        close(fd1[0]); // Close reading end of fd1 after reading
  
        // Concatenate fixed string "howard.edu"
        strcat(concat_str, fixed_str);
  
        // Send concatenated string back to parent (P1)
        write(fd2[1], concat_str, strlen(concat_str) + 1); 
        close(fd2[1]); // Close writing end of fd2 after sending
  
        exit(0); 
    } 

    return 0;
}
