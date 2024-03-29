# NotARook-ie
## My Attempt at a Chess Engine written in C

I started playing chess recently, so why not keep my skills in C up to date by writing a chess engine?

### Overview
This engine uses [alpha-beta search with null-move pruning implemented via negamax](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning) for move selection as well as [IDDFS](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search) to speed things up and to provide a decently good move if the algorithm is interrupted before it can calculate the best possible move. It also uses the [killer heuristic/history heuristic](https://en.wikipedia.org/wiki/Killer_heuristic) to help speed up the search for best moves.
This engine is compatible with xboard using the [xboard protocol](https://www.gnu.org/software/xboard/engine-intf.html) or other GUIs that use the <a href="http://wbec-ridderkerk.nl/html/UCIProtocol.html" target="_blank" style="text-decoration:none;">UCI protocol</a> for chess. It can also be played in command line mode without any need for an external GUI.

The engine uses the same board representation as the <a href="https://www.chessprogramming.org/Cray-1" target="_blank" style="text-decoration:none;">Cray-1 supercomputer</a> where the board is 120 squares. The top and bottom have two buffer
rows, and each side of the board has one buffer row. This is to ensure that when calculating all possible moves, null moves
are accounted for and the engine doesn't just try to move a piece off of the board. The code itself is full of comments explaining why things are the way that they are to hopefully help those needing inspiration for their own engines.

See the [issues tab](https://github.com/nate-browne/NotARook-ie/issues) to see what kinds of improvements are on the way in the future.

Also note that this engine comes bundled with a ton of opening books in the form of Polyglot books (found in the `books/` directory). See the `README.md` file instead of the `books/` directory for more information.

### Setup and Usage
The easiest way to use this program is to use the script by running
`scripts/run.sh`. However, instructions for manual setup are provided below. The
script assumes that you've installed xboard, so go ahead and do that by
following the first step of the instructions below.

#### XBoard/WinBoard setup
<details>

Here are the steps:
1. Download xboard or winboard
    * on macOS, type `brew install xboard`
        * After installing on macOS, open the program (`xboard &`) then open the sound options (Engine -> Sound) and change the sound program to `afplay`
        * Next, go to Engine -> Time Control and under the incremental tab set "Initial time" to 10 min and set "Increment or max (sec/move)" to 5 seconds to set up a 10 + 5 game
    * On linux, check out [the xboard download page](https://www.gnu.org/software/xboard/#download).
    * For windows, [check here](https://www.gnu.org/software/xboard/#tag-A1). Fair warning that this engine may or may not run well (or at all, really) on windows (haven't tested it).
2. Clone the repo
3. Type `make` to build the program.
4. To play against the engine, run `xboard -cp -fcp "bin/notarookie <path to openings book to use (optional)>" &`.
    * This will start the engine with you playing the white pieces.
5. To have the engine play itself, run `xboard -fcp "bin/notarookie <path to openings book to use (optional)>" -scp bin/notarookie <path to openings book to use (optional)> &`.
6. To play another engine, run the above command but change the part after `-scp` to be the path to the other engine's executable.
7. Once in xboard, you can use the "Mode" tab to switch if the engine is playing the white pieces or the black pieces.
</details>

#### Arena/PyChess (UCI protocol setup)
Watch [this video](https://www.youtube.com/watch?v=7WUN1dgUEmw) to see how to set up the engine to run in Arena chess.
For PyChess, you can [download here](https://pychess.github.io/download/).

#### Command line setup
<details>

1. Clone the repo
2. Type `make` to build the program.
3. Run the program with `bin/notarookie <path to openings book to use (optional)>`
4. Type `console` to go to command line mode
5. Set up the game
    * First, type `new` to initialize the board
    * Type `nopost` to hide the engine's thought process from you
    * You can now set the depth (how far ahead you want the engine to think), the time (how many seconds you want the engine to be able to think), or both to limit the engine's output time. If neither are set, the engine will calculate till depth 64, however long that may take (could be a while!)
        * depth is set with `depth x` where `x` is an integer
        * time is set with `time x` where `x` is an integer in seconds
        * you can type `view` to see the current settings
    * To play the white pieces, type a move. To play black, type `go` to tell the engine to play a move first, then play your move.
        * moves are entered in long algebraic notation:
            * "1. d4" is written as "d2d4"
            * "O-O" is written as "e1g1" (for white)
            * captures are written by just moving the piece to the square
            * promotion is written as "e7e8q" (for promotion to queen)
        * The engine will automatically respond to your move
    * You can print the current board state with `print`

**WARNING**: The engine in console mode is somewhat difficult to read since it displays everything in text on screen. It may help to have a board open (either physically or on chess.com/lichess.org) as well to make the move you see for better readability, but make sure that you _turn off_ engine evaluation on those online tools so that you're not cheating :)
</details>

### Known Issues
* On clang version < 13.1.6, the compiler directive
`-Wno-unused-but-set-variable` doesn't exist so the program fails to compile. If
this happens to you (or happens due to another version of some other compiler),
just comment out that directive (but leave the others on that line alone!)

### Acknowledgements

Shoutout to [@sccontre](http://github.com/sccontre) for giving me the name for this engine.
