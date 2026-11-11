class DLCS : DPLL, IDPLL
{
    public DLCS(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var result = new Dictionary<Literal, int>();
        var occurrences = formula.NumberOfOccurrencesOfLiterals();

        foreach (var item in occurrences)
        {
            var count = item.Value;
            Literal key;
            if (!item.Key.isTrue)
                key = item.Key.Not();
            else
                key = item.Key;

            if (!result.ContainsKey(key))
                result.Add(key, count);
            else
                result[key] += count;
        }

        var first = result.OrderBy(x => x.Value).ToDictionary(x => x.Key, x => x.Value).First();
        var literal = first.Key;

        var a = 0;
        if (occurrences.ContainsKey(literal.Abs()))
            a = occurrences[literal];

        var b = 0;
        if (occurrences.ContainsKey(literal.Abs().Not()))
            b = occurrences[literal.Abs().Not()];

        if (a >= b)
            return literal.Abs();

        return literal.Abs().Not();
    });

    public string HeuristicName()
    {
        return "DLCS";
    }
}
