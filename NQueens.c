/*
Author: Elliot Klauke
*/


#include <stack>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <chrono>
#include <cmath>
#include <queue>
#include <algorithm>
#include <climits>
#include <sstream>
#include <stdlib.h>

using namespace std;

#define ARC 0x1
#define FC 0x2
#define MRV 0x4
#define LCV 0x8

class Queen {
    public:
        int id;
        int col;
        vector<bool> domain;

        Queen() {
            id = -1;
            col = -1;
        }
        Queen(int id, int board_size) {
            // Assign the queen to a particular row
            this->id = id;
            // Assume it can be placed in any column in the row
            this->domain.resize(board_size);
            this->domain.assign(board_size, 1);
            // Initialize its col to -1 (not placed yet)
            this->col = -1;
        }
};

class Board {
    public:
        vector<Queen> board;

        // Constructor
        Board(int board_size) {
            for (int i = 0; i < board_size; i++) {
                board.push_back(Queen(i, board_size));
            }
        }
        // Place a queen at a row and column
        void place_queen(int row, int column) {
            board[row].col = column;
        }

        // Remove the queen of a row
        void remove_queen(int row) {
            board[row].col = -1;
        }

        // Print method for debugging
        void print() {
            for (int r = 0; r < board.size(); r++) {
                for (int c = 0; c < board.size(); c++) {
                    if (board[r].col == c) printf("Q ");
                    else printf(". ");
                }
                printf("\n");
            }
            printf("----------------------\n");
        }
};

// Determines if x and y are contesting each other
bool threatening(int r1, int c1, int r2, int c2) {
    // If queens are in the same column OR diagonal
    return c1 == c2 || abs(r1 - r2) == abs(c1 - c2);
}

// Checks if the current board is valid
bool is_valid(Board& b) {
    for (int q1 = 0; q1 < b.board.size(); q1++) {
        for (int q2 = 0; q2 < b.board.size(); q2++) {
            if (q1 != q2 && b.board[q1].col != -1 && b.board[q2].col != -1) {
                if (threatening(q1, b.board[q1].col, q2, b.board[q2].col)) return false;
            }
        }
    }
    return true;
}

// Get next unassigned row
int get_next_unassigned(Board& b) {
    for (int i = 0; i < b.board.size(); i++) {
        if (b.board[i].col == -1) return i;
    }
    return -1;
}

// Get the row with the smallest domain
int get_mrv(Board& b) {
    int best_row = -1;
    int min_remaining = INT_MAX;

    for (int i = 0; i < b.board.size(); ++i) {
        if (b.board[i].col == -1) {
            // For all rows, if the Queen is currently unassigned
            int count = 0;
            for (bool val : b.board[i].domain) if (val) count++;
            // Count the possible column placements and find the minimum
            if (count < min_remaining) {
                min_remaining = count;
                best_row = i;
            }
        }
    }
    return best_row;
}

// Counts the number of conflicts between a placement and all other possible placements
int count_conflicts(Board& b, int row, int col) {
    int conflicts = 0;
    // Iterate through all rows
    for (int next_row = 0; next_row < b.board.size(); next_row++) {
        // Skip if it's the current row OR the Queen is already placed
        if (next_row == row || b.board[next_row].col != -1) continue;

        // For each column in the domain of this Queen:
        for (int next_col = 0; next_col < b.board.size(); next_col++) {
            if (b.board[next_row].domain[next_col]) {
                // If the proposed queen and next queen threaten each other, increment counter
                if (threatening(row, col, next_row, next_col)) {
                    conflicts++;
                }
            }
        }
    }
    return conflicts;
}

bool forward_check(Board& b, int row) {
    int current_col = b.board[row].col;

    for (int next_row = 0; next_row < b.board.size(); ++next_row) {
        // Skip the current queen and any queens already assigned
        if (next_row == row || b.board[next_row].col != -1) continue;

        for (int next_col = 0; next_col < b.board.size(); ++next_col) {
            if (b.board[next_row].domain[next_col]) {
                // Remove column if it conflicts with the queen at (row, current_col)
                if (current_col == next_col || 
                    abs(row - next_row) == abs(current_col - next_col)) {
                    b.board[next_row].domain[next_col] = false;
                }
            }
        }

        // If domain is empty for this future queen, fail
        bool possible = false;
        for (int col = 0; col < b.board.size(); col++) {
            if (b.board[next_row].domain[col]) {
                possible = true;
                break;
            }
        }
        if (!possible) return false;
    }
    return true;
}


