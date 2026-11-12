public class RandomizedTests
{
    public static IEnumerable<string> Factories() => Helpers.Names;

    [Test]
    [MethodDataSource(nameof(Factories))]
    [Timeout(30_000)]
    public async Task AgreesWithBruteForce(string make, CancellationToken ct)
    {
        var rng = new Random(20221229);
        int sat = 0, unsat = 0;

        for (int iter = 0; iter < 400; iter++)
        {
            int vars = rng.Next(1, 9);
            int nClauses = rng.Next(1, 4 * vars + 1);
            var clauses = new List<int[]>();
            for (int c = 0; c < nClauses; c++)
            {
                int len = rng.Next(1, Math.Min(vars, 4) + 1);
                var lits = new HashSet<int>();
                while (lits.Count < len)
                {
                    int v = rng.Next(1, vars + 1);
                    lits.Add(rng.Next(2) == 0 ? v : -v);
                }
                clauses.Add(lits.ToArray());
            }

            var text = $"p cnf {vars} {nClauses}\n" +
                       string.Join("\n", clauses.Select(c => string.Join(" ", c) + " 0"));
            var expected = Helpers.BruteForce(vars, clauses);
            if (expected) sat++; else unsat++;

            var solver = Helpers.Make(make, Helpers.Lines(text));
            var actual = Solver.Solve(solver).Item1;
            await Assert.That(actual).IsEqualTo(expected)
                .Because($"{solver.HeuristicName()} on seed iter {iter}:\n{text}");
        }

        // Sanity: the generator must produce both outcomes.
        await Assert.That(sat).IsGreaterThan(20);
        await Assert.That(unsat).IsGreaterThan(20);
    }
}
