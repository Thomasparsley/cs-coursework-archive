using System.Reflection;

namespace RbTree.Tests;

// Node.NilLeaf is a shared mutable static (its parent pointer is rewritten during
// delete), so trees are not thread-safe; run tests serially.
[NotInParallel]
public class TreeTests
{
    // ---- helpers -------------------------------------------------------

    static Node? RootOf(Tree tree)
    {
        var field = typeof(Tree).GetField("root", BindingFlags.NonPublic | BindingFlags.Instance)!;
        var opt = (Optional<Node>)field.GetValue(tree)!;
        return opt.IsSome() ? opt.Get() : null;
    }

    /// Checks all red-black invariants; returns the number of real nodes.
    
    static int CheckInvariants(Tree tree)
    {
        var root = RootOf(tree);
        int count = 0;
        if (root is not null)
        {
            if (root.color != Color.Black) throw new Exception("root is not black");
            if (root.parrent.IsSome()) throw new Exception("root has a parent");
            BlackHeight(root, int.MinValue, int.MaxValue, false, false, ref count);
        }
        if (count != (int)tree.Size())
            throw new Exception($"size mismatch: counted {count}, Size() {tree.Size()}");
        return count;
    }

    static int BlackHeight(Node n, int lo, int hi, bool hasLo, bool hasHi, ref int count)
    {
        if (n == Node.NilLeaf) return 1;
        count++;
        if (hasLo && n.key <= lo) throw new Exception($"BST order violated at {n.key}");
        if (hasHi && n.key >= hi) throw new Exception($"BST order violated at {n.key}");
        if (n.color == Color.Red)
        {
            if (n.left.color == Color.Red || n.right.color == Color.Red)
                throw new Exception($"red-red at {n.key}");
        }
        if (n.left != Node.NilLeaf && !(n.left.parrent.IsSome() && n.left.parrent.Get() == n))
            throw new Exception($"bad parent pointer of left child of {n.key}");
        if (n.right != Node.NilLeaf && !(n.right.parrent.IsSome() && n.right.parrent.Get() == n))
            throw new Exception($"bad parent pointer of right child of {n.key}");
        int l = BlackHeight(n.left, lo, n.key, hasLo, true, ref count);
        int r = BlackHeight(n.right, n.key, hi, true, hasHi, ref count);
        if (l != r) throw new Exception($"black height mismatch at {n.key}");
        return l + (n.color == Color.Black ? 1 : 0);
    }

    static Tree Build(params int[] keys)
    {
        var t = new Tree();
        foreach (var k in keys) t.Insert(k);
        return t;
    }

    // ---- basics --------------------------------------------------------

    [Test]
    public async Task NewTree_IsEmpty()
    {
        var t = new Tree();
        await Assert.That(t.Size()).IsEqualTo(0u);
        await Assert.That(RootOf(t)).IsNull();
    }

    [Test]
    public async Task Insert_SingleKey_BecomesBlackRoot()
    {
        var t = Build(42);
        await Assert.That(t.Size()).IsEqualTo(1u);
        var root = RootOf(t)!;
        await Assert.That(root.key).IsEqualTo(42);
        await Assert.That(root.color).IsEqualTo(Color.Black);
        CheckInvariants(t);
    }

    [Test]
    public async Task Insert_ThenFind_ReturnsNodeWithKey()
    {
        var t = Build(5, 3, 8, 1, 4);
        foreach (var k in new[] { 5, 3, 8, 1, 4 })
        {
            var f = t.Find(k);
            await Assert.That(f.IsSome()).IsTrue();
            await Assert.That(f.Get().key).IsEqualTo(k);
        }
        await Assert.That(t.Size()).IsEqualTo(5u);
    }

    [Test]
    public async Task Find_MissingKey_ReturnsNone()
    {
        var t = Build(5, 3, 8);
        await Assert.That(t.Find(4).IsNone()).IsTrue();
        await Assert.That(t.Find(100).IsNone()).IsTrue();
        await Assert.That(t.Find(-100).IsNone()).IsTrue();
    }

    [Test]
    public async Task Insert_DuplicateKey_Throws_AndKeepsSize()
    {
        var t = Build(1, 2, 3);
        await Assert.That(() => t.Insert(2)).Throws<Exception>();
        await Assert.That(t.Size()).IsEqualTo(3u);
        CheckInvariants(t);
    }

    [Test]
    public async Task Insert_DuplicateOfRoot_Throws()
    {
        var t = Build(10);
        await Assert.That(() => t.Insert(10)).Throws<Exception>();
        await Assert.That(t.Size()).IsEqualTo(1u);
    }

    [Test]
    public async Task Insert_AscendingAndDescending_StaysBalanced()
    {
        var asc = new Tree();
        for (int i = 0; i < 200; i++) { asc.Insert(i); CheckInvariants(asc); }
        var desc = new Tree();
        for (int i = 200; i > 0; i--) { desc.Insert(i); CheckInvariants(desc); }
        await Assert.That(asc.Size()).IsEqualTo(200u);
        await Assert.That(desc.Size()).IsEqualTo(200u);
    }

