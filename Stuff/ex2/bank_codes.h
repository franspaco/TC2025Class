
#ifndef BANK_CODES_H
#define BANK_CODES_H

// The different types of operations available
typedef enum valid_operations {
    CHECK, 
    DEPOSIT, 
    WITHDRAW, 
    EXIT
} operation_t;

// The types of responses available
typedef enum valid_responses {
    OK, 
    INSUFFICIENT,
    NO_ACCOUNT,
    BYE,
    ERROR,
    CLOSED
} response_t;

#endif