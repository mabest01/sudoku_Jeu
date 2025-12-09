# Sudoku Master

## How to Open and Build

### Option 1: CLion (Recommended)
1.  Open **CLion**.
2.  Select **Open** and choose the `sudoku_game` folder.
3.  CLion will automatically detect the `CMakeLists.txt` and configure the project.
4.  Click the **Run** button (green play icon).

### Option 2: Code::Blocks
1.  Open **Code::Blocks**.
2.  Go to **File > New > Project**.
3.  Select **Empty Project** and click **Go**.
4.  Name the project (e.g., "SudokuMaster") and save it in the `sudoku_game` folder.
5.  Right-click the project in the "Management" pane and select **Add files...**.
6.  Navigate to the `src` folder and select **ALL** `.c` and `.h` files.
7.  **Right-click the project > Build options...**
    *   Select the project name (top level) on the left.
    *   **Compiler settings > Other compiler options**:
        *   Add: `` `pkg-config --cflags gtk+-3.0` ``
    *   **Linker settings > Other linker options**:
        *   Add: `` `pkg-config --libs gtk+-3.0` ``
8.  Click **OK** and then **Build and Run**.

### Option 3: Command Line (Manual)
You can always build it manually using the provided command:
```bash
gcc -o sudoku_master.exe src/main.c src/gui.c src/auth.c src/storage.c src/sudoku.c src/sha256.c $(pkg-config --cflags --libs gtk+-3.0)
```
