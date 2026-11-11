package main

import (
	"fmt"
	"math/rand"
	"sort"
	"testing"
)

var degrees = []int{2, 3, 4, 5, 8}

// checkInvariants validates every structural B-tree property and fails the
// test on the first violation.
func checkInvariants(t *testing.T, tr *tree) {
	t.Helper()

	if tr.root == nil {
		t.Fatal("root is nil")
	}
	if tr.root.parent != nil {
		t.Fatal("root has a parent")
	}

	leafDepth := -1
	var prev *int

	var walk func(n *node, depth int, lo, hi *int)
	walk = func(n *node, depth int, lo, hi *int) {
		t.Helper()

		if n.length > 2*tr.T-1 {
			t.Fatalf("node has too many keys: %d", n.length)
		}
		if n != tr.root && n.length < tr.T-1 {
			t.Fatalf("non-root node has too few keys: %d < %d", n.length, tr.T-1)
		}
		if n == tr.root && !n.isLeaf && n.length < 1 {
			t.Fatal("internal root has no keys")
		}

		for i := 0; i < n.length; i++ {
			k := n.keys[i]
			if i > 0 && n.keys[i-1] >= k {
				t.Fatalf("keys not strictly sorted in node: %v", n.keys[:n.length])
			}
			if lo != nil && k <= *lo {
				t.Fatalf("key %d violates lower bound %d", k, *lo)
			}
			if hi != nil && k >= *hi {
				t.Fatalf("key %d violates upper bound %d", k, *hi)
			}
		}

		if n.isLeaf {
			if leafDepth == -1 {
				leafDepth = depth
			} else if leafDepth != depth {
				t.Fatalf("leaves at different depths: %d and %d", leafDepth, depth)
			}
			for i, c := range n.childrens {
				if c != nil {
					t.Fatalf("leaf has child at slot %d", i)
				}
			}
			for i := 0; i < n.length; i++ {
				if prev != nil && *prev >= n.keys[i] {
					t.Fatal("in-order traversal not sorted")
				}
				k := n.keys[i]
				prev = &k
			}
			return
		}

		for i := 0; i <= n.length; i++ {
			if n.childrens[i] == nil {
				t.Fatalf("internal node missing child %d (keys=%d)", i, n.length)
			}
		}
		for i := n.length + 1; i < len(n.childrens); i++ {
			if n.childrens[i] != nil {
				t.Fatalf("stale child pointer in slot %d", i)
			}
		}

		for i := 0; i <= n.length; i++ {
			c := n.childrens[i]
			if c.parent != n {
				t.Fatalf("child %d has wrong parent link", i)
			}
			clo, chi := lo, hi
			if i > 0 {
				v := n.keys[i-1]
				clo = &v
			}
			if i < n.length {
				v := n.keys[i]
				chi = &v
			}
			walk(c, depth+1, clo, chi)
			// separator keys appear in order between children
			if i < n.length {
				k := n.keys[i]
				if prev != nil && *prev >= k {
					t.Fatal("in-order traversal not sorted at separator")
				}
				prev = &k
			}
		}
	}

	walk(tr.root, 0, nil, nil)
}

func mustFind(t *testing.T, tr *tree, key int) {
	t.Helper()
	n, i := tr.Search(key)
	if n == nil || i < 0 || i >= n.length || n.keys[i] != key {
		t.Fatalf("key %d not found", key)
	}
}

func mustNotFind(t *testing.T, tr *tree, key int) {
	t.Helper()
	n, i := tr.Search(key)
	if n != nil || i != -1 {
		t.Fatalf("key %d unexpectedly found", key)
	}
}

func build(t *testing.T, deg int, keys []int) *tree {
	t.Helper()
	tr := NewBtree(deg)
	for _, k := range keys {
		tr.Insert(k)
	}
	checkInvariants(t, tr)
	return tr
}

func seq(from, to int) []int {
	var s []int
	for i := from; i <= to; i++ {
		s = append(s, i)
	}
	return s
}

func rev(s []int) []int {
	r := append([]int(nil), s...)
	sort.Sort(sort.Reverse(sort.IntSlice(r)))
	return r
}

// firstRootKey returns the first key of an internal root built by ascending
// inserts of 1..n.
func firstRootKey(t *testing.T, deg, n int) int {
	t.Helper()
	tr := build(t, deg, seq(1, n))
	if tr.root.isLeaf {
		t.Fatal("root is a leaf")
	}
	return tr.root.keys[0]
}

func TestNewBtreePanicsOnSmallDegree(t *testing.T) {
	for _, d := range []int{-1, 0, 1} {
		func() {
			defer func() {
				if recover() == nil {
					t.Fatalf("T=%d: expected panic", d)
				}
			}()
			NewBtree(d)
		}()
	}
}

