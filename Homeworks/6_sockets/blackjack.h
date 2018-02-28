

#ifndef BLACKJACK_H
#define BLACKJACK_H

typedef enum com_code {
    // SENDER - trailer data | description
    NEW,    // CLIENT - INT | Client requests new game, sends bet as an int
    DEAL,   // SERVER - INT | Server has given client a new card, card info in INT
    HIT,    // CLIENT - void | Client wished to receive a new card
    STAND,  // CLIENT - void | Client wishes not to receive any more cards
    QUIT    // CLIENT - void | exit the game & connection
} code;


#endif 