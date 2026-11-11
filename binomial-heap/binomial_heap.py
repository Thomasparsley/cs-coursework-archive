class BinomialHeap:

    def __init__(self, heap: list["BinomialTree"] = None):

        if heap is None:
            heap = []

        self.heap = heap

    def __str__(self):
        """
        Return a string representation of the heap.

        """
        result = ""

        import ete3
        newicks = self.newick_strings()
        for newick in newicks:
            result += str(ete3.Tree(newick)) + "\n"

        return result

    @property
    def is_empty(self):
        return len(self.heap) == 0

    def insert(self, node: "BinomialNode"):
        """
        Insert a node into the heap.

        """
        new_heap = BinomialHeap([BinomialTree(root=node)])
        self.heap = self.union(new_heap).heap

    def delete(self, node: "BinomialNode"):
        """
        Delete a node from the heap.

        """
        self.decrease_key(node, -1.0*float("inf"))
        return self.extract_min()

    def union(self, other: "BinomialHeap") -> "BinomialHeap":
        """
        Return the union of two binomial heaps.

        """
        new_heap = BinomialHeap()
        new_heap.heap.extend(self.heap)
        new_heap.heap.extend(other.heap)
        new_heap.heap.sort(key=lambda tree: tree.degree)

        # since when unioning two heaps there may be a situation when there are
        # two or more trees with the same degree in the heap, it is necessary
        # to merge these trees
        i = 0
        heap_len = len(new_heap.heap)
        while i < heap_len - 1:
            if new_heap.heap[i].degree == new_heap.heap[i + 1].degree:
                new_heap.heap[i].merge(new_heap.heap[i + 1])
                del new_heap.heap[i + 1]
                heap_len -= 1
            else:
                i += 1

        new_heap.heap.sort(key=lambda tree: tree.degree)
        return new_heap

    def find_node(self, node: "BinomialNode"):
        """
        Return the node in the heap.

        """
        for tree in self.heap:
            finded_node = tree.find_node(node)
            if finded_node is not None:
                return finded_node

        return None

    def min_tree(self):
        """
        Return the tree with the minimum key.

        """
        m_tree: BinomialTree = None

        for tree in self.heap:
            if m_tree is None or tree.root.key < m_tree.root.key:
                m_tree = tree

        return m_tree

    def extract_min(self):
        """
        Extract the minimum key from the heap.

        """
        m_tree = self.min_tree()

        if m_tree is None:
            return None

        self.heap.remove(m_tree)

        m_node, new_heap = m_tree.extract_min()
        self.heap = self.union(new_heap).heap
        return m_node

    def decrease_key(self, node: "BinomialNode", new_key: int):
        """
        Decrease the key of a node. If node is not in the heap, do nothing.
        New key must be less than the current key.

        """
        finded_node: BinomialNode = self.find_node(node)

        if finded_node is None:
            return

        assert new_key < finded_node.key
        finded_node.key = new_key
        finded_node.bubble_up()

    def newick_strings(self):
        """
        Return the newick string from all trees in the heap.

        """
        newicks: list[str] = []

        for tree in self.heap:
            newicks.append(tree.newick_string())

        return newicks


class BinomialTree:

    def __init__(self, degree: int = 0, root: "BinomialNode" = None):
        self.degree = degree
        self.root = root

    def merge(self, other: "BinomialTree"):
        """
        Merge two trees.

        Merge can only be performed on trees with the same degree. Technically,
        it's binomial link.

        """
        if self.root is None:
            self.root = other.root
            return

        if self.root.key <= other.root.key:
            self.root.add_child(other.root)
        else:
            other.root.add_child(self.root)
            self.root = other.root

        self.degree += 1

    def extract_min(self):
        """
        Extract the minimum key from the tree.

        """
        min_node = self.root

        degree = 0
        new_heap = BinomialHeap()
        for sub_tree in self.root.childrens:
            sub_tree.parent = None
            new_tree = BinomialTree(degree=degree, root=sub_tree)
            new_heap.heap = new_heap.union(BinomialHeap([new_tree])).heap
            degree += 1

        return min_node, new_heap

    def find_node(self, node: "BinomialNode"):
        """
        Find node in tree.

        """
        if self.root is None:
            return None

        finded_node: "BinomialNode" = None
        queue: list["BinomialNode"] = [self.root]
        queue_len = 1
        while queue_len > 0:
            node_to_test = queue.pop(0)

            if node_to_test == node:
                finded_node = node_to_test
                break

            for child in node_to_test.childrens:
                queue.append(child)

            queue_len = len(queue)

        return finded_node

    def newick_string(self):
        """
        Return the newick string of the tree.

        """
        root = self.root
        if root is None:
            return ";"

        newick_string = root.newick_string()

        return newick_string


class BinomialNode:

    def __init__(self, key: int):
        self.key = key
        self.parent: "BinomialNode" | None = None
        self.childrens: list["BinomialNode"] = []

    def __eq__(self, other: "BinomialNode"):
        return self.key == other.key

    @property
    def is_leaf(self):
        return len(self.childrens) == 0

    def add_child(self, child: "BinomialNode"):
        """
        Add child to the node.

        """
        child.parent = self
        self.childrens.append(child)

    def bubble_up(self):
        if self.parent is not None and self.key < self.parent.key:
            self.key, self.parent.key = self.parent.key, self.key
            self.parent.bubble_up()

    def newick_string(self):
        newick = ""

        newick += str(self.key)

        for child in self.childrens:
            newick += "," + child.newick_string()

        return f"({newick});"


if __name__ == "__main__":
    h1 = BinomialHeap()
    h1_keys = [5, 4, 3, 6, 8, 9]
    for key in h1_keys:
        h1.insert(BinomialNode(key))

    print("H1")
    print(h1)

    h2 = BinomialHeap()
    h2_keys = [1, 10, 7, 2, 22, 33, 11, 44]
    for key in h2_keys:
        h2.insert(BinomialNode(key))

    print("H2")
    print(h2)

    h3 = h1.union(h2)
    print("H3")
    print(h3)

    h3.extract_min()
    print("H3 - extract min")
    print(h3)

    h4 = BinomialHeap()
    h4_keys = [20, 50, 11, 7, 45, 12, 9, 3, 13, 15, 10, 5, 6, 55]
    for key in h4_keys:
        h4.insert(BinomialNode(key))

    print("H4 - before delete")
    print(h4)

    h4_keys_del = [11, 20, 10, 9, 15, 7, 6, 5, 13, 12, 50, 45, 3]
    for key in h4_keys_del:
        h4.delete(BinomialNode(key))
        print(f"deleted {key}")
        print(h4)

    print("H4 - after delete")
    print(h4)