func TestEmptyTree(t *testing.T) {
	for _, d := range degrees {
		t.Run(fmt.Sprintf("T=%d", d), func(t *testing.T) {
			tr := NewBtree(d)
			mustNotFind(t, tr, 0)
			mustNotFind(t, tr, 42)
			if got := tr.Delete(5); got != -1 {
				t.Fatalf("Delete on empty = %d, want -1", got)
			}
			if k, i := tr.FindMaxKey(); k != -1 || i != -1 {
				t.Fatalf("FindMaxKey on empty = (%d,%d), want (-1,-1)", k, i)
			}
			checkInvariants(t, tr)
		})
	}
}

func TestInsertSearch(t *testing.T) {
	orders := map[string][]int{
		"ascending":  seq(1, 60),
		"descending": rev(seq(1, 60)),
		"shuffled":   rand.New(rand.NewSource(1)).Perm(61)[:60],
	}
	for _, d := range degrees {
		for name, keys := range orders {
			t.Run(fmt.Sprintf("T=%d/%s", d, name), func(t *testing.T) {
				tr := NewBtree(d)
				for i, k := range keys {
					tr.Insert(k)
					if i%7 == 0 {
						checkInvariants(t, tr)
					}
				}
				checkInvariants(t, tr)
				for _, k := range keys {
					mustFind(t, tr, k)
				}
				mustNotFind(t, tr, -5)
				mustNotFind(t, tr, 1000)
			})
		}
	}
}

func TestSplitGrowsHeight(t *testing.T) {
	tests := []struct {
		deg, n     int
		wantLeaf   bool
		minHeights int // levels below root
	}{
		{2, 3, true, 0}, // exactly full root, no split yet
		{2, 4, false, 1},
		{3, 5, true, 0},
		{3, 6, false, 1},
		{4, 7, true, 0},
		{4, 8, false, 1},
		{2, 20, false, 2},
	}
	for _, tc := range tests {
		t.Run(fmt.Sprintf("T=%d/n=%d", tc.deg, tc.n), func(t *testing.T) {
			tr := build(t, tc.deg, seq(1, tc.n))
			if tr.root.isLeaf != tc.wantLeaf {
				t.Fatalf("root.isLeaf = %v, want %v", tr.root.isLeaf, tc.wantLeaf)
			}
			h := 0
			for n := tr.root; !n.isLeaf; n = n.childrens[0] {
				h++
			}
			if h < tc.minHeights {
				t.Fatalf("height %d, want >= %d", h, tc.minHeights)
			}
		})
	}
}

func TestFindMaxKey(t *testing.T) {
	for _, d := range degrees {
		t.Run(fmt.Sprintf("T=%d", d), func(t *testing.T) {
			tr := NewBtree(d)
			rng := rand.New(rand.NewSource(7))
			max := -1
			for _, k := range rng.Perm(100) {
				tr.Insert(k)
				if k > max {
					max = k
				}
				got, idx := tr.FindMaxKey()
				if got != max {
					t.Fatalf("FindMaxKey = %d, want %d", got, max)
				}
				if idx < 0 {
					t.Fatalf("FindMaxKey index = %d", idx)
				}
			}
			// delete the max repeatedly
			for want := 99; want >= 0; want-- {
				if got, _ := tr.FindMaxKey(); got != want {
					t.Fatalf("FindMaxKey = %d, want %d", got, want)
				}
				if tr.Delete(want) != want {
					t.Fatalf("Delete(%d) failed", want)
				}
				checkInvariants(t, tr)
			}
			if k, i := tr.FindMaxKey(); k != -1 || i != -1 {
				t.Fatalf("FindMaxKey on drained tree = (%d,%d)", k, i)
			}
		})
	}
}

func TestDeleteMissing(t *testing.T) {
	tr := build(t, 2, seq(1, 20))
	for _, k := range []int{0, 21, 100} {
		if got := tr.Delete(k); got != -1 {
			t.Fatalf("Delete(%d) = %d, want -1", k, got)
		}
	}
	checkInvariants(t, tr)
	for _, k := range seq(1, 20) {
		mustFind(t, tr, k)
	}
}

func TestDeleteReturnsKey(t *testing.T) {
	tr := build(t, 3, seq(1, 30))
	if got := tr.Delete(17); got != 17 {
		t.Fatalf("Delete(17) = %d", got)
	}
	if got := tr.Delete(17); got != -1 {
		t.Fatalf("second Delete(17) = %d, want -1", got)
	}
}

