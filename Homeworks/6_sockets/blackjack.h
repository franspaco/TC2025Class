

#ifndef BLACKJACK_H
#define BLACKJACK_H

typedef enum com_code {
    // SENDER - trailer data | description
    NEW,                // CLIENT - INT | Client requests new game, sends bet as an int
    DEAL,               // SERVER - INT | Server has given client a new card, card info in INT
    DEALER_ACTION,      // SERVER - INT | reports abouts cards the dealed received
    HIT,                // CLIENT - void | Client wished to receive a new card
    STAND,              // CLIENT - void | Client wishes not to receive any more cards
    QUIT,               // CLIENT - void | exit the game & connection
    LOST,               // CLIENT - void | Client confirming it lost the game, ends game.
    INQUIRY_DEALER,     // CLIENT - void | Used to ask server for dealer actions.
    DEALER_READY,       // SERVER - void | used to indicate dealer is ready for user input
    DEALING_FINISHED,   // SERVER - INT  | used to tell client dealing is over, in is the result of the game: -1=player lost, 0=tie, 1=player won
    GET_DEALER_CARDS,   // CLIENT - void | used to ask server to show dealer's cards
    READY_TO_SEND_CARDS,// SERVER - void | server is ready to send cards
    NEXT_CARD,          // CLIENT - void | request the next card
    DEALER_CARD,        // SERVER - INT  | used to send cards to client
    DEALER_DONE         // SERVER - void | sent when all cards have been sent
} bj_code;

int hasTrailer(bj_code code);

#define HAND_SIZE 30

typedef struct  hand_struct{
    int num;
    int cards[HAND_SIZE];
    int bet;
    int sum;
} hand_t;


#endif 