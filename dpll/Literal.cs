class Literal
{
    public bool isTrue
    {
        get; private set;
    }

    public Variable variable
    {
        get; private set;
    }

    public Literal(bool isTrue, Variable variable)
    {
        this.variable = variable;
        this.isTrue = isTrue;
    }

    public bool Equals(Literal other)
    {
        return this.variable.Equals(other.variable) && this.isTrue == other.isTrue;
    }

    public void Print()
    {
        if (!isTrue)
            System.Console.Write("!");

        variable.Print();
    }

    public bool IsSame(Literal other) => IsSame(other.variable);
    public bool IsSame(Variable other) => variable.Equals(other);
    public Literal Not() => new Literal(!isTrue, variable);
    public Literal Abs() => new Literal(true, variable);

    public static Literal operator - (Literal a) => a.Not();
}