func TestDeleteCases(t *testing.T) {
	tests := []struct {
		name  string
		deg   int
		keys  []int
		del   []int
		check func(t *testing.T, tr *tree)
	}{
		{name: "leaf with spare keys", deg: 3, keys: seq(1, 10), del: []int{1}},
		{name: "leaf at minimum triggers rebalance", deg: 2, keys: seq(1, 7), del: []int{1, 2}},
		{
			name: "internal key replaced by predecessor",
			deg:  2, keys: seq(1, 15), del: []int{firstRootKey(t, 2, 15)},
		},
		{
			name: "internal root key deleted",
			deg:  3, keys: seq(1, 30), del: []int{firstRootKey(t, 3, 30)},
		},
		{name: "borrow from left sibling", deg: 2, keys: seq(1, 10), del: []int{10, 9}},
		{name: "borrow from right sibling", deg: 2, keys: seq(1, 10), del: []int{1, 2}},
		{name: "merge with right sibling", deg: 2, keys: seq(1, 4), del: []int{1, 2}},
		{name: "merge with left sibling", deg: 2, keys: seq(1, 4), del: []int{4, 3}},
		{
			name: "cascading merge shrinks root",
			deg:  2, keys: seq(1, 8), del: seq(1, 6),
			check: func(t *testing.T, tr *tree) {
				if tr.root.length != 2 {
					t.Fatalf("expected two keys left, got %d", tr.root.length)
				}
			},
		},
		{
			name: "root shrinks to leaf",
			deg:  2, keys: seq(1, 4), del: []int{2, 3, 4},
			check: func(t *testing.T, tr *tree) {
				if !tr.root.isLeaf || tr.root.length != 1 || tr.root.keys[0] != 1 {
					t.Fatalf("unexpected root: leaf=%v len=%d", tr.root.isLeaf, tr.root.length)
				}
			},
		},
		{name: "internal node borrow from left", deg: 2, keys: seq(1, 30), del: rev(seq(20, 30))},
		{name: "internal node borrow from right", deg: 2, keys: seq(1, 30), del: seq(1, 12)},
		{name: "delete all ascending", deg: 3, keys: seq(1, 50), del: seq(1, 50)},
		{name: "delete all descending", deg: 3, keys: seq(1, 50), del: rev(seq(1, 50))},
		{name: "delete from middle outward", deg: 2, keys: seq(1, 40), del: []int{20, 21, 19, 22, 18, 23, 17, 24}},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			tr := build(t, tc.deg, tc.keys)
			deleted := map[int]bool{}
			for _, k := range tc.del {
				if got := tr.Delete(k); got != k {
					t.Fatalf("Delete(%d) = %d", k, got)
				}
				deleted[k] = true
				checkInvariants(t, tr)
			}
			for _, k := range tc.keys {
				if deleted[k] {
					mustNotFind(t, tr, k)
				} else {
					mustFind(t, tr, k)
				}
			}
			if tc.check != nil {
				tc.check(t, tr)
			}
		})
	}
}

func TestDrainAndRefill(t *testing.T) {
	for _, d := range degrees {
		t.Run(fmt.Sprintf("T=%d", d), func(t *testing.T) {
			rng := rand.New(rand.NewSource(int64(d)))
			keys := rng.Perm(80)
			tr := build(t, d, keys)

			for round := 0; round < 3; round++ {
				for _, k := range rng.Perm(80) {
					if tr.Delete(k) != k {
						t.Fatalf("round %d: Delete(%d) failed", round, k)
					}
					checkInvariants(t, tr)
				}
				if !tr.root.isLeaf || tr.root.length != 0 {
					t.Fatalf("tree not empty after drain: leaf=%v len=%d", tr.root.isLeaf, tr.root.length)
				}
				mustNotFind(t, tr, keys[0])
				if tr.Delete(keys[0]) != -1 {
					t.Fatal("Delete on drained tree should return -1")
				}
				for _, k := range rng.Perm(80) {
					tr.Insert(k)
				}
				checkInvariants(t, tr)
				for _, k := range keys {
					mustFind(t, tr, k)
				}
			}
		})
	}
}

func TestRandomizedAgainstMap(t *testing.T) {
	for _, d := range degrees {
		for seed := int64(1); seed <= 3; seed++ {
			t.Run(fmt.Sprintf("T=%d/seed=%d", d, seed), func(t *testing.T) {
				rng := rand.New(rand.NewSource(seed*100 + int64(d)))
				tr := NewBtree(d)
				ref := map[int]bool{}
				const space = 300

				for step := 0; step < 3000; step++ {
					k := rng.Intn(space)
					if rng.Intn(3) < 2 {
						// insert; duplicates are out of scope
						if !ref[k] {
							tr.Insert(k)
							ref[k] = true
						}
					} else {
						want := -1
						if ref[k] {
							want = k
							delete(ref, k)
						}
						if got := tr.Delete(k); got != want {
							t.Fatalf("step %d: Delete(%d) = %d, want %d", step, k, got, want)
						}
					}

					if step%25 == 0 {
						checkInvariants(t, tr)
					}
					probe := rng.Intn(space)
					n, _ := tr.Search(probe)
					if (n != nil) != ref[probe] {
						t.Fatalf("step %d: Search(%d) found=%v, want %v", step, probe, n != nil, ref[probe])
					}
				}
				checkInvariants(t, tr)

				max := -1
				for k := range ref {
					if k > max {
						max = k
					}
					mustFind(t, tr, k)
				}
				if got, _ := tr.FindMaxKey(); got != max {
					t.Fatalf("FindMaxKey = %d, want %d", got, max)
				}
			})
		}
	}
}
