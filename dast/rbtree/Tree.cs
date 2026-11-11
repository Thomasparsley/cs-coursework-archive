using System;
using System.Text;

class Tree
{
    private delegate void Rotate(Node node);

    private Optional<Node> root;
    private uint size;

    public Tree()
    {
        root = new Optional<Node>();
        size = 0;
    }

    public uint Size() => size;
    public void Insert(int key) => Insert(new Node(key));
    public void Insert(Node node)
    {
        if (root.IsNone())
        {
            node.color = Color.Black;
            root = new Optional<Node>(node);
            size++;
            return;
        }

        (var parrent, var direction) = FindParent(node.key);
        if (parrent.IsNone())
            throw new System.Exception("Key is already in the tree");

        node.parrent = new Optional<Node>(parrent.Get());
        if (direction == Direction.Left)
            parrent.Get().left = node;
        else
            parrent.Get().right = node;

        size++;
        FixInsert(node);
    }

    (Optional<Node>, Direction) FindParent(int key)
    {
        (Optional<Node>, Direction) Finder(Node parrent)
        {
            if (key == parrent.key)
                return (new Optional<Node>(), Direction.None);

            if (parrent.key < key)
            {
                if (parrent.right == Node.NilLeaf)
                    return (new Optional<Node>(parrent), Direction.Right);

                return Finder(parrent.right);
            }

            if (parrent.left == Node.NilLeaf)
                return (new Optional<Node>(parrent), Direction.Left);

            return Finder(parrent.left);
        }

        return Finder(root.Get());
    }

    void FixInsert(Node node)
    {
        while (
            node.parrent.IsSome() &&
            node.parrent.Get().color == Color.Red &&
            node != root.Get()
        )
        {
            if (node.parrent.Get() == node.Grandparent().Get().left)
                node = FixInsertLeft(node);
            else
                node = FixInsertRight(node);
        }

        root.Get().color = Color.Black;
    }

    static Node FixupCaseOne(Node node, Node uncle)
    {
        node.parrent.Get().color = Color.Black;
        uncle.color = Color.Black;
        var grandparent = node.Grandparent().Get();
        grandparent.color = Color.Red;

        return grandparent;
    }

    Node FixupCaseThree(Node node, Rotate rotate)
    {
        node.parrent.Get().color = Color.Black;
        node.Grandparent().Get().color = Color.Red;
        rotate(node.Grandparent().Get());

        return node;
    }

    Node FixInsertLeft(Node node)
    {
        var uncle = node.Grandparent().Get().right;

        if (uncle.color == Color.Red)
            return FixupCaseOne(node, uncle);

        if (node.IsRightChild())
        {
            node = node.parrent.Get();
            LeftRotate(node);
        }

        return FixupCaseThree(node, RightRotate);
    }

    Node FixInsertRight(Node node)
    {
        var uncle = node.Grandparent().Get().left;

        if (uncle.color == Color.Red)
            return FixupCaseOne(node, uncle);

        if (node.IsLeftChild())
        {
            node = node.parrent.Get();
            RightRotate(node);
        }

        return FixupCaseThree(node, LeftRotate);
    }

    void LeftRotate(Node node)
    {
        var sibling = node.right;
        node.right = sibling.left;

        if (sibling.left != Node.NilLeaf)
            sibling.left.parrent = new Optional<Node>(node);

        sibling.parrent = node.parrent;

        if (node.parrent.IsNone())
            root = new Optional<Node>(sibling);
        else if (node == node.parrent.Get().left)
            node.parrent.Get().left = sibling;
        else
            node.parrent.Get().right = sibling;

        sibling.left = node;
        node.parrent = new Optional<Node>(sibling);
    }

    void RightRotate(Node node)
    {
        var sibling = node.left;
        node.left = sibling.right;

        if (sibling.right != Node.NilLeaf)
            sibling.right.parrent = new Optional<Node>(node);

        sibling.parrent = node.parrent;

        if (node.parrent.IsNone())
            root = new Optional<Node>(sibling);
        else if (node == node.parrent.Get().right)
            node.parrent.Get().right = sibling;
        else
            node.parrent.Get().left = sibling;

        sibling.right = node;
        node.parrent = new Optional<Node>(sibling);
    }

