public class FormulaTests
{
    [Test]
    public async Task AssignTrueRemovesSatisfiedClausesAndShrinksOthers()
    {
        var f = Helpers.Parse("1 2 0\n-1 3 0\n2 3 0\n");
        var g = f & Helpers.Lit(1);
        // (1 2) subsumed, (-1 3) -> (3), (2 3) kept
        await Assert.That(Helpers.Show(g)).IsEqualTo("(3) (2 3)");
    }

    [Test]
    public async Task AssignFalseLiteral()
    {
        var f = Helpers.Parse("1 2 0\n-1 3 0\n");
        var g = f & Helpers.Lit(-1);
        await Assert.That(Helpers.Show(g)).IsEqualTo("(2)");
    }

    [Test]
    public async Task AssignCanProduceEmptyClause()
    {
        var f = Helpers.Parse("1 0\n-1 2 0\n");
        var g = f & Helpers.Lit(-1);
        await Assert.That(g.HasEmptyClause()).IsTrue();
    }

    [Test]
    public async Task AssignDoesNotMutateOriginal()
    {
        var f = Helpers.Parse("1 2 0\n-1 3 0\n");
        var before = Helpers.Show(f);
        _ = f & Helpers.Lit(1);
        await Assert.That(Helpers.Show(f)).IsEqualTo(before);
    }

    [Test]
    public async Task CloneIsDeepAndIndependent()
    {
        var f = Helpers.Parse("1 2 0\n-1 3 0\n");
        var c = f.Clone();
        await Assert.That(Helpers.Show(c)).IsEqualTo(Helpers.Show(f));

        c.clauses[0].literals.Clear();
        c.clauses.RemoveAt(1);

        await Assert.That(Helpers.Show(f)).IsEqualTo("(1 2) (-1 3)");
        await Assert.That(ReferenceEquals(f.clauses[0], c.clauses[0])).IsFalse();
    }

    [Test]
    public async Task UnitSubsumptionRemovesClausesContainingLiteral()
    {
        var f = Helpers.Parse("1 2 0\n-1 3 0\n1 -2 0\n");
        DPLL.UnitSubsumpion(f, Helpers.Lit(1));
        await Assert.That(Helpers.Show(f)).IsEqualTo("(-1 3)");
    }

    [Test]
    public async Task UnitResolutionRemovesNegatedLiteral()
    {
        var f = Helpers.Parse("-1 2 0\n1 3 0\n-1 0\n");
        DPLL.UnitResolution(f, Helpers.Lit(1));
        await Assert.That(Helpers.Show(f)).IsEqualTo("(2) (1 3) ()");
    }

    [Test]
    public async Task FindUnitClauseLiteral()
    {
        var f = Helpers.Parse("1 2 0\n-3 0\n2 3 0\n");
        var u = f.FindUnitClauseLiteral();
        await Assert.That(u.IsSome()).IsTrue();
        await Assert.That(u.Get().Equals(Helpers.Lit(-3))).IsTrue();

        var g = Helpers.Parse("1 2 0\n-1 -2 0\n");
        await Assert.That(g.FindUnitClauseLiteral().IsNone()).IsTrue();
    }

    [Test]
    public async Task FindPureLiteral()
    {
        var f = Helpers.Parse("1 2 0\n-1 2 0\n-1 3 0\n");
        var p = f.FindPureLiteral();
        await Assert.That(p.IsSome()).IsTrue();
        var lit = p.Get();
        // 2 and 3 are pure (positive only)
        await Assert.That(lit.isTrue).IsTrue();
        await Assert.That(lit.variable.ID == 2 || lit.variable.ID == 3).IsTrue();
    }

    [Test]
    public async Task NoPureLiteral()
    {
        var f = Helpers.Parse("1 2 0\n-1 -2 0\n");
        await Assert.That(f.FindPureLiteral().IsNone()).IsTrue();
    }

    [Test]
    public async Task LiteralNegationAndEquality()
    {
        var a = Helpers.Lit(3);
        await Assert.That((-a).isTrue).IsFalse();
        await Assert.That((-(-a)).Equals(a)).IsTrue();
        await Assert.That(a.Equals(Helpers.Lit(-3))).IsFalse();
        await Assert.That(a.GetHashCode()).IsEqualTo(Helpers.Lit(3).GetHashCode());
    }

    [Test]
    public async Task OccurrenceCounts()
    {
        var f = Helpers.Parse("1 2 0\n1 -2 0\n-1 2 0\n");
        var occ = f.NumberOfOccurrencesOfLiterals();
        await Assert.That(occ[Helpers.Lit(1)]).IsEqualTo(2);
        await Assert.That(occ[Helpers.Lit(-1)]).IsEqualTo(1);
        await Assert.That(occ[Helpers.Lit(2)]).IsEqualTo(2);
        await Assert.That(occ[Helpers.Lit(-2)]).IsEqualTo(1);
    }

    [Test]
    public async Task ShortestClause()
    {
        var f = Helpers.Parse("1 2 3 0\n-1 2 0\n3 4 5 0\n");
        await Assert.That(f.ShortestClause().literals.Count).IsEqualTo(2);
    }
}
