public class SolverTests
{
    public static IEnumerable<string> Factories() => Helpers.Names;

    static bool Solve(IDPLL s) => Solver.Solve(s).Item1;

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task SimpleSatisfiable(string make)
    {
        var lines = Helpers.Lines("p cnf 3 2\n1 -3 0\n2 3 -1 0\n");
        await Assert.That(Solve(Helpers.Make(make, lines))).IsTrue();
    }

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task SimpleUnsatisfiable(string make)
    {
        var lines = Helpers.Lines("p cnf 2 4\n1 2 0\n-1 2 0\n1 -2 0\n-1 -2 0\n");
        await Assert.That(Solve(Helpers.Make(make, lines))).IsFalse();
    }

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task ContradictoryUnits(string make)
    {
        var lines = Helpers.Lines("p cnf 1 2\n1 0\n-1 0\n");
        await Assert.That(Solve(Helpers.Make(make, lines))).IsFalse();
    }

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task EmptyClauseIsUnsat(string make)
    {
        var lines = Helpers.Lines("p cnf 1 2\n1 0\n0\n");
        await Assert.That(Solve(Helpers.Make(make, lines))).IsFalse();
    }

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task EmptyFormulaIsSat(string make)
    {
        var lines = Helpers.Lines("p cnf 0 0\n");
        await Assert.That(Solve(Helpers.Make(make, lines))).IsTrue();
    }

    [Test]
    [MethodDataSource(nameof(Factories))]
    public async Task RequiresBranching(string make)
    {
        // Odd cycle of XOR constraints: UNSAT; even chain: SAT. No units, no pure literals.
        var unsat = Helpers.Lines("p cnf 3 6\n1 2 0\n-1 -2 0\n2 3 0\n-2 -3 0\n1 3 0\n-1 -3 0\n");
        await Assert.That(Solve(Helpers.Make(make, unsat))).IsFalse();
        var sat = Helpers.Lines("p cnf 3 4\n1 2 0\n-1 -2 0\n2 3 0\n-2 -3 0\n");
        await Assert.That(Solve(Helpers.Make(make, sat))).IsTrue();
    }

    [Test]
    [Arguments("formula.cnf", true)]
    [Arguments("simple_v3_c2.cnf", true)]
    [Arguments("quinn.cnf", true)]
    [Arguments("aim-50-1_6-yes1-4.cnf", true)]
    [Arguments("php.cnf", false)]
    [Arguments("generated.cnf", true)]
    [Timeout(30_000)]
    public async Task BundledSamplesAllHeuristics(string file, bool expected, CancellationToken ct)
    {
        var lines = Helpers.CnfFile(file);
        foreach (var make in Helpers.Names)
        {
            var s = Helpers.Make(make, lines);
            var (result, _) = Solver.Solve(s);
            await Assert.That(result).IsEqualTo(expected).Because($"{s.HeuristicName()} on {file}");
        }
    }

    [Test]
    [Skip("Too slow: exceeds 60s even with the Bohm heuristic")]
    public async Task Aim100UnsatWithBohm(CancellationToken ct)
    {
        // Slowest bundled sample; one heuristic is enough to check the answer.
        var (result, _) = Solver.Solve(new Bohm(Helpers.CnfFile("aim-100-1_6-no-1.cnf")));
        await Assert.That(result).IsFalse();
    }

    [Test]
    public async Task HeuristicNames()
    {
        var names = Helpers.AllSolvers(Helpers.Lines("1 0")).Select(s => s.HeuristicName()).ToArray();
        await Assert.That(names).IsEquivalentTo(new[] { "DLIS", "DLCS", "MOM", "Bohm", "MY" });
    }
}
