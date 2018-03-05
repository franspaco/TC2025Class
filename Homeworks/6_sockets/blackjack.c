#include <stdio.h>
#include "blackjack.h"

int hasTrailer(bj_code code){
    switch(code){
        case NEW:
        case DEAL:
        case DEALER_ACTION:
        case DEALING_FINISHED:
        case DEALER_CARD:
            return 1;
        default:
            return 0;
    }
}