public class HeuristicTests
{
    const string Cnf = "p cnf 4 6\n1 2 0\n1 2 0\n1 3 0\n-1 4 0\n-2 -3 -4 0\n-4 -3 0\n";

    [Test]
    public async Task DlisChoosesVariable1()
    {
        var lit = Helpers.Choose(typeof(DLIS), Helpers.Parse(Cnf));
        await Assert.That(lit.variable.ID).IsEqualTo(1);
    }

    [Test]
    public async Task DlcsChoosesVariable1()
    {
        var lit = Helpers.Choose(typeof(DLCS), Helpers.Parse(Cnf));
        await Assert.That(lit.variable.ID).IsEqualTo(1);
    }

    [Test]
    public async Task MomChoosesVariable1()
    {
        var lit = Helpers.Choose(typeof(MOM), Helpers.Parse(Cnf));
        await Assert.That(lit.variable.ID).IsEqualTo(1);
    }

    [Test]
    public async Task BohmChoosesVariable1()
    {
        var lit = Helpers.Choose(typeof(Bohm), Helpers.Parse(Cnf));
        await Assert.That(lit.variable.ID).IsEqualTo(1);
    }

    [Test]
    public async Task MyChoosesPositiveLiteral()
    {
        var lit = Helpers.Choose(typeof(MY), Helpers.Parse(Cnf));
        await Assert.That(lit.isTrue).IsTrue();
    }
}
