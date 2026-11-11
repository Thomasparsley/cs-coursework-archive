class MOM : DPLL, IDPLL
{
    public MOM(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var result = new Dictionary<Literal, int>();

        var flatten = DPLL.FlattenFormula(formula);
        var p = flatten.Count * flatten.Count + 1;
        var shortest = formula.ShortestClause();

        Literal maxLiteral = shortest.literals.First();
        var maxLiteralValue = long.MinValue;

        foreach(var l in flatten.Where(x => shortest.literals.Contains(x)))
        {
            var l1 = l.variable.ID;
#pragma warning disable CS8602
            var l2 = shortest.Contains(-l) ? shortest.literals.Find(x => x.Equals(-l)).variable.ID : 0;
#pragma warning restore CS8602

            var num = (l1 + l2) * p + l1 * l2;
            if (num > maxLiteralValue)
            {
                maxLiteralValue = num;
                maxLiteral = l;
            }
        }

        return maxLiteral;
    });

    public string HeuristicName()
    {
        return "MOM";
    }
}
