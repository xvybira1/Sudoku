#include "sudoku.h"
#include <stdint.h>
#include <ctype.h>


/* ************************************************************** *
 *               Functions required by assignment                 *
 * ************************************************************** */

unsigned int power_of_two(unsigned int number) {
    if(!number) return 0;
    unsigned int result = 1;
    result <<= number;
    return result;
}


unsigned int bitset_to_number(unsigned int bitset) {
    unsigned int result = 0;
    unsigned int count = 0;
    unsigned int pos = 0;

    for (int i = 0; i < 9; ++i) {
        if (bitset & (1 << i)) {
            count++;
            pos = i + 1; 
        }
    }

    if (count == 1) {
        result = pos;
    }

    return result;
}

unsigned int number_to_bitset(unsigned int number) {
    if (number == 0) {
        return 0x1FF;
    }
    if (number == 1) {
        return 1;
    }

    return power_of_two(number - 1);
}

bool eliminate_row(unsigned int sudoku[9][9], int row_index) {
    bool is_changed = false;
    unsigned int row_bitset_value = 0;

    for (int i = 0; i < 9; i++) {
        if (bitset_to_number(sudoku[row_index][i])) {
            row_bitset_value += sudoku[row_index][i];
        }
    }
     
    
    for (int i = 0; i < 9; i++) {
        
        if (sudoku[row_index][i] == (sudoku[row_index][i] & (~row_bitset_value))) {
            continue;
        }

        if (bitset_to_number(sudoku[row_index][i])) {
            continue;
        }
        
        sudoku[row_index][i] = sudoku[row_index][i] & ~row_bitset_value;
        is_changed = true;
    }

    return is_changed;
}

bool eliminate_col(unsigned int sudoku[9][9], int col_index) {

    bool is_changed = false;
    unsigned int col_bitset_value = 0;

    for (int i = 0; i < 9; i++) {
        if (bitset_to_number(sudoku[i][col_index])) {
            col_bitset_value += sudoku[i][col_index];
        }
    }
     
    
    for (int i = 0; i < 9; i++) {
        
        if (sudoku[i][col_index] == (sudoku[i][col_index] & (~col_bitset_value))) {
            continue;
        }

        if (bitset_to_number(sudoku[i][col_index])) {
            continue;
        }

        sudoku[i][col_index] = sudoku[i][col_index] & ~col_bitset_value;
        is_changed = true;
    }

    return is_changed;
}

bool eliminate_box(unsigned int sudoku[9][9], int row_index, int col_index) {

    bool is_changed = false;
    unsigned int box_bitset_value = 0;

    for (int i = row_index; i <= row_index + 2; i++) {
        for (int j = col_index; j <= col_index + 2; j++) {
            if (bitset_to_number(sudoku[i][j])) {
                box_bitset_value += sudoku[i][j];
            }
        }
    }
    
    for (int i = row_index; i <= row_index + 2; i++) {
        for (int j = col_index; j <= col_index + 2; j++) {

            if (sudoku[i][j] == (sudoku[i][j] & (~box_bitset_value))) {
                continue;
            }
            if (bitset_to_number(sudoku[i][j])) {
                continue;
            }
            sudoku[i][j] = sudoku[i][j] & ~box_bitset_value;
            is_changed = true;
        }
    }

    return is_changed;
}

bool needs_solving(unsigned int sudoku[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!bitset_to_number(sudoku[i][j])) {
                return true;
            }
        }
    }
    return false;
}


bool check_group(unsigned int group[9]) {
    bool array_of_used_numbers[10] = { false }; 

    for (int i = 0; i < 9; i++) {
        if (group[i] == 0) {
            return false;
        }
        if (bitset_to_number(group[i]) != 0) {
            if (array_of_used_numbers[bitset_to_number(group[i])]) {
                return false;
            }
            array_of_used_numbers[bitset_to_number(group[i])] = true;
        }
    }
    return true;
}


bool is_valid(unsigned int sudoku[9][9]) {
    for (int i = 0; i < 9; i++) { //rows
        if (!check_group(sudoku[i])) {
            return false;
        }
    }

    for (int j = 0; j < 9; j++) { //columns
        unsigned int column[9];
        for (int i = 0; i < 9; i++) {
            column[i] = sudoku[i][j];
        }
        if (!check_group(column)) {
            return false;
        }
    }

    for (int m = 0; m <= 6; m += 3) { //squares
        for (int k = 0; k <= 6; k += 3) {
            unsigned int square[9];
            int index = 0;
            for (int i = m; i <= m + 2; i++) {
                for (int j = k; j <= k + 2; j++) {
                    square[index++] = sudoku[i][j];
                }
            }
            if (!check_group(square)) {
                return false;
            }
        }
    }

    return true;
}

