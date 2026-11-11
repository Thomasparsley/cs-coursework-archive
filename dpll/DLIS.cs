class DLIS : DPLL, IDPLL
{
    public DLIS(string[] cnf) : base(cnf) { }

    public new (bool, long) Solve() => Solve((formula) =>
    {
        var occurrences = formula.NumberOfOccurrencesOfLiterals();
        var first = occurrences.OrderBy(x => x.Value).ToDictionary(x => x.Key, x => x.Value).First();
        var literal = first.Key;

        return literal;
    });

    public string HeuristicName()
    {
        return "DLIS";
    }
}
