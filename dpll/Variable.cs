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

    public void Print()
    {
        System.Console.Write(ID);
    }
}
