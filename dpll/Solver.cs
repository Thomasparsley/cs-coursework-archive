class Solver
{
    public static (bool, long) Solve<T>(T solver) where T : IDPLL => solver.Solve();
}
