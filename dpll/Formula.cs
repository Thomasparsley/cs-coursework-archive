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

    private bool SkipChar(char ch) =>
    ch switch
    {
        'c' => true,
        'p' => false,
        '%' => true,
        '0' => true,
        _ => false,
    };

    private Formula() { }
    public Formula(string[] cnf)
    {
        foreach (var line in cnf)
        {
            if (line.Length == 0 || SkipChar(line[0]))
                continue;

            if (line[0] == 'p')
            {
                description = line;
                continue;
            }

            var clause = new Clause();

            foreach (var literalString in line.Split(" "))
            {
                if (string.IsNullOrEmpty(literalString))
                    continue;
                else if (literalString == "0")
                    continue;

                var isTrue = true;
                var literalValue = literalString;
                if (literalValue[0] == '-')
                {
                    isTrue = false;
                    literalValue = literalValue.Remove(0, 1);
                }

                var ID = Int64.Parse(literalValue);

                Variable variable;
                try
                {
                    variable = variables[ID];
                }
                catch (KeyNotFoundException)
                {
                    variables[ID] = new Variable(ID);
                    variable = variables[ID];
                }

                clause.Add(new Literal(isTrue, variable));
            }

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
            clauses = new List<Clause>(this.clauses),
            variables = new Dictionary<long, Variable>(this.variables)
        };
    }

    public void AddClause(Clause clause) => clauses.Add(clause);

    public static Formula operator & (Formula formula, Literal literal)
    {
        var f = formula.Clone();
        
        if (literal.isTrue)
            f = DPLL.UnitSubsumpion(f, literal);
        else
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
