# N-Queens Solver

A C++ command-line utility designed to solve the N-Queens problem using a stack-based Depth-First Search (DFS) algorithm. The program finds a valid placement of \(N\) non-attacking queens on an \(N x N\) chessboard, starting from either an empty board or a pre-configured partial state.

## Usage
```
./main [input file] [output file]
```

## Architecture & Class Structure

### `Board` Class
Represents the chessboard configuration and encapsulates the validation logic.

#### Member Variables
* **`int size`**: The dimensions \(N x N\) of the chessboard.
* **`std::vector<std::pair<int, int>> board`**: A list of current queen placements, represented as coordinate pairs `(row, col)`.

#### Member Functions
* **`void sort(void)`**: Sorts the placed queens by column index from smallest to largest.
* **`void add(std::pair<int, int>)`**: Positions a new queen onto the board.
* **`bool check(void)`**: Verifies whether all currently placed queens are in valid, non-attacking positions.
* **`bool validCol(int col)`**: Checks if a specific column is free of other queens.
* **`bool validRow(int row)`**: Checks if a specific row is free of other queens.
* **`bool validDia(int row, int col)`**: Checks if a specific diagonal position is safe from existing queens.
* **`void print(void)`**: Debugging utility that prints the current queen coordinates directly to the console.

## Execution Flow

### 1. Input Parsing & Initialization
* The program processes the specified input file line by line.
* It initializes a `Board` object with the designated board size \(N\).
* It reads any pre-existing queen positions from the file, parses their row and column coordinates, and populates the initial board configuration using the `add()` function.

### 2. Backtracking Simulation (DFS)
* **Frontier**: An explicit `std::stack` containing `Board` objects to manage the search space.
* **Initial State**: The initial configuration parsed from the input file is pushed onto the stack (provided its existing layout contains no immediate conflicts).
* **State Evaluation**: The solver continually pops a `Board` configuration from the top of the stack. If the state contains \(N\) valid queens, it meets the winning criteria. The configuration is written to the output file, and the program terminates successfully.
* **Actions & Successor Generation**: If a state is not a solution, the algorithm attempts to place a new queen. It identifies the first available row, iterates through each column, evaluates diagonal constraints, and pushes every valid subsequent `Board` state onto the stack.

If the frontier stack is completely exhausted without satisfying the winning criteria, the program writes **"No Solution"** to the output file.
