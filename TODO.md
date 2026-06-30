# TODO — Chess Engine Project

## Short-term

- [ ] **PV (Principal Variation)** — store best line from depth `d-1`, print it at each depth
- [ ] **Move ordering** — PV move first, then captures by MVV-LVA, then killers, then rest. Replaces `_swap_to_front`
- [ ] **Quiescence in-check** — when king is attacked, generate all evasions (stand-pat eval is invalid)
- [ ] **Remove `_swap_to_front`** — obsolete once PV ordering is in place

## Medium-term

- [ ] **Piece-square tables** — reward central knights, bishops on diagonals, king safety, etc.
- [ ] **Mate distance pruning** — detect forced mates early
- [ ] **Transposition table** — hash table to cache searched positions
- [ ] **Time management** — allocate time per move based on clock and remaining moves

## Long-term

- [ ] **UCI protocol** — talk to GUIs (Arena, Cute Chess, Lichess)
- [ ] **Null-move pruning** — skip search when side to move has a big advantage
- [ ] **Futility pruning** — skip unpromising moves near leaf nodes
- [ ] **Late move reductions** — reduce depth for late moves in the ordering
- [ ] **Tapered eval** — smoothly interpolate between middlegame and endgame PSTs
- [ ] **NNUE (maybe)** — neural network evaluation
