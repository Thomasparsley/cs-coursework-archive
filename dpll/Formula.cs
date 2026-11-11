using System;

class Formula
{
    public string description = "";
    Dictionary<long, Variable> variables = new();
    public List<Clause> clauses = new();

    public int literalsCount
    {
        get
        {
            return DPLL.FlattenFormula(this).Count;
        }
    }

    private Formula() { }
    public Formula(string[] cnf)
    {
        var clause = new Clause();

        foreach (var rawLine in cnf)
        {
            var line = rawLine.Trim();

            if (line.Length == 0 || line[0] == 'c')
                continue;

            // SATLIB files end with a "%" line followed by "0".
            if (line[0] == '%')
                break;

            if (line[0] == 'p')
            {
                description = line;
                continue;
            }

            var tokens = line.Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries);
            foreach (var token in tokens)
            {
                var value = Int64.Parse(token);

                if (value == 0)
                {
                    clause.literals.Reverse();
                    clauses.Add(clause);
                    clause = new Clause();
                    continue;
                }

                var ID = Math.Abs(value);

                if (!variables.TryGetValue(ID, out var variable))
                {
                    variable = new Variable(ID);
                    variables[ID] = variable;
                }

                clause.Add(new Literal(value > 0, variable));
            }
        }

        // Tolerate a missing final "0".
        if (clause.literals.Count > 0)
        {
            clause.literals.Reverse();
            clauses.Add(clause);
        }

        clauses.Reverse();
    }

    public Clause this[int index]
    {
        get
        {
            return clauses[index];
        }

        set
        {
            clauses[index] = value;
        }
    }

    public void Print() => Print(clauses.Count - 1);
    void Print(int index)
    {
        if (index < 0)
        {
            System.Console.Write("\n");
            return;
        }

        clauses[index].Print();

        if (index > 0)
            System.Console.Write(" &\n");

        Print(index - 1);
    }

    public bool IsEmpty() => clauses.Count == 0;
    public bool HasEmptyClause() => clauses.FindAll(c => c.Empty()).Count != 0;

    public Optional<Literal> FindPureLiteral()
    {
        var literals = DPLL.FlattenFormula(this);
        foreach (var literal in literals)
            if (!literals.Contains(-literal))
                return new Optional<Literal>(literal);

        return new Optional<Literal>();
    }

    Optional<Clause> FindUnitClause()
    {
        var clausesWithOneLiteral = clauses.FindAll(c => c.literals.Count == 1);

        if (clausesWithOneLiteral.Count >= 1)
            return new Optional<Clause>(clausesWithOneLiteral.First());

        return new Optional<Clause>();
    }

    public Optional<Literal> FindUnitClauseLiteral()
    {
        var unitClause = FindUnitClause();
        if (unitClause.IsNone())
            return new Optional<Literal>();

        var clause = unitClause.Get();

        return new Optional<Literal>(clause.literals.First());
    }

    public Formula Clone()
    {
        return new Formula()
        {
            clauses = this.clauses.Select(c => new Clause { literals = new Literals(c.literals) }).ToList(),
            variables = new Dictionary<long, Variable>(this.variables)
        };
    }

    public void AddClause(Clause clause) => clauses.Add(clause);

    public static Formula operator & (Formula formula, Literal literal)
    {
        var f = formula.Clone();

        f = DPLL.UnitSubsumpion(f, literal);
        f = DPLL.UnitResolution(f, literal);

        return f;
    }

    public Dictionary<Literal, int> NumberOfOccurrencesOfLiterals()
    {
        var result = new Dictionary<Literal, int>();
        var flatten = DPLL.FlattenFormula(this);
        var distinctFlatten = flatten.Distinct().ToList();


        foreach (var literal in distinctFlatten)
        {
            var count = flatten.Count(l => (l.Equals(literal)));
            result.Add(literal, count);
        }

        return result;
    }

    public Clause ShortestClause()
    {
        var shortest = clauses.First();

        foreach (var clause in clauses)
            if (clause.literals.Count < shortest.literals.Count)
                shortest = clause;

        return shortest;
    }
}