// Helper to check if a specific placement (row1, col1) is compatible with (row2, col2)
bool is_compatible(int r1, int c1, int r2, int c2) {
    if (c1 == c2) return false; // same column
    if (abs(r1 - r2) == abs(c1 - c2)) return false; // same diagonal
    return true;
}

// Update the domain of a Queen (and returns whether its domain was updated or not)
bool update_domain(Board& b, int r1, int r2) {
    bool updated = false;
    // Check every possible column for the first Queen
    for (int r1_col = 0; r1_col < b.board.size(); r1_col++) {
        if (b.board[r1].domain[r1_col]) { // If it's placed
            bool valid = false;
            // There must be at least 1 valid placement of the second Queen
            for (int r2_col = 0; r2_col < b.board.size(); r2_col++) {
                if (b.board[r2].domain[r2_col] && !threatening(r1, r1_col, r2, r2_col)) {
                    valid = true;
                    break;
                }
            }
            // If no such placement exists, then prune the domain of the first Queen
            if (!valid) {
                b.board[r1].domain[r1_col] = false;
                updated = true;
            }
        }
    }
    return updated;
}

/* Arc Consistency
    Represents all arcs between Queen variables as pairs of integers (the row of each Queen)
    Using a queue, all arcs are validated and their Queens' domains updated if needed
                                                                ↑
                                      valid column placements --┘

    Arc Consistency states:
    For all values v1 in domain V1,
    there must exist at least 1 value v2 in domain V2,
    such that v1 and v2 respect the constraint.

    i.e., For every placement of the first Queen,
          there must be a possible placement of the second Queen.

        FC vs. ARC
    Forward Checking: See how a placement will affect the possible placements of all other Queens
     Arc Consistency: See how the domain of one Queen will affect the domains of other Queens,
                      then see how limiting those domains will affect the domains of other Queens...
*/
bool arc_consistency(Board& b, int row) {
    queue<pair<int, int>> arcs;
    int n = b.board.size();

    // Initialize queue with arcs pointing from unassigned Queens to the parameter Queen
    for (int i = 0; i < n; i++) {
        if (i != row && b.board[i].col == -1) {
            arcs.push({i, row});
        }
    }

    while (!arcs.empty()) {
        pair<int, int> arc = arcs.front();
        arcs.pop();

        // Updates domains according to arc relationship
            // Initially, this will update the domains of all unplaced Queens on the board
            // relative to the domain of the parameter Queen
            // But as queue propagates, it will check arcs both ways
        if (update_domain(b, arc.first, arc.second)) {
            // If nothing is left in the domain after updating, then there is no solution
            int count = 0;
            for (bool val : b.board[arc.first].domain) if (val) count++;
            if (count == 0) return false;

            // Propagation: Re-add arcs pointing to the modified variable
            for (int k = 0; k < n; k++) {
                if (k != arc.first && b.board[k].col == -1) {
                    arcs.push({k, arc.first});
                }
            }
        }
    }
    return true;
}
/* Backtrack search
    Recursively searches for a solution by placing a Queen
    and calling itself with the rest of the board.

    By the time the search returns, it will have found a solution, and the board is complete (yay).
    However, if not, then the current placement of Queens is not solvable,
    and the program "backtracks" by reversing the last placement.

*/
bool backtrack_search(Board& b, int assigned_count, int flags, int& nodes_expanded) {
    nodes_expanded++;
    if (assigned_count == b.board.size()) { // Base case
        return is_valid(b);
    }
    // Checking the current board for validity is only necessary w/o FC or ARC
    if (!(flags & (FC | ARC)) && !is_valid(b)) return false;

    int row;
    if (flags & MRV) {
        // Minimum Remaining Values:
        //  Pick the variable with the smallest domain to place next
        //             ↑                          ↑
        //             |                          └ possible column placements
        //             └------ Queen

        row = get_mrv(b);
    } else {
        // Default behavior: Get the numerically next available row
        row = get_next_unassigned(b);
    }
    if (row == -1) return false;

    vector<pair<int, int>> col_conflicts;
    if (flags & LCV) {
        // Least Constrained Variable:
        //  Pick the value that limits the domains of other variables the least
        //              ↑
        //            column

        //     (i.e., leaves the most number of open placements on the board
        //                 = least conflicts with other placements)

        //  vector<pair<int, int>> col_conflicts;
        //               ↑    ↑
        //               |    └ col
        //               └- # conflicts

        // For every possible placement in the row, count the number of conflicts
        for (int col = 0; col < b.board.size(); col++) {
            if (b.board[row].domain[col]) {
                col_conflicts.push_back(make_pair(count_conflicts(b, row, col), col));
            }
        }

        // Sort by smallest # conflicts first
        sort(col_conflicts.begin(), col_conflicts.end());
    } else {
        // Default behavior: Go through columns in numeric order
        for (int col = 0; col < b.board.size(); col++) {
            col_conflicts.push_back(make_pair(0, col));
        }
    }

    for (auto& value : col_conflicts) {
        int col = value.second;

        // (For non-LCV) Skip columns that aren't in domain
        if (!b.board[row].domain[col]) continue;

        // Save the full state of the board (including all queen domains)
        vector<Queen> board_copy = b.board;

        b.place_queen(row, col);

        b.board[row].domain.assign(b.board.size(), false);
        b.board[row].domain[col] = true;

        // Pruning
        bool valid = true;
        if (flags & ARC) {
            // Check for arc consistency
            valid = arc_consistency(b, row);
        } else if (flags & FC) {
            // Do forward checking
            valid = forward_check(b, row);
        }

        if (valid) {
            if (backtrack_search(b, assigned_count + 1, flags, nodes_expanded)) return true;
        }
        // If no solution found
        b.board = board_copy;
    }

    return false;
}

