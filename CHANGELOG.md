This project adheres to [semantic versioning](https://semver.org/).

# 1.2.1 (2021-02-14)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/ba30689db25fb2a4a3b8adb059fe3ed33c234bb3)
* Fixed bug in `engine.c` regarding the `quit` variable to avoid undefined behavior.

# 1.2.0 (2021-02-14)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/cb1d0d9c71bc9a9aec28fdfbef4c3e9ede5cc565)
* Added null move pruning to speed up searching.
  * New null move creation/takeback functions added to `makemove.c`
  * Function defs added to `functions.h`.
  * New code used in `search.c`

# 1.1.1 (2021-02-14)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/6e990166adf546c3c5f80efcae50561a1d47fdf7)
* Fixed bug in `evaluate.c` regarding incorrect metrics
used to evaluate king safety.
* Added bonus for keeping both bishops in the game/removing
both of the opponent's bishops.

# 1.1.0 (2021-02-14)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/3ada7d6998150a7f11a1019680a6afe86881289b)
* Added material draw recognition. This recognition is for
draws that are draws no matter what, not just theoretical draws.
* Updated/added piece evaluation for queens, pawns, rooks, kings.

# 1.0.1 (2021-02-13)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/2e00a7559df6eec1f6093c11ac9c2ed7de26d871)
* Added check evaluation so that being in check causes us to
increase the depth, hopefully allowing us to find a move that gets us out of check.

# 1.0.0 (2021-02-12)
***
### Release Highlights [build](https://github.com/nate-browne/NotARook-ie/tree/224235d81411935fbe8663e8b21eba9c97f224da)
* Finally released version 1.0.0! This build is pretty unoptimized, but will still play a decent game.
