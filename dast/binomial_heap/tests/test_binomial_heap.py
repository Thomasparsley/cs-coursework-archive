import heapq
import random

import pytest

from binomial_heap import BinomialHeap, BinomialNode, BinomialTree


def make_heap(keys):
    heap = BinomialHeap()
    nodes = [BinomialNode(k) for k in keys]
    for node in nodes:
        heap.insert(node)
    return heap, nodes


def all_nodes(heap):
    out = []
    for tree in heap.heap:
        stack = [tree.root]
        while stack:
            n = stack.pop()
            out.append(n)
            stack.extend(n.childrens)
    return out


def keys_of(heap):
    return sorted(n.key for n in all_nodes(heap))


def check_invariants(heap):
    degrees = [t.degree for t in heap.heap]
    assert len(degrees) == len(set(degrees)), "duplicate root degrees"
    for tree in heap.heap:
        assert tree.root.parent is None

        def walk(node):
            size = 1
            for child in node.childrens:
                assert child.parent is node
                assert node.key <= child.key, "heap order violated"
                size += walk(child)
            return size

        assert walk(tree.root) == 2 ** tree.degree
        assert len(tree.root.childrens) == tree.degree


def drain(heap):
    out = []
    while not heap.is_empty:
        out.append(heap.extract_min().key)
        check_invariants(heap)
    return out


def test_empty_heap():
    heap = BinomialHeap()
    assert heap.is_empty
    assert heap.min_tree() is None
    assert heap.extract_min() is None
    assert heap.delete(BinomialNode(1)) is None
    assert heap.find_node(BinomialNode(1)) is None
    heap.decrease_key(BinomialNode(1), 0)  # no-op
    assert heap.is_empty
    assert heap.newick_strings() == []


def test_default_arg_not_shared():
    a, b = BinomialHeap(), BinomialHeap()
    a.insert(BinomialNode(1))
    assert b.is_empty


def test_insert_and_min():
    heap, _ = make_heap([5, 4, 3, 6, 8, 9])
    check_invariants(heap)
    assert heap.min_tree().root.key == 3
    assert [t.degree for t in heap.heap] == [1, 2]  # 6 = 0b110


@pytest.mark.parametrize("n", [1, 2, 3, 7, 8, 15, 16, 33])
def test_structure_matches_binary_size(n):
    heap, _ = make_heap(list(range(n, 0, -1)))
    check_invariants(heap)
    assert [t.degree for t in heap.heap] == [i for i in range(n.bit_length()) if n >> i & 1]


def test_extract_min_order():
    keys = [20, 50, 11, 7, 45, 12, 9, 3, 13, 15, 10, 5, 6, 55, 3, 7]
    heap, _ = make_heap(keys)
    assert drain(heap) == sorted(keys)
    assert heap.extract_min() is None


def test_extract_min_node_detached():
    heap, _ = make_heap(range(16))
    node = heap.extract_min()
    assert node.key == 0
    assert node.childrens == []
    assert node.parent is None
    assert keys_of(heap) == list(range(1, 16))
    for n in all_nodes(heap):
        assert n is not node


def test_union_result_and_operands_emptied():
    h1, _ = make_heap([5, 4, 3, 6, 8, 9])
    h2, _ = make_heap([1, 10, 7, 2, 22, 33, 11, 44])
    h3 = h1.union(h2)
    check_invariants(h3)
    assert h1.is_empty and h2.is_empty
    assert drain(h3) == sorted([5, 4, 3, 6, 8, 9, 1, 10, 7, 2, 22, 33, 11, 44])


def test_union_with_empty_and_self():
    h, _ = make_heap([3, 1, 2])
    result = h.union(BinomialHeap())
    check_invariants(result)
    assert keys_of(result) == [1, 2, 3]
    result2 = BinomialHeap().union(result)
    assert keys_of(result2) == [1, 2, 3]
    result3 = result2.union(result2)
    assert keys_of(result3) == [1, 2, 3]
    check_invariants(result3)


@pytest.mark.parametrize("a", range(0, 12))
@pytest.mark.parametrize("b", [0, 1, 3, 7, 8, 11])
def test_union_sizes(a, b):
    h1, _ = make_heap(range(0, 2 * a, 2))
    h2, _ = make_heap(range(1, 2 * b, 2))
    u = h1.union(h2)
    check_invariants(u)
    assert keys_of(u) == sorted(list(range(0, 2 * a, 2)) + list(range(1, 2 * b, 2)))


def test_decrease_key_keeps_handle_key():
    heap, nodes = make_heap([10, 20, 30, 40, 50, 60, 70, 80])
    target = nodes[6]  # key 70
    heap.decrease_key(target, 1)
    assert target.key == 1
    for node, orig in zip(nodes, [10, 20, 30, 40, 50, 60, 70, 80]):
        if node is not target:
            assert node.key == orig  # other handles untouched
    check_invariants(heap)
    assert heap.min_tree().root is target
    assert heap.extract_min() is target


