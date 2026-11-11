class Variable
{
    public long ID
    {
        get;
        private set;
    }

    public Variable(long ID)
    {
        this.ID = ID;
    }

    public bool Equals(Variable other)
    {
        return this.ID == other.ID;
    }

    public override bool Equals(object? obj) => obj is Variable other && Equals(other);
    public override int GetHashCode() => ID.GetHashCode();

    public void Print()
    {
        System.Console.Write(ID);
    }
}
