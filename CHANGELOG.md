This project adheres to [semantic versioning](https://semver.org/).
# 2.0.3 (2022-05-02)
***
### Release Highlights
* Fixed compile error caused by newest varion of GCC/Clang that prevented compilation in optimized mode due to an extraneous warning flag.
# 2.0.2 (2021-02-27)
***
### Release Highlights
* Fixed a bug involving pawn move generation regarding quiet moves vs capture moves.
* Engine is better at playing bullet.
    * In these time controls, the engine will only "think" for one second maximum when it finds a book move to play as opposed to the 2-4 second "think" time when playing blitz/rapid/classical/correspondence.

# 2.0.1 (2021-02-19)
***
### Release Highlights
* Cleaned up the code and added comments. Should see slight performance boost where in
10 + 5 games the engine will get to a depth of ~7-9 before it plays a move on my 2017 MacBook Pro.
Stronger computers should get to a deeper depth.

# 2.0.0 (2021-02-18)
***
### Release Highlights
* Engine now comes with and can use Polyglot opening books.
README instructions are updated and a bunch of books come bundled with the program.

# 1.3.0 (2021-02-18)
***
### Release Highlights
* Added ability for engine to print moves in standard algebraic notation
when playing in console mode.

# 1.2.2 (2021-02-14)
***
### Release Highlights
* Fixed segfault in `evaluate.c` caused from adding king evaluation.
* Updated engine's response to the `force` command.

# 1.2.1 (2021-02-14)
***
### Release Highlights
* Fixed bug in `engine.c` regarding the `quit` variable to avoid undefined behavior.

# 1.2.0 (2021-02-14)
***
### Release Highlights
* Added null move pruning to speed up searching.
  * New null move creation/takeback functions added to `makemove.c`
  * Function defs added to `functions.h`.
  * New code used in `search.c`

# 1.1.1 (2021-02-14)
***
### Release Highlights
* Fixed bug in `evaluate.c` regarding incorrect metrics
used to evaluate king safety.
* Added bonus for keeping both bishops in the game/removing
both of the opponent's bishops.

# 1.1.0 (2021-02-14)
***
### Release Highlights
* Added material draw recognition. This recognition is for
draws that are draws no matter what, not just theoretical draws.
* Updated/added piece evaluation for queens, pawns, rooks, kings.

# 1.0.1 (2021-02-13)
***
### Release Highlights
* Added check evaluation so that being in check causes us to
increase the depth, hopefully allowing us to find a move that gets us out of check.

# 1.0.0 (2021-02-12)
***
### Release Highlights
* Finally released version 1.0.0! This build is pretty unoptimized, but will still play a decent game.
