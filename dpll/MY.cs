class MY : DPLL, IDPLL
{
    public MY(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var flatten = DPLL.FlattenFormula(formula);
        var literal = flatten.FindAll(l => l.isTrue).First();
        return literal;
    });

    public string HeuristicName()
    {
        return "MY";
    }
}