int main(int argc, char* argv[]) {
  // Getting input/output streams
  fstream inputf(argv[1]);
  ofstream outputf(argv[2]);
  int flags;
  if (argv[3] != NULL) flags = stoi(argv[3], nullptr, 2);
  else flags = 0x0;
  string line;

  // Checking version
  printf("Using:\n");
  if (flags & MRV) printf("MRV\n");
  if (flags & LCV) printf("LCV\n");
  if (flags & FC) printf("Forward-Checking\n");
  if (flags & ARC) printf("Arc Consistency\n");

  while (getline(inputf, line)) {
        // Timer
        auto start = std::chrono::high_resolution_clock::now();
        int nodesChecked = 0;

        // Parsing size
        stringstream ss(line);
        int size;
        if (!(ss >> size)) continue;
        printf("Size: %d\n", size);

        // Create game
        Board game(size);

        // Initialize pre-placed Queens
        int row, col;
        int num_queens = 0;
        while (ss >> row >> col) {
            num_queens++;
            // Translate to 0 indexing
            row--;
            col--;
            printf("Placing: %d, %d\n", row, col);

            // Place queens and then manually update domains of placed queen and other queens
            game.place_queen(row, col);
            game.board[row].domain.assign(size, false);
            game.board[row].domain[col] = true;

            // Fixing domains
            if (flags & ARC) arc_consistency(game, row);
            else if (flags & FC) forward_check(game, row);
        }

        // Begin solution search
        if (backtrack_search(game, num_queens, flags, nodesChecked)) {
            for (int i = 0; i < size; i++) {
            outputf << i + 1 << " " << game.board[i].col + 1 << " ";
            printf("%d %d ", i, game.board[i].col);
            }
            cout << endl;;
            game.print(); // Debugging
        } else {
          outputf << "No solution";
          printf("No solution\n");
        }
        outputf << endl;
        printf("\n");
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        printf("Execution time: %f ms\n", duration.count());
        printf(" Nodes visited: %d\n", nodesChecked);
        printf("---------------\n");
  }
  return 0;
}
