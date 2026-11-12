public class ParsingTests
{
    [Test]
    public async Task CommentsAndProblemLine()
    {
        var f = Helpers.Parse("c comment\nc another\np cnf 3 2\n1 -2 0\n2 3 0\n");
        await Assert.That(f.description).IsEqualTo("p cnf 3 2");
        await Assert.That(Helpers.Show(f)).IsEqualTo("(-2 1) (2 3)");
    }

    [Test]
    public async Task ClauseSpanningMultipleLines()
    {
        var f = Helpers.Parse("p cnf 3 1\n1\n-2\n3 0\n");
        await Assert.That(f.clauses.Count).IsEqualTo(1);
        await Assert.That(Helpers.Show(f)).IsEqualTo("(-2 1 3)");
    }

    [Test]
    public async Task SeveralClausesOnOneLine()
    {
        var f = Helpers.Parse("p cnf 3 3\n1 2 0 -1 3 0 -2 0\n");
        await Assert.That(Helpers.Show(f)).IsEqualTo("(1 2) (-1 3) (-2)");
    }

    [Test]
    public async Task TabsAndExtraWhitespace()
    {
        var f = Helpers.Parse("p cnf 3 2\n\t1\t-2   0\n   \n  2 \t 3\t0  \n");
        await Assert.That(Helpers.Show(f)).IsEqualTo("(-2 1) (2 3)");
    }

    [Test]
    public async Task EmptyClause()
    {
        var f = Helpers.Parse("p cnf 2 2\n1 2 0\n0\n");
        await Assert.That(f.clauses.Count).IsEqualTo(2);
        await Assert.That(f.HasEmptyClause()).IsTrue();
    }

    [Test]
    public async Task PercentTerminatorStopsParsing()
    {
        var f = Helpers.Parse("p cnf 2 2\n1 2 0\n-1 0\n%\n0\n");
        await Assert.That(f.clauses.Count).IsEqualTo(2);
        await Assert.That(f.HasEmptyClause()).IsFalse();
    }

    [Test]
    public async Task EmptyFormula()
    {
        var f = Helpers.Parse("p cnf 0 0\n");
        await Assert.That(f.IsEmpty()).IsTrue();
        await Assert.That(f.HasEmptyClause()).IsFalse();
        var g = Helpers.Parse("");
        await Assert.That(g.IsEmpty()).IsTrue();
    }

    [Test]
    public async Task MissingFinalZeroIsTolerated()
    {
        var f = Helpers.Parse("p cnf 2 1\n1 2\n");
        await Assert.That(Helpers.Show(f)).IsEqualTo("(1 2)");
    }

    [Test]
    public async Task VariablesAreSharedBetweenLiterals()
    {
        var f = Helpers.Parse("p cnf 2 2\n1 2 0\n-1 0\n");
        var a = f.clauses.SelectMany(c => c.literals).Where(l => l.variable.ID == 1).Select(l => l.variable).ToList();
        await Assert.That(a.Count).IsEqualTo(2);
        await Assert.That(ReferenceEquals(a[0], a[1])).IsTrue();
    }

    [Test]
    public async Task BundledFileParses()
    {
        var f = new Formula(Helpers.CnfFile("quinn.cnf"));
        await Assert.That(f.clauses.Count).IsEqualTo(18);
    }
}
