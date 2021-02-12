# NotARook-ie
## My Attempt at a Chess Engine written in C

I started playing chess recently, so why not keep my skills in C up to date by writing a chess engine?

### Overview
This engine uses negamax with alpha-beta pruning for move selection. It's compatible with
xboard using the xboard protocol or other GUIs that use the <a href="http://wbec-ridderkerk.nl/html/UCIProtocol.html" target="_blank" style="text-decoration:none;">UCI protocol</a> for chess. It can also be played in command line mode.

The engine uses the same board representation as the <a href="https://www.chessprogramming.org/Cray-1" target="_blank" style="text-decoration:none;">Cray-1 supercomputer</a> where the board is 120 squares. The top and bottom have two buffer
rows, and each side of the board has one buffer row. This is to ensure that when calculating all possible moves, null moves
are accounted for and the engine doesn't just try to move a piece off of the board.

### Setup and Usage

#### XBoard/WinBoard setup
*coming soon!*

#### Arena Chess setup (UCI protocol)
*coming soon!*

#### Command line setup
*coming soon!*

### Acknowledgements
Shoutout to Bluefever software for providing a basic guide to the steps for implementing this engine. It really
helped keep me from floundering around trying to guess on what to do next.
