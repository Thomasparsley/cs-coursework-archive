class Bohm : DPLL, IDPLL
{
    public Bohm(string[] cnf) : base(cnf) { }

    const long alpha = 1;
    const long beta = 2;

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var maxLength = formula.clauses.Max(c => c.literals.Count);

        // h[l][i - 1] = number of clauses of length i containing literal l
        var h = new Dictionary<Literal, long[]>();
        foreach (var clause in formula.clauses)
        {
            var length = clause.literals.Count;
            if (length == 0)
                continue;

            foreach (var l in clause.literals.Distinct())
            {
                if (!h.TryGetValue(l, out var counts))
                    h[l] = counts = new long[maxLength];

                counts[length - 1]++;
            }
        }

        var zeros = new long[maxLength];

        Literal? best = null;
        long[] bestVector = Array.Empty<long>();
        var bestPositive = zeros;
        var bestNegative = zeros;

        foreach (var l in h.Keys)
        {
            var positive = l.Abs();
            var pos = h.GetValueOrDefault(positive, zeros);
            var neg = h.GetValueOrDefault(positive.Not(), zeros);

            var vector = new long[maxLength];
            for (int i = 0; i < maxLength; i++)
                vector[i] = alpha * Math.Max(pos[i], neg[i]) + beta * Math.Min(pos[i], neg[i]);

            if (best == null || Compare(vector, bestVector) > 0)
            {
                best = positive;
                bestVector = vector;
                bestPositive = pos;
                bestNegative = neg;
            }
        }

        return Compare(bestPositive, bestNegative) >= 0 ? best! : best!.Not();
    });

    // Lexicographic comparison starting from the shortest clauses.
    static int Compare(long[] a, long[] b)
    {
        for (int i = 0; i < a.Length; i++)
            if (a[i] != b[i])
                return a[i].CompareTo(b[i]);

        return 0;
    }

    public string HeuristicName()
    {
        return "Bohm";
    }
}
