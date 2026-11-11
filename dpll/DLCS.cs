class DLCS : DPLL, IDPLL
{
    public DLCS(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var occurrences = formula.NumberOfOccurrencesOfLiterals();

        Literal best = occurrences.First().Key.Abs();
        var bestTotal = -1;
        var bestPositive = 0;
        var bestNegative = 0;

        foreach (var item in occurrences)
        {
            var positive = item.Key.Abs();
            var negative = positive.Not();

            var a = occurrences.GetValueOrDefault(positive);
            var b = occurrences.GetValueOrDefault(negative);

            if (a + b > bestTotal)
            {
                best = positive;
                bestTotal = a + b;
                bestPositive = a;
                bestNegative = b;
            }
        }

        return bestPositive >= bestNegative ? best : best.Not();
    });

    public string HeuristicName()
    {
        return "DLCS";
    }
}
