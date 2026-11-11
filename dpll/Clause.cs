class Clause
{
    public Literals literals = new();

    public Clause() {}
    public Clause(Literal literal)
    {
        Add(literal);
    }

    public void Print()
    {
        System.Console.Write("( ");
        Print(literals.Count - 1);
        System.Console.Write(" )");
    }
    void Print(int index)
    {
        if (index < 0)
            return;

        literals[index].Print();

        if (index > 0)
            System.Console.Write(" | ");

        Print(index - 1);
    }

    public void Add(Literal literal)
    {
        literals.Add(literal);
    }

    public bool Empty() => literals.Count == 0;
    public bool Contains(Literal literal) => literals.Contains(literal);
}
