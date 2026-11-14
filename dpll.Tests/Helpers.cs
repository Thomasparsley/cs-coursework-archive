using System.Reflection;

static class Helpers
{
    public static string[] Lines(string text) =>
        text.Replace("\r", "").Split('\n');

    public static Formula Parse(string text) => new Formula(Lines(text));

    public static string[] CnfFile(string name) =>
        File.ReadAllLines(Path.Combine(AppContext.BaseDirectory, "cnf", name));

    /// <summary>
    /// Clauses as sorted signed-integer lists, in file order. Formula stores its
    /// clauses reversed (Print walks them backwards), so reverse here.
    /// </summary>
    public static List<List<long>> Ints(Formula f) =>
        Enumerable.Reverse(f.clauses)
            .Select(c => c.literals.Select(l => l.isTrue ? l.variable.ID : -l.variable.ID).OrderBy(x => x).ToList())
            .ToList();

    public static string Show(Formula f) =>
        string.Join(" ", Ints(f).Select(c => "(" + string.Join(" ", c) + ")"));

    public static Literal Lit(long v) => new Literal(v > 0, new Variable(Math.Abs(v)));

    public static IDPLL[] AllSolvers(string[] lines) => new IDPLL[]
    {
        new DLIS(lines), new DLCS(lines), new MOM(lines), new Bohm(lines), new MY(lines),
    };

    public static readonly string[] Names = { "DLIS", "DLCS", "MOM", "Bohm", "MY" };

    public static IDPLL Make(string name, string[] lines) => name switch
    {
        "DLIS" => new DLIS(lines),
        "DLCS" => new DLCS(lines),
        "MOM" => new MOM(lines),
        "Bohm" => new Bohm(lines),
        "MY" => new MY(lines),
        _ => throw new ArgumentException(name),
    };

    /// <summary>
    /// Heuristics are lambdas defined inside each solver's Solve(); fetch the
    /// compiler-generated Formula -> Literal method by reflection so the choice
    /// can be tested directly without touching the sources.
    /// </summary>
    public static Literal Choose(Type heuristic, Formula f)
    {
        var closure = heuristic.GetNestedTypes(BindingFlags.NonPublic | BindingFlags.Public)
            .First(t => t.Name == "<>c");
        var method = closure
            .GetMethods(BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public)
            .First(m => m.ReturnType == typeof(Literal)
                        && m.GetParameters().Length == 1
                        && m.GetParameters()[0].ParameterType == typeof(Formula));
        var instance = closure.GetField("<>9", BindingFlags.Static | BindingFlags.Public)!.GetValue(null);
        return (Literal)method.Invoke(instance, new object[] { f })!;
    }

    /// <summary>Brute-force satisfiability check; clauses are lists of signed ints.</summary>
    public static bool BruteForce(int vars, List<int[]> clauses)
    {
        for (long m = 0; m < (1L << vars); m++)
        {
            bool all = true;
            foreach (var c in clauses)
            {
                bool sat = false;
                foreach (var l in c)
                {
                    bool val = ((m >> (Math.Abs(l) - 1)) & 1) == 1;
                    if (val == (l > 0)) { sat = true; break; }
                }
                if (!sat) { all = false; break; }
            }
            if (all) return true;
        }
        return false;
    }
}
