class DPLL
{
    protected Formula formula;
#pragma warning disable CS0414
    long recursiveCalls = 0;
#pragma warning restore CS0414


    public DPLL(string[] cnf)
    {
        formula = new Formula(cnf);
    }

    public static Literals FlattenFormula(Formula formula)
    {
        var result = new Literals();

        foreach (var clause in formula.clauses)
            foreach (var literal in clause.literals)
                result.Add(literal);

        return result;
    }

    public static Formula UnitSubsumpion(Formula formula, Literal literal)
    {
        for (int i = formula.clauses.Count-1; i >= 0; i--)
        {
            var clause = formula.clauses[i];

            if (clause.Contains(literal))
                formula.clauses.Remove(clause);
        }

        return formula;
    }

    public static Formula UnitResolution(Formula formula, Literal literal)
    {
        var notLiteral = -literal;

        for (int i = formula.clauses.Count-1; i >= 0; i--)
        {
            var clause = formula.clauses[i];
            clause.literals.RemoveAll(l => l.Equals(notLiteral));
        }

        return formula;
    }

    public (bool, long) Solve() => (false, 0);
    protected (bool, long) Solve(Func<Formula, Literal> heuristics) => Solve(formula, heuristics);
    (bool, long) Solve(Formula formula, Func<Formula, Literal> heuristics)
    {
        recursiveCalls++;

        Optional<Literal> unitClauseLiteral;
        while ((unitClauseLiteral = formula.FindUnitClauseLiteral()).IsSome())
        {
            formula = UnitSubsumpion(formula, unitClauseLiteral.Get());
            formula = UnitResolution(formula, unitClauseLiteral.Get());
        }

        if (formula.HasEmptyClause())
            return (false, recursiveCalls);

        Optional<Literal> pureLiteral;
        while ((pureLiteral = formula.FindPureLiteral()).IsSome())
            formula = UnitSubsumpion(formula, pureLiteral.Get());

        if (formula.IsEmpty())
            return (true, recursiveCalls);

        var x = heuristics(formula);

        var l = Solve(formula & x, heuristics);
        if (l.Item1)
            return l;

        var r = Solve(formula & -x, heuristics);
        if (r.Item1)
            return r;

        return (false, recursiveCalls);
    }
}
