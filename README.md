# NotARook-ie
## My Attempt at a Chess Engine written in C

I started playing chess recently, so why not keep my skills in C up to date by writing a chess engine?

### Overview
This engine uses [alpha-beta pruning implemented with negamax](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning) for move selection as well as [IDDFS](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search) to speed things up and to provide a decently good move if the algorithm is interrupted before it can calculate the best possible move.
This engine compatible with xboard using the xboard protocol or other GUIs that use the <a href="http://wbec-ridderkerk.nl/html/UCIProtocol.html" target="_blank" style="text-decoration:none;">UCI protocol</a> for chess. It can also be played in command line mode.

The engine uses the same board representation as the <a href="https://www.chessprogramming.org/Cray-1" target="_blank" style="text-decoration:none;">Cray-1 supercomputer</a> where the board is 120 squares. The top and bottom have two buffer
rows, and each side of the board has one buffer row. This is to ensure that when calculating all possible moves, null moves
are accounted for and the engine doesn't just try to move a piece off of the board.

Next steps are to implement an openings book, as well as refine the evaluation functions to make the engine stronger. As of
version 1.0.1, some interesting behavior you may see is that the engine only castles if it is able to use the rook in a future
threat quickly instead of castling for king safety, as well as the engine has a tendency to (due to not having an openings book)
play into gambits that you may offer it. Another improvement for command line mode in the future is to enable support for algebraic notation
instead of playing moves in pure algebraic coordinate notation (e.g. you can type e4 instead of e2e4 or Bf3 instead of f1d3).

### Setup and Usage

#### XBoard/WinBoard setup
Here are the steps:
1. Download xboard or winboard
    * on macOS, type `brew install xboard`
    * On linux, check out [the xboard download page](https://www.gnu.org/software/xboard/#download).
    * For windows, [check here](https://www.gnu.org/software/xboard/#tag-A1). Fair warning that this engine may or may not run well (or at all, really) on windows (haven't tested it).
2. Clone the repo
3. Type `make` to build the program.
4. To play against the engine, run `xboard -cp -fcp bin/notarookie`.
    * This will start the engine with you playing the white pieces.
5. To have the engine play itself, run `xboard -mm -fcp bin/notarookie -fd "$(pwd)" -scp bin/notarookie -sd "$(pwd)"` then type `<CTRL>-t` to start engine mode.

#### Arena Chess setup (UCI protocol)
Watch [this video](https://www.youtube.com/watch?v=7WUN1dgUEmw) to see how to set up the engine to run in arena chess.

#### Command line setup
1. Clone the repo
2. Type `make` to build the program.
3. Run the program with `bin/notarookie`
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

### Acknowledgements
Shoutout to [Bluefever software](https://www.youtube.com/user/BlueFeverSoft) for providing a basic guide to the steps for implementing a chess engine over on YouTube. It really helped keep me from floundering around trying to guess on what task to do next.

Shoutout to [@sccontre](http://github.com/sccontre) for giving me the name for this engine.
