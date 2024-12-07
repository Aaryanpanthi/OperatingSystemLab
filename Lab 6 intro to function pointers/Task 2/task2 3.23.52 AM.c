#include <stdio.h>
#include <stdlib.h>

// Function prototypes
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);

int main(void) {
    // Predefined integers
    int a = 6, b = 3;

    // Array of function pointers
    int (*operations[])(int, int) = {add, subtract, multiply, divide};

    // Variable to store user input
    char input;

    // Print operands
    printf("Operand 'a' : %d | Operand 'b' : %d\n", a, b);

    // Infinite loop to handle user input
    while (1) {
        printf("Specify the operation to perform (0 : add | 1 : subtract | 2 : Multiply | 3 : divide | 4 : exit): ");
        scanf(" %c", &input);

        // Exit condition
        if (input == '4') {
            printf("Exiting the program.\n");
            break;
        }

        // Perform operation using function pointers
        int index = input - '0'; // Convert character input to an integer index
        if (index >= 0 && index < 4) {
            printf("Result: x = %d\n", operations[index](a, b));
        } else {
            printf("Invalid input. Please enter a valid choice.\n");
        }
    }

    return 0;
}

// Define operation functions
int add(int a, int b) {
    printf("Adding 'a' and 'b'\n");
    return a + b;
}

int subtract(int a, int b) {
    printf("Subtracting 'b' from 'a'\n");
    return a - b;
}

int multiply(int a, int b) {
    printf("Multiplying 'a' and 'b'\n");
    return a * b;
}

int divide(int a, int b) {
    if (b != 0) {
        printf("Dividing 'a' by 'b'\n");
        return a / b;
    } else {
        printf("Error: Division by zero.\n");
        return 0;
    }
}
