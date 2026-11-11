using System;

/*
Třída Tree reprezentuje RedBlackTree.
Operace:
    - Insert: vložení nového uzlu (uzel nebo klíč)
    - Delete: odstranění uzlu (uzel nebo klíč)
    - Find: podle klíče najde uzel
    - Size: vrátí počet uzlů
    - Print: vypíše strom (při výpisu, uzel který má červenou barvu tak text má červenou barvu)

Třída Node reprezentuje uzel RedBlackTree.
Operace:
    - Grandparent: vrátí rodiči uzel rodiče
    - NumberOfChildrens: vrátí počet potomků uzlu
    - IsLeftChild: zjistí, zda je uzel levým potomkem
    - IsRightChild: zjistí, zda je uzel pravým potomkem
    - Print: vypíše uzel a jeho potomky (při výpisu, uzel který má červenou barvu tak text má červenou barvu)

Konstanta:
    - NilLeaf reprezentuje černý list. Každý černý list má klíč Int32.MinValue.

Enum Color reprezentuje barvu uzlu.

5  <-- Kořen stromu
├─9  <-- Pravý potomek
| ├─15
| | ├─25
| | | ├─30
| | | | ├─BLACK LEAF
| | | | └─BLACK LEAF
| | | └─20
| | |   ├─BLACK LEAF
| | |   └─BLACK LEAF
| | └─11
| |   ├─13
| |   | ├─14
| |   | | ├─BLACK LEAF
| |   | | └─BLACK LEAF
| |   | └─12
| |   |   ├─BLACK LEAF
| |   |   └─BLACK LEAF
| |   └─10
| |     ├─BLACK LEAF
| |     └─BLACK LEAF
| └─7
|   ├─8
|   | ├─BLACK LEAF
|   | └─BLACK LEAF
|   └─6
|     ├─BLACK LEAF
|     └─BLACK LEAF
└─3 <-- Levý potomek
  ├─4
  | ├─BLACK LEAF
  | └─BLACK LEAF
  └─1
    ├─2
    | ├─BLACK LEAF
    | └─BLACK LEAF
    └─BLACK LEAF
*/


var t1 = new Tree();
var t1Keys = new int[] { 5, 3, 7, 1, 4, 6, 8, 2, 9, 10, 20, 15, 30, 25, 11, 12, 13, 14 };

foreach (var key in t1Keys)
    t1.Insert(key);
    
t1.Print();
var t1KeysDel = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 20, 25 };
foreach (var key in t1KeysDel)
{
    System.Console.WriteLine();
    System.Console.WriteLine($"Deleting {key}");
    var node = t1.Delete(key);
    if (node.IsNone())
        throw new System.Exception($"Key not found, key: {key} or root is none");
    t1.Print();
}
