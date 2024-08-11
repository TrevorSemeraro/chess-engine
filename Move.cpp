#include <iostream>

#include "pch.h"

namespace Board
{
    void printMove(Move move)
    {
        // number
        int initial_rank = 8 - move.initalPosition.row;
        int final_rank = 8 - move.finalPosition.row;

        // letter
        char initial_file = move.initalPosition.col + 'a';
        char final_file = move.finalPosition.col + 'a';

        std::cout << move.piece << ": " << initial_file << initial_rank << " to " << final_file << final_rank << std::endl;

        if (move.willCapture) {
            std::cout << "Capturing: " << move.captured << std::endl;
        }
        if (move.promotion != ' ') {
            std::cout << "Promotion: " << move.promotion << std::endl;
        }
    }
}