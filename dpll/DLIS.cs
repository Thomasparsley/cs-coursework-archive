class DLIS : DPLL, IDPLL
{
    public DLIS(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var occurrences = formula.NumberOfOccurrencesOfLiterals();
        var best = occurrences.First();

        foreach (var item in occurrences)
            if (item.Value > best.Value)
                best = item;

        return best.Key;
    });

    public string HeuristicName()
    {
        return "DLIS";
    }
}
