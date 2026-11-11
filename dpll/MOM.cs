class MOM : DPLL, IDPLL
{
    public MOM(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var minSize = formula.clauses.Min(c => c.literals.Count);
        var weight = 1L << Math.Min(minSize, 40);

        var f = new Dictionary<Literal, long>();
        foreach (var clause in formula.clauses)
            if (clause.literals.Count == minSize)
                foreach (var l in clause.literals)
                    f[l] = f.GetValueOrDefault(l) + 1;

        Literal? best = null;
        var bestScore = long.MinValue;
        var bestPositive = 0L;
        var bestNegative = 0L;

        foreach (var l in f.Keys)
        {
            var positive = l.Abs();
            var a = f.GetValueOrDefault(positive);
            var b = f.GetValueOrDefault(positive.Not());

            var score = (a + b) * weight + a * b;
            if (score > bestScore)
            {
                bestScore = score;
                best = positive;
                bestPositive = a;
                bestNegative = b;
            }
        }

        return bestPositive >= bestNegative ? best! : best!.Not();
    });

    public string HeuristicName()
    {
        return "MOM";
    }
}