bool solve(unsigned int sudoku[9][9]) {
    
    if (!needs_solving(sudoku)) {
        return true;
    }

    while(true) {
        bool is_changed_row = false;
        for (int i = 0; i < 9; i++) {
            is_changed_row |= eliminate_row(sudoku, i);
        }

        bool is_changed_col = false;
        for (int i = 0; i < 9; i++) {
            is_changed_col |= eliminate_col(sudoku, i);
        }

        bool is_changed_box = false;
        for (int i = 0; i <= 6; i+=3) {
            for (int j = 0; j <= 6; j+=3) {
                is_changed_box |= eliminate_box(sudoku, i, j);
            }
        }

        if(!is_valid(sudoku)) {
            return false;
        }

        if (!needs_solving(sudoku)) {
            return true;
        }

        if(!is_changed_row && !is_changed_col && !is_changed_box) {
            return false;
        }
    }
    return false;
}


bool scan_row_of_numbers(int x, int y, char loaded_char, int scan_success, unsigned int sudoku[9][9]) {
    for(int i = x; i <= y; i++) {
        for(int j = 0; j < 3; j++) {
            if(j == 0) {
                scan_success = scanf("%c", &loaded_char);
                if(!scan_success || loaded_char != '|') {
                    return false;
                }
            } else {
                scan_success = scanf(" %c", &loaded_char);
                if(!scan_success || loaded_char != '|') {
                    return false;
                }
            }
            
            for(int k = 0; k < 3; k++) {
                scan_success = scanf(" %c", &loaded_char);
                if (scan_success && loaded_char == '.') {
                    sudoku[i][3*j + k] = 0x1FF;
                    continue;
                }
                else if (!scan_success || (loaded_char < '0') || (loaded_char > '9')) { //ASCII codes for numbers 0-9
                    return false;
                }

                sudoku[i][3*j + k] = number_to_bitset(loaded_char - '0');
            }
        }
        scan_success = scanf(" %c", &loaded_char);
        if(!scan_success || loaded_char != '|') {
            return false;
        }

        scan_success = scanf("%c", &loaded_char);
        if(!scan_success || loaded_char != '\n') {
            return false;
        }
    }
    return true;
}


bool load_easy(char loaded_char, unsigned int sudoku[9][9]) {
    int scan_success = 0;
    for (int i = 0; i < 9 ; i++) {
        for (int j = 0; j < 9; j++) {
            if ((loaded_char < 48) || (loaded_char > 57)) { //ASCII codes for numbers 0-9
                return false;
            }
            sudoku[i][j] = number_to_bitset(loaded_char - '0');

            scan_success = scanf("%c", &loaded_char);
            if (i == 8 && j == 8) {
                if (loaded_char != '\n' && scan_success != EOF) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool load(unsigned int sudoku[9][9]) {
    char loaded_char;
    int scan_success = 0;

    do {
        scan_success = scanf("%c", &loaded_char);
        if (scan_success == EOF) {
            fprintf(stderr, "Unexpected end of input.\n");
            return false;
        }
    } while (loaded_char == '\n');
    
    
    if (loaded_char == '+') {
        scan_success = scanf("=======+=======+=======+%c", &loaded_char); //first row of '+' and '='
        if(!scan_success || loaded_char != '\n') {
            fprintf(stderr, "Bad input from user. Detected characters different from numbers\n");
            return false;
        }

        for (int z = 0; z <= 6; z += 3) {

            if(!(scan_row_of_numbers(z, z + 2, loaded_char, scan_success, sudoku))) {
                fprintf(stderr, "Bad input from user. Detected characters different from numbers\n");
                return false;
            }

            if (z == 6) {
                scan_success = scanf("+=======+=======+=======+%c", &loaded_char); 
                if(!scan_success || loaded_char != '\n' || scan_success == EOF) {
                    fprintf(stderr, "Bad input from user. No new line at the end of sudoku.\n");
                    return false;
                }
            } else {
                scan_success = scanf("+=======+=======+=======+%c", &loaded_char); 
                if(!scan_success || loaded_char != '\n') {
                    fprintf(stderr, "Bad input from user. Detected characters different from numbers\n");
                    return false;
                }
            }
        }
    }
    else if (loaded_char >= '0' && loaded_char <= '9') {
        if(!(load_easy(loaded_char, sudoku))) {
            fprintf(stderr, "Bad input from user. Detected characters different from numbers\n");
            return false;
        }
    } 
    else {
        fprintf(stderr, "Bad input from user. Detected characters different from numbers\n");
        return false;
    }

    return true;
}

void print(unsigned int sudoku[9][9]) {
    printf("+=======+=======+=======+\n");

    for (int i = 0; i < 9; i++) {
        
        printf("| ");
        
        for (int j = 0; j < 9; j++) {

            if(!sudoku[i][j]) {
                printf("! ");
            }
            else if (!bitset_to_number(sudoku[i][j])) {
                printf(". ");
            }
            else {
                printf("%d ", bitset_to_number(sudoku[i][j])); //value to be printed
            }
        
            if (((j + 1) % 3) == 0) {
                if (j == 8) {
                    printf("|");
                }
                else {
                    printf("| "); //after 3 numbers printed, they're splitted (talking rows)
                }
            }
        }
        printf("\n");

        if (((i + 1) % 3) == 0) {
            printf("+=======+=======+=======+\n");
        }
    }
}
