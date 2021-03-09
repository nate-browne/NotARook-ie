# Using the Opening Books

Each book here was created from master level games (a.k.a >= 2000 ELO).
The criteria was that each position comes from a minimum of 3 games where either white won
or black won (no draws).

Below is a list (ranked by number of moves) of all possible first opening moves based on which book you select if you let the engine use the white pieces.
This list should allow you to pick which book you want the engine to use based on what kinds of stuff you're interested in playing against.

Please note that the move selected is random based on availability of moves in the position; therefore, you cannot force
a certain variation out of the engine.

The reason I'm not listing the book possibilities if you play white is this: "1. e4" when using `Performance.bin` can be met with either "1...e5" or "1...c5" but that same move in `KomodoVariety.bin` can be met with _11_ different second moves including, but not limited to: "1...c6", "1...c5", "1...b6", and so on. Obviously this is just for that move; changing the move to "1. d4" leads to a similarly large explosion of possibilities (_14_ possible moves in `KomodoVariety.bin`).

Personally, I'd recommend trying out the `KomodoVariety.bin` book if you want a ton of possibilities or `Book.bin`/`Performance.bin` for more standard stuff. The other books are fun too, so just try them out and see what you like.

*__NOTE__: I'm not listing all openings possible in each book. 1. e4 e5 leads to the italian, the ruy lópez, the scotch, the vienna, and many more depending on black's response but isn't guaranteed since you could get 1. e4 d5 (the Scandinavian), 1. e4 c5 (the Sicilian), 1. e4 c6 (the Caro-Kann) 1. e4 e6 (the French), etc.*

### `Book.bin`/`Performance.bin`
<details>
  <summary>3 first moves</summary>

  * "1. e4" (a.k.a. king's pawn)
  * "1. d4" (a.k.a. queen's pawn)
  * "1. c4" (a.k.a. the English Opening)
</details>

### `DCbook_large.bin`
<details>
  <summary>6 first moves</summary>

  * "1. d4" (a.k.a. queen's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. e4" (a.k.a. king's pawn)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
</details>

### `varied.bin`
<details>
  <summary>6 first moves</summary>


  * "1. d4" (a.k.a. queen's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. e4" (a.k.a. king's pawn)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. c3" (a.k.a. Saragossa Opening)
</details>

### `codekiddy.bin`
<details>
  <summary>10 first moves</summary>

  * "1. e4" (a.k.a. king's pawn)
  * "1. d4" (a.k.a. queen's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
  * "1. f4" (a.k.a. Bird's Opening)
  * "1. Nc3" (a.k.a. Van Geet Opening)
  * "1. b4" (a.k.a. Orangutan Opening/Polish Opening)
  * "1. e3" (a.k.a. Van't Kruijs Opening)
</details>

### `gm2600.bin`
<details>
  <summary>10 first moves</summary>

  * "1. d4" (a.k.a. queen's pawn)
  * "1. e4" (a.k.a. king's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
  * "1. f4" (a.k.a. Bird's Opening)
  * "1. Nc3" (a.k.a. Van Geet Opening)
  * "1. b4" (a.k.a. Orangutan Opening/Polish Opening)
  * "1. c3" (a.k.a. Saragossa Opening)
</details>

### `final-book.bin`
<details>
  <summary>11 first moves</summary>

  * "1. e4" (a.k.a. king's pawn)
  * "1. d4" (a.k.a. queen's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
  * "1. f4" (a.k.a. Bird's Opening)
  * "1. Nc3" (a.k.a. Van Geet Opening)
  * "1. b4" (a.k.a. Orangutan Opening/Polish Opening)
  * "1. d3" (a.k.a. Mieses Opening)
  * "1. e3" (a.k.a. Van't Kruijs Opening)
</details>

### `Elo2400.bin`
<details>
  <summary>13 first moves</summary>

  * "1. e4" (a.k.a. king's pawn)
  * "1. d4" (a.k.a. queen's pawn)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
  * "1. f4" (a.k.a. Bird's Opening)
  * "1. Nc3" (a.k.a. Van Geet Opening)
  * "1. b4" (a.k.a. Orangutan Opening/Polish Opening)
  * "1. e3" (a.k.a. Van't Kruijs Opening)
  * "1. a3" (a.k.a. Anderssen Opening)
  * "1. c3" (a.k.a. Saragossa Opening)
  * "1. d3" (a.k.a. Mieses Opening)
</details>

### `KomodoVariety.bin`
<details>
  <summary>17 first moves</summary>

  * "1. Nc3" (a.k.a. Van Geet Opening)
  * "1. Nf3" (a.k.a. Réti Opening)
  * "1. Nh3" (a.k.a. Amar Opening)
  * "1. a3" (a.k.a. Anderssen Opening)
  * "1. a4" (a.k.a. Ware Opening)
  * "1. b3" (a.k.a. Nimzowitsch-Larsen Attack)
  * "1. b4" (a.k.a. Orangutan Opening/Polish Opening)
  * "1. c3" (a.k.a. Saragossa Opening)
  * "1. c4" (a.k.a. English Opening)
  * "1. d3" (a.k.a. Mieses Opening)
  * "1. d4" (a.k.a. queen's pawn)
  * "1. e3" (a.k.a. Van't Kruijs Opening)
  * "1. e4" (a.k.a. king's pawn)
  * "1. f4" (a.k.a. Bird's Opening)
  * "1. g3" (a.k.a. King's Fianchetto Opening)
  * "1. g4" (a.k.a. Grob Opening)
  * "1. h3" (a.k.a. Clemenz Opening)
</details>
