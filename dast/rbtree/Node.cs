using System;

class Node
{
    public static readonly Node NilLeaf = new Node(Int32.MinValue, Color.Black);

    public int key;
    public Color color;
    public Optional<Node> parrent;
    public Node left;
    public Node right;

    public Node(int key)
    {
        this.key = key;
        color = Color.Red;
        parrent = new Optional<Node>();

        left = NilLeaf;
        right = NilLeaf;
    }

    public Node(int key, Color color) : this(key)
    {
        this.color = color;
    }

    public Optional<Node> Grandparent()
        => parrent.IsNone() ? new Optional<Node>() : parrent.Get().parrent;

    public uint NumberOfChildrens()
    {
        uint count = 0;

        if (this == NilLeaf)
            return count;
        else
        {
            if (left != NilLeaf)
                count++;
            if (right != NilLeaf)
                count++;
        }

        return count + left.NumberOfChildrens() + right.NumberOfChildrens();

    }

    public bool IsLeftChild() =>
        parrent.IsSome() && parrent.Get().left == this;

    public bool IsRightChild() =>
        parrent.IsSome() && parrent.Get().right == this;

    public void Print() => Print(true, "");
    public void Print(bool last, string prefix)
    {
        string keyString;
        if (color == Color.Red)
            keyString = $"{ColoredText.Red}{key}{ColoredText.Reset}";
        else if (this == NilLeaf)
            keyString = "BLACK LEAF";
        else
            keyString = $"{key}";

        (string current, string next) = last
            ? (prefix + "└─" + keyString, prefix + "  ")
            : (prefix + "├─" + keyString, prefix + "| ");

        System.Console.WriteLine(current[2..]);

        if (this != NilLeaf)
        {
            right.Print(false, next);
            left.Print(true, next);
        }
    }
}