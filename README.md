# Flakes

A modern chess engine in C built with bitboards, PEXT sliding attacks, and a straightforward negamax search.

## Features

- **Bitboard representation** — all pieces tracked as `u64` bitboards for fast attack lookups
- **PEXT sliding attacks** — BMI2 `_pext_u64` for instant bishop/rook/queen move generation (Haswell+)
- **Make/unmake filter** — legal move filter via board make/unmake, correct by construction for pins, en passant, and castling
- **Negamax search** — fail-soft alpha-beta with iterative deepening and quiescence
- **FEN parsing** — import/export standard FEN strings
- **Zobrist hashing** — incremental hash updates on every make/unmake
- **Material evaluation** — Larry Kaufman 2021 piece values with bishop pair, rook, and queen adjustments

## Building

Requires a C11 compiler with BMI2 support (Haswell or newer).

```bash
cmake -B build
cmake --build build
```

### Supported toolchains

| Compiler | Flags | Target |
|----------|-------|--------|
| MSVC     | `/arch:AVX2` | Windows x64 |
| GCC/Clang | `-mbmi2` | Linux/macOS x64 |

## Usage

```bash
apps/cli/cli.exe
```

The CLI accepts FEN positions and supports commands to analyze positions, display the board, and run the engine.

## Project structure

```
flakes/
├── apps/cli/          # CLI application
├── libs/
│   ├── core/          # Core chess logic
│   │   ├── src/       #   board, moves, FEN, Zobrist, legality, algebraic notation
│   │   └── include/   #   public headers
│   ├── engine/        # Search and evaluation
│   │   ├── src/       #   negamax, quiescence, evaluation, engine control
│   │   └── include/   #   public headers
│   └── utils/         # Platform helpers (bitscan, popcount, etc.)
├── CMakeLists.txt     # Root build file
└── TODO.md            # Upcoming work and known issues
```

## Technical highlights

- **Move encoding**: 32-bit `move_t` packs from/to squares, flags, moving piece, and captured piece — zero-lookup unmake
- **State stack**: caller-owned array, tracked via `st`/`base_st` pointers in `chessboard_t` — no memory allocation during search
- **Legal move cache**: per-position cache stored in the state stack avoids redundant generation
- **All core in `libs/core/`, search in `libs/engine/`**: clean separation between chess logic and search

## Roadmap

See [TODO.md](TODO.md) for planned features: PV, move ordering, transposition table, UCI protocol, and more.

## License

MIT
