class Literals : List<Literal>
{
    public Literals() : base() { }
    public Literals(Literals literals) : base(literals) { }

    public new Literals Add(Literal literal)
    {
        base.Add(literal);
        return this;
    }

    public new bool Contains(Literal literal)
    {
        foreach (var l in this)
            if (l.Equals(literal))
                return true;

        return false;
    }
}
