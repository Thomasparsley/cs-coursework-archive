using System;

// Cesta k dimacs cnf souboru
var files = new string[] {
    //"./cnf/quinn.cnf",
    //"./cnf/formula.cnf",
    //"./cnf/simple_v3_c2.cnf",
};

foreach (var path in files)
{
    string[] allLines = System.IO.File.ReadAllLines(path);

    var hearestics = new IDPLL[] {
        new DLIS(allLines),
        new DLCS(allLines),
        new MOM(allLines),
        new Bohm(allLines),
        new MY(allLines),
    };

    System.Console.WriteLine("path: " + path);
    var formula = new Formula(allLines);
    System.Console.WriteLine(formula.description);

    foreach (var hearestic in hearestics)
    {
        var solvedSimple = Solver.Solve(hearestic);
        System.Console.Write(hearestic.HeuristicName() + ": ");
        System.Console.WriteLine(solvedSimple);
    }

    System.Console.WriteLine();
}