    [Test]
    public async Task Insert_ExtremeKeys()
    {
        var t = Build(0, int.MaxValue, -1);
        await Assert.That(t.Find(int.MaxValue).IsSome()).IsTrue();
        await Assert.That(t.Find(-1).IsSome()).IsTrue();
        CheckInvariants(t);
    }

    // ---- delete --------------------------------------------------------

    [Test]
    public async Task Delete_ByKey_ReturnsNodeAndShrinks()
    {
        var t = Build(5, 3, 8, 1, 4);
        var d = t.Delete(3);
        await Assert.That(d.IsSome()).IsTrue();
        await Assert.That(d.Get().key).IsEqualTo(3);
        await Assert.That(t.Size()).IsEqualTo(4u);
        await Assert.That(t.Find(3).IsNone()).IsTrue();
        CheckInvariants(t);
    }

    [Test]
    public async Task Delete_ByNode_Works()
    {
        var t = Build(5, 3, 8);
        var node = t.Find(8).Get();
        var d = t.Delete(node);
        await Assert.That(d.IsSome()).IsTrue();
        await Assert.That(d.Get()).IsSameReferenceAs(node);
        await Assert.That(t.Size()).IsEqualTo(2u);
        CheckInvariants(t);
    }

    [Test]
    public async Task Delete_MissingKey_ReturnsNone_SizeUnchanged()
    {
        var t = Build(5, 3, 8);
        await Assert.That(t.Delete(99).IsNone()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(3u);
        CheckInvariants(t);
    }

    [Test]
    public async Task Delete_Root_WithTwoChildren()
    {
        var t = Build(5, 3, 8);
        var d = t.Delete(RootOf(t)!.key);
        await Assert.That(d.IsSome()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(2u);
        CheckInvariants(t);
    }

    [Test]
    public async Task EmptyTree_FindAndDelete_ReturnNone()
    {
        var t = new Tree();
        await Assert.That(t.Find(1).IsNone()).IsTrue();
        await Assert.That(t.Delete(1).IsNone()).IsTrue();
        await Assert.That(t.Delete(new Node(1)).IsNone()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(0u);
        t.Print(); // must not throw on empty tree
    }

    [Test]
    public async Task DrainToEmpty_ThenReinsert()
    {
        var t = Build(2, 1, 3);
        foreach (var k in new[] { 1, 2, 3 })
        {
            await Assert.That(t.Delete(k).IsSome()).IsTrue();
            CheckInvariants(t);
        }
        await Assert.That(t.Size()).IsEqualTo(0u);
        await Assert.That(RootOf(t)).IsNull();
        await Assert.That(t.Find(2).IsNone()).IsTrue();

        t.Insert(7);
        t.Insert(1);
        await Assert.That(t.Size()).IsEqualTo(2u);
        CheckInvariants(t);
    }

    [Test]
    public async Task SingleElement_DeleteThenReinsert()
    {
        var t = Build(1);
        await Assert.That(t.Delete(1).IsSome()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(0u);
        t.Insert(1);
        await Assert.That(t.Size()).IsEqualTo(1u);
        CheckInvariants(t);
    }

    [Test]
    public async Task DoubleDelete_ByKey_DoesNotCorruptSize()
    {
        var t = Build(1, 2, 3, 4);
        await Assert.That(t.Delete(2).IsSome()).IsTrue();
        await Assert.That(t.Delete(2).IsNone()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(3u);
        CheckInvariants(t);
    }

    [Test]
    public async Task DoubleDelete_ByNode_DoesNotCorruptSize()
    {
        var t = Build(1, 2, 3, 4);
        var node = t.Find(3).Get();
        await Assert.That(t.Delete(node).IsSome()).IsTrue();
        await Assert.That(t.Delete(node).IsNone()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(3u);
        CheckInvariants(t);
    }

    [Test]
    public async Task Delete_ForeignNodeWithSameKey_IsRejected()
    {
        var t = Build(1, 2, 3);
        var foreign = new Node(2);
        await Assert.That(t.Delete(foreign).IsNone()).IsTrue();
        await Assert.That(t.Size()).IsEqualTo(3u);
        await Assert.That(t.Find(2).IsSome()).IsTrue();
        CheckInvariants(t);
    }

    [Test]
    public async Task Delete_NodeFromOtherTree_IsRejected()
    {
        var a = Build(1, 2, 3);
        var b = Build(1, 2, 3);
        var nodeOfB = b.Find(2).Get();
        await Assert.That(a.Delete(nodeOfB).IsNone()).IsTrue();
        await Assert.That(a.Size()).IsEqualTo(3u);
        await Assert.That(b.Size()).IsEqualTo(3u);
        CheckInvariants(a);
        CheckInvariants(b);
    }

    [Test]
    public async Task DeletedNode_IsDetached_AndCanBeReinserted()
    {
        var t = Build(5, 3, 8, 1, 4);
        var node = t.Delete(3).Get();
        await Assert.That(node.left).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(node.right).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(node.parrent.IsNone()).IsTrue();
        await Assert.That(node.color).IsEqualTo(Color.Red);

        t.Insert(node);
        await Assert.That(t.Size()).IsEqualTo(5u);
        await Assert.That(t.Find(3).Get()).IsSameReferenceAs(node);
        CheckInvariants(t);
    }

    [Test]
    public async Task DeletedNode_CanBeInsertedIntoAnotherTree()
    {
        var a = Build(1, 2, 3);
        var b = Build(10, 20);
        var node = a.Delete(2).Get();
        b.Insert(node);
        await Assert.That(b.Size()).IsEqualTo(3u);
        await Assert.That(a.Size()).IsEqualTo(2u);
        CheckInvariants(a);
        CheckInvariants(b);
    }

    // ---- sentinel / node ----------------------------------------------

    [Test]
    public async Task NilLeaf_IsBlack_AndSelfReferencing()
    {
        await Assert.That(Node.NilLeaf.color).IsEqualTo(Color.Black);
        await Assert.That(Node.NilLeaf.key).IsEqualTo(int.MinValue);
        await Assert.That(Node.NilLeaf.left).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(Node.NilLeaf.right).IsSameReferenceAs(Node.NilLeaf);
    }

    [Test]
    public async Task NewNode_HasNilLeafChildren_AndIsRed()
    {
        var n = new Node(7);
        await Assert.That(n.left).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(n.right).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(n.color).IsEqualTo(Color.Red);
        await Assert.That(n.parrent.IsNone()).IsTrue();
    }

    [Test]
    public async Task NilLeaf_StaysBlack_AfterManyOperations()
    {
        var t = new Tree();
        for (int i = 0; i < 100; i++) t.Insert(i);
        for (int i = 0; i < 100; i += 2) t.Delete(i);
        await Assert.That(Node.NilLeaf.color).IsEqualTo(Color.Black);
        await Assert.That(Node.NilLeaf.left).IsSameReferenceAs(Node.NilLeaf);
        await Assert.That(Node.NilLeaf.right).IsSameReferenceAs(Node.NilLeaf);
    }

    [Test]
    public async Task Node_ChildHelpers_AndGrandparent()
    {
        var t = Build(2, 1, 3);
        var root = RootOf(t)!;
        var l = root.left;
        var r = root.right;
        await Assert.That(l.IsLeftChild()).IsTrue();
        await Assert.That(l.IsRightChild()).IsFalse();
        await Assert.That(r.IsRightChild()).IsTrue();
        await Assert.That(root.IsLeftChild()).IsFalse();
        await Assert.That(root.NumberOfChildrens()).IsEqualTo(2u);
        await Assert.That(Node.NilLeaf.NumberOfChildrens()).IsEqualTo(0u);
        await Assert.That(root.Grandparent().IsNone()).IsTrue();
        await Assert.That(l.Grandparent().IsNone()).IsTrue();
    }

    [Test]
    public async Task Optional_GetOnNone_Throws()
    {
        var o = new Optional<Node>();
        await Assert.That(o.IsNone()).IsTrue();
        await Assert.That(() => o.Get()).Throws<Exception>();
    }

    // ---- randomized ----------------------------------------------------

    [Test]
    [Arguments(1)]
    [Arguments(2)]
    [Arguments(12345)]
    public async Task Randomized_AgainstSortedSet(int seed)
    {
        var rng = new Random(seed);
        var tree = new Tree();
        var model = new SortedSet<int>();

        for (int step = 0; step < 3000; step++)
        {
            int key = rng.Next(0, 300);
            switch (rng.Next(3))
            {
                case 0:
                    if (model.Add(key)) tree.Insert(key);
                    else await Assert.That(() => tree.Insert(key)).Throws<Exception>();
                    break;
                case 1:
                    var removed = tree.Delete(key);
                    await Assert.That(removed.IsSome()).IsEqualTo(model.Remove(key));
                    break;
                default:
                    var found = tree.Find(key);
                    await Assert.That(found.IsSome()).IsEqualTo(model.Contains(key));
                    break;
            }

            await Assert.That(tree.Size()).IsEqualTo((uint)model.Count);
            if (step % 10 == 0) CheckInvariants(tree);
        }

        CheckInvariants(tree);
        foreach (var k in model.ToArray())
        {
            await Assert.That(tree.Delete(k).IsSome()).IsTrue();
        }
        await Assert.That(tree.Size()).IsEqualTo(0u);
        CheckInvariants(tree);
    }

    [Test]
    public async Task Randomized_InvariantsAfterEveryOp_Shuffled()
    {
        var rng = new Random(777);
        var keys = Enumerable.Range(0, 150).OrderBy(_ => rng.Next()).ToArray();
        var t = new Tree();
        foreach (var k in keys) { t.Insert(k); CheckInvariants(t); }
        foreach (var k in keys.OrderBy(_ => rng.Next())) { t.Delete(k); CheckInvariants(t); }
        await Assert.That(t.Size()).IsEqualTo(0u);
    }
}