    void Transplant(Node a, Node b)
    {
        if (a.parrent.IsNone())
            root = new Optional<Node>(b);
        else if (a.IsLeftChild())
            a.parrent.Get().left = b;
        else
            a.parrent.Get().right = b;

        b.parrent = a.parrent;
    }

    public Optional<Node> Delete(int key)
    {
        var node = Find(key);
        if (node.IsNone())
            return new Optional<Node>();

        return Delete(node.Get());
    }

    public Optional<Node> Delete(Node node)
    {
        if (root.IsNone())
            return new Optional<Node>();

        size--;

        var startColor = node.color;

        Node x;
        // Zero or one child
        if (node.left == Node.NilLeaf)
        {
            x = node.right;
            Transplant(node, node.right);
        }
        else if (node.right == Node.NilLeaf)
        {
            x = node.left;
            Transplant(node, node.left);
        }
        // Two children
        else
        {
            var minNode = MinNode(node.right);
            startColor = minNode.color;
            x = minNode.right;

            if (minNode.parrent.Get() == node)
                x.parrent = new Optional<Node>(minNode);
            else
            {
                Transplant(minNode, minNode.right);
                minNode.right = node.right;
                minNode.right.parrent = new Optional<Node>(minNode);
            }

            Transplant(node, minNode);
            minNode.left = node.left;
            minNode.left.parrent = new Optional<Node>(minNode);
            minNode.color = node.color;
        }

        if (startColor == Color.Black)
            FixDelete(x);

        return new Optional<Node>(node);
    }

    Node MinNode(Node node)
    {
        while (node.left != Node.NilLeaf)
            node = node.left;

        return node;
    }

    void FixDelete(Node node)
    {
        while (
            node != root.Get() &&
            node.color == Color.Black
        )
        {
            if (node.IsLeftChild())
            {
                var sibling = node.parrent.Get().right;

                // Case 1
                if (sibling.color == Color.Red)
                {
                    sibling.color = Color.Black;
                    node.parrent.Get().color = Color.Red;
                    LeftRotate(node.parrent.Get());
                    sibling = node.parrent.Get().right;
                }

                // Case 2
                if (sibling.left.color == Color.Black && sibling.right.color == Color.Black)
                {
                    sibling.color = Color.Red;
                    node = node.parrent.Get();
                }
                else
                {
                    // Case 3
                    if (sibling.right.color == Color.Black)
                    {
                        sibling.left.color = Color.Black;
                        sibling.color = Color.Red;
                        RightRotate(sibling);
                        sibling = node.parrent.Get().right;
                    }

                    // Case 4
                    sibling.color = node.parrent.Get().color;
                    node.parrent.Get().color = Color.Black;
                    sibling.right.color = Color.Black;
                    LeftRotate(node.parrent.Get());
                    node = root.Get();
                }
            }
            else
            {
                var sibling = node.parrent.Get().left;

                // Case 1
                if (sibling.color == Color.Red)
                {
                    sibling.color = Color.Black;
                    node.parrent.Get().color = Color.Red;
                    RightRotate(node.parrent.Get());
                    sibling = node.parrent.Get().left;
                }

                // Case 2
                if (sibling.right.color == Color.Black && sibling.left.color == Color.Black)
                {
                    sibling.color = Color.Red;
                    node = node.parrent.Get();
                }
                else
                {
                    // Case 3
                    if (sibling.left.color == Color.Black)
                    {
                        sibling.right.color = Color.Black;
                        sibling.color = Color.Red;
                        LeftRotate(sibling);
                        sibling = node.parrent.Get().left;
                    }

                    // Case 4
                    sibling.color = node.parrent.Get().color;
                    node.parrent.Get().color = Color.Black;
                    sibling.left.color = Color.Black;
                    RightRotate(node.parrent.Get());
                    node = root.Get();
                }
            }
        }

        node.color = Color.Black;
    }

    public Optional<Node> Find(int key)
    {
        var node = root.Get();

        while (node != Node.NilLeaf)
        {
            if (key < node.key)
                node = node.left;
            else if (key > node.key)
                node = node.right;
            else
                return new Optional<Node>(node);
        }

        return new Optional<Node>();
    }

    public void Print()
    {
        if (root.IsNone())
            return;

        root.Get().Print();
    }
}
