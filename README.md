USAGE
./main [input file] [output file]

DESCRIPTION
Returns a solution to the N-Queens problem based on the provided existing queens on the board, or "No Solution" if none.

CLASSES
Board   - Represents a chessboard and holds a vector of queens.

int size                    - The length of the chessboard.
vector<pair<int,int>> board - The total queens on the board, represented by pairs of ints.

void sort(void)             - Sorts the queens by columns, smallest to largest.
void add (pair<int,int>)    - Adds a new queen to the board.
bool check(void)            - Checks that the board has valid placements of current queens.
bool validCol(int)          - Checks that a column is available.
bool validRow(int)          - Checks that a row is available.
bool validDia(int, int)     - Checks that a diagonal is available.
void print(void)            - Prints the queens to the console (for debugging).

1. Parsing input and creating initial board
  Iterates through a line in the input file and parses and initializes the Board object with the specified size. Then, it counts the number of queens provided.
  For as many queens as the input provides, creates them by parsing the given col and row values and adds them to the board.
3. Simulating possible placements
  a.    Frontier: A Stack of Board objects.
   Initial State: The initial Board as specified by the input file (if it has valid placements).
  b. Checking States: Pull a Board from the top of the stack and check if it reaches the winning condition (N queens on the board). If so, print the pairs of columns and rows in the board to the output file and quit.
  c. Actions: Place a queen. First checks for the first available row. Then, for each available column (that is also diagonally valid), places a queen and pushes a new board containing it to the stack.  
  If the stack is exhausted without reaching a solution, then "No Solution" is printed to the output file.
