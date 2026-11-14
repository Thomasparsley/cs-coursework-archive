# CS coursework archive

Archive of old university coursework projects, kept for reference. Each project
was copied as-is from its original repository (working tree only, no history)
into its own folder. Nothing was modified, and the code is not maintained.

| Project | Language | Year | Folder |
| --- | --- | --- | --- |
| os2 | C | 2022 | `os2/` |
| dpll | C# (.NET 6) | 2022 | `dpll/` |
| dast | C# (.NET 6) and Python | 2022 | `dast/` |
| binomial-heap | Python | 2022 | `binomial-heap/` |
| b-tree | Go | 2021 | `b-tree/` |

Note on years: they are taken from the commit dates of the original
repositories. os2, dpll and dast each consist of a single "Add files via
upload" commit dated 2022-12-29, so that is a bulk upload date and the actual
work may have been done earlier in 2022 (or the autumn term of that year).
Treat those years as approximate.

## os2

- Original: https://github.com/Thomasparsley/os2
- Language: C
- Year: 2022 (single upload commit, 2022-12-29)

Three small assignments for an operating systems course, each in one C file:

- `petrzela-tomas-1-lin.c`: "pipes for threads", a bounded buffer for
  communication between threads (`pipe_create`, `pipe_write`, `pipe_read`,
  `pipe_close`, `pipe_free`) with blocking and wake-up when the buffer is
  full or empty.
- `petrzela-tomas-2-any.c`: `pgmtoascii`, a converter from PGM (P2) images,
  including comment lines, to ASCII art using a user-given character set.
  `input.pgm` is a sample image.
- `petrzela-tomas-3-lin.c`: a dotted pair allocator for Lisp
  (`lalloc` / `lfree`).

### Running tests

Tests live in `os2/tests/` (plain C with a tiny assert harness, plus a shell
script that drives `pgmtoascii` as a black box on the fixtures). The original
sources are not modified. From `os2/`:

```sh
make test            # build and run all tests
make test-sanitize   # same, with AddressSanitizer + UBSan
make test-tsan       # pipe tests with ThreadSanitizer
```

## dpll

- Original: https://github.com/Thomasparsley/dpll
- Language: C# (.NET 6, `dpll.csproj`)
- Year: 2022 (single upload commit, 2022-12-29)

A DPLL SAT solver that reads formulas in DIMACS CNF format. It has a
common DPLL core and several branching heuristics: Bohm, DLCS, DLIS, MOM and
a simple custom one ("MY"). Sample CNF instances are in `cnf/`. The included
README (in Czech) describes how to run it.

Running tests: the TUnit suite lives in `dpll.Tests/` (parsing, formula
operations, all heuristics, a seeded differential test against brute force).
Run it with `dotnet run --project dpll.Tests` (needs the .NET 9 runtime; on the
.NET 10 SDK `dotnet test` requires opting in to the new test experience, so use
`dotnet run`). The slow `aim-100-1_6-no-1.cnf` sample is skipped.

## dast

- Original: https://github.com/Thomasparsley/dast
- Language: C# (.NET 6) and Python
- Year: 2022 (single upload commit, 2022-12-29)

Credit project for a data structures course with two parts:

- `rbtree/`: a red-black tree in C# (insert, delete, find, size, colored
  text printing).
- `binomial_heap/`: a binomial heap in Python (insert, delete, union, find,
  min tree, extract min, decrease key), with optional tree printing via the
  `ete3` package.

Running tests for `binomial_heap/`: `cd dast/binomial_heap && python -m pytest`
(needs `pytest`, Python 3.10+; the `str()` printing test is skipped unless
`ete3` is installed).

Running tests for `rbtree/`: the red-black tree has a [TUnit](https://github.com/thomhurst/TUnit) test
project in `dast/rbtree.Tests/` (needs a .NET 9 SDK):

```sh
dotnet run --project dast/rbtree.Tests/rbtree.Tests.csproj
```

## binomial-heap

- Original: https://github.com/Thomasparsley/binomial-heap
- Language: Python (3.10+)
- Year: 2022 (commits dated 2022-06-03)

Standalone version of the binomial heap from the data structures course:
`BinomialHeap`, `BinomialTree` and `BinomialNode` classes with the same
operations as listed under dast.

Running tests: `cd binomial-heap && python -m pytest` (needs `pytest`,
Python 3.10+; the `str()` printing test is skipped unless `ete3` is installed).

## b-tree

- Original: https://github.com/Thomasparsley/b-tree
- Language: Go
- Year: 2021 (first commit 2021-05-10, `b-tree.go` added 2021-05-31; last
  commit 2022-03-29 only removed a leftover C# file)

A B-tree of integer keys with a configurable minimum degree: insert with node
splitting, search, find maximum key, and delete with borrowing from siblings
and node merging.

Running tests: `cd b-tree && go test ./...` (standard `testing` package,
no dependencies; includes a seeded randomized test checked against B-tree
invariants).
