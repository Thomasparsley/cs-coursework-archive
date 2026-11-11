class Bohm : DPLL, IDPLL
{
    public Bohm(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        const long p1 = 10;
        const long p2 = 20;
        var flatten = DPLL.FlattenFormula(formula);


        Literal maxLiteral = flatten.First();
        var maxLiteralValue = long.MinValue;

        foreach (var l in flatten)
        {
            long sum = 0;

            foreach (var clause in formula.clauses)
            {
                if (clause.literals.Count == 0)
                    continue;

#pragma warning disable CS8602
                var l1 = clause.Contains(l) ? clause.literals.Find(x => x.Equals(l)).variable.ID : 0;
                var l2 = clause.Contains(-l) ? clause.literals.Find(x => x.Equals(-l)).variable.ID : 0;
#pragma warning restore CS8602
                sum += p1 * Math.Max(l1, l2) + p2 * Math.Min(l1, l2);
            }

            if (sum > maxLiteralValue)
            {
                maxLiteralValue = sum;
                maxLiteral = l;
            }
        }

        return maxLiteral;
    });

    public string HeuristicName()
    {
        return "Bohm";
    }
}
