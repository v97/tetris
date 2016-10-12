*** TETRIS ***
--- Objective ---
Blocks will fall from the sky. Rotate and position them in such a way as to avoid the screen from filling up with deposited blocks.
The white box on the right will display a preview of the upcoming block, so take this into consideration.
The game starts easy, but will get faster and harder over time!

--- Controls ---
Up = initialise when appropriate
Down = drop
Left = move left
Right = move right
Z = rotate clockwise
X = rotate anticlockwise

--- Implementation ---
This game is made using Qt libraries in C++

There are two classes for implementing the game itself:

"block" takes care of the game's actual logic:
-7 block shapes, by storing information about the relative positions of each of four pieces of the tetris block relative to the center position (e.g. the center position itself is element 0 in the array, taking the value (0, 0))
-Block orientation, in a 'state' variable, which is generated in the block constructor using rand()
-Block colour, corresponding to the block 'type' (shape), also generated in the constructor
-'Settled' pieces, stored in a 20x10 array as integers from 0 to 7 - 0 corresponds to empty, and the rest are colours
-Identifying and wiping filled rows
-Moving - this is done by incrementing/decrementing the 'center' piece, which is stored in int Block::mid[2]
-Rotating - this is done by incrementing/decrementing the state variable
-Preventing the block from moving or rotating beyond the boundaries (with some simple if statements)
No other information is stored for each block - the absolute positions of pieces are calculated using getposx() and getposy()

"window" deals with the user interface, painting and timing:
-There are 3 'windows' -> 1 main game board, a preview window, and a score/level counter - this is accomplished through a qt Grid layout, with the game board set to occupy 2 rows
-Drops the blocks at an interval determined by (1000ms - (level -1)*100) - using a Qtimer object called timer, which calls the update() function through signals and slots
-Paints all pieces, using for loops and fillRect() for colour and drawRect() for outline
-Keeps score - every line filled equates to 10 points; the score updates the statistics window each time the block hits the bottom
-Keeps the level count, which is simply 1 + score/100
-Speeds up the game by decreasing the timer interval, each level (100 points)
-Stops the game if either the level exceeds 10, or there are any settled pieces in the top two rows, and invites user to try again by changing the text contents of the statistics label
-Initialises the game when the user presses the 'up' arrow key
-Recognises keystrokes and calling the appropriate functions (move/rotate + direction passed as one integer into shift())
-Some #define tags are made here for readability throughout the program