def test_decrease_key_not_in_heap_noop():
    heap, _ = make_heap([5, 6, 7])
    heap.decrease_key(BinomialNode(99), 1)
    assert keys_of(heap) == [5, 6, 7]


def test_decrease_key_requires_smaller():
    heap, nodes = make_heap([5, 6, 7])
    with pytest.raises(AssertionError):
        heap.decrease_key(nodes[0], 5)


def test_delete_existing():
    heap, nodes = make_heap(range(1, 15))
    removed = heap.delete(nodes[6])
    assert removed is nodes[6]
    assert removed.childrens == []
    check_invariants(heap)
    assert keys_of(heap) == [k for k in range(1, 15) if k != 7]


@pytest.mark.xfail(
    reason="bug: delete() sets key to -inf to bubble the node up and never "
    "restores it, so the returned node has key -inf instead of its original key",
    strict=True,
)
def test_delete_returned_node_keeps_key():
    heap, nodes = make_heap([4, 2, 9])
    assert heap.delete(nodes[2]).key == 9


def test_delete_by_equal_key():
    heap, nodes = make_heap([4, 2, 9])
    removed = heap.delete(BinomialNode(9))
    assert removed is nodes[2]
    assert keys_of(heap) == [2, 4]


def test_delete_absent_returns_none_heap_unchanged():
    heap, _ = make_heap([4, 2, 9, 1, 5])
    before = heap.newick_strings()
    assert heap.delete(BinomialNode(1234)) is None
    assert heap.newick_strings() == before
    check_invariants(heap)


def test_delete_all_in_given_order():
    keys = [20, 50, 11, 7, 45, 12, 9, 3, 13, 15, 10, 5, 6, 55]
    heap, _ = make_heap(keys)
    for k in [11, 20, 10, 9, 15, 7, 6, 5, 13, 12, 50, 45, 3, 55]:
        assert heap.delete(BinomialNode(k)) is not None
        check_invariants(heap)
    assert heap.is_empty


def test_find_node_prefers_identity():
    heap, nodes = make_heap([3, 3, 3, 3])
    for n in nodes:
        assert heap.find_node(n) is n


def test_newick_strings():
    heap, _ = make_heap([1])
    assert heap.newick_strings() == ["(1);"]
    assert BinomialTree().newick_string() == ";"


def test_str_uses_ete3():
    pytest.importorskip("ete3")
    heap, _ = make_heap([1, 2, 3])
    assert isinstance(str(heap), str)


def test_node_helpers():
    parent, child = BinomialNode(1), BinomialNode(2)
    assert parent.is_leaf
    parent.add_child(child)
    assert child.parent is parent and not parent.is_leaf


@pytest.mark.parametrize("seed", range(5))
def test_randomized_differential_vs_heapq(seed):
    rng = random.Random(seed)
    heap = BinomialHeap()
    ref = []  # heapq of (key, id)
    live = {}  # id -> node
    next_id = 0
    for _ in range(400):
        op = rng.choice(["insert", "insert", "extract", "decrease", "delete", "union"])
        if op == "insert":
            node = BinomialNode(rng.randint(0, 200))
            live[next_id] = node
            heap.insert(node)
            heapq.heappush(ref, (node.key, next_id))
            next_id += 1
        elif op == "union":
            other = BinomialHeap()
            for _ in range(rng.randint(0, 6)):
                node = BinomialNode(rng.randint(0, 200))
                live[next_id] = node
                other.insert(node)
                heapq.heappush(ref, (node.key, next_id))
                next_id += 1
            heap = heap.union(other)
            assert other.is_empty
        elif op == "extract":
            got = heap.extract_min()
            if not ref:
                assert got is None
            else:
                # equal keys: the heap may return any node with the min key
                assert got.key == ref[0][0]
                i = next(k for k, v in live.items() if v is got)
                del live[i]
                ref = [(k, j) for k, j in ref if j != i]
                heapq.heapify(ref)
        elif live and op == "decrease":
            i = rng.choice(list(live))
            node = live[i]
            if node.key == 0:
                continue
            new = rng.randint(0, node.key - 1)
            heap.decrease_key(node, new)
            ref = [(new if j == i else k, j) for k, j in ref]
            heapq.heapify(ref)
        elif live and op == "delete":
            i = rng.choice(list(live))
            node = live.pop(i)
            assert heap.delete(node) is node
            ref = [(k, j) for k, j in ref if j != i]
            heapq.heapify(ref)
        check_invariants(heap)
        assert keys_of(heap) == sorted(k for k, _ in ref)
    assert drain(heap) == [k for k, _ in sorted(ref)]
