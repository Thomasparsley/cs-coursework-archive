package main

import (
	"errors"
)

type node struct {
	parent    *node   // rodič
	T         int     //
	length    int     //
	keys      []int   // klíč
	childrens []*node // potomci
	isLeaf    bool    // jsem listem
}

func NewBtreeNode(t int, isLeaf bool) *node {
	return &node{
		T:         t,
		length:    0,
		keys:      make([]int, 2*t-1),
		childrens: make([]*node, 2*t),
		isLeaf:    isLeaf,
	}
}

func (this *node) GetFirstKey() int {
	if this.length == 0 {
		return -1
	} else {
		return this.keys[0]
	}
}

func (this *node) GetFirstKeyIndex() int {

	if this.length == 0 {
		return -1
	} else {
		return 0
	}
}

func (this *node) GetLastKey() int {
	if this.length == 0 {
		return -1
	} else {
		return this.keys[this.length-1]
	}
}

func (this *node) GetLastKeyIndex() int {

	if this.length == 0 {
		return -1
	} else {
		return this.length - 1
	}
}

func (this *node) GetLastChildrenIndex() int {
	for i := len(this.childrens) - 1; i >= 0; i-- {
		if this.childrens[i] != nil {
			return i
		}
	}

	return -1
}

func (this *node) GetLastChildren() *node {
	for i := len(this.childrens) - 1; i >= 0; i-- {
		if this.childrens[i] != nil {
			return this.childrens[i]
		}
	}

	return nil
}

func (this *node) FindMaxKey() (int, int) {
	if this.isLeaf {
		return this.GetLastKey(), this.GetLastKeyIndex()
	} else {
		return this.GetLastChildren().FindMaxKey()
	}
}

func (this *node) InsertInOrder(key int) {
	i := this.length - 1

	for i >= 0 && key < this.keys[i] {
		this.keys[i+1] = this.keys[i]
		i--
	}

	this.keys[i+1] = key
	this.length++
}

func (this *node) InsertNonFull(key int) {
	if this.isLeaf {
		this.InsertInOrder(key)
	} else {
		i := this.length - 1

		for i >= 0 && this.keys[i] > key {
			i--
		}

		i++

		if this.childrens[i].length == 2*this.T-1 {
			this.SplitChild(i)

			if key > this.keys[i] {
				i++
			}
		}

		this.childrens[i].InsertNonFull(key)
	}
}

func (this *node) SplitChild(index int) {
	y := this.childrens[index]
	z := NewBtreeNode(this.T, y.isLeaf)

	z.parent = this
	z.length = this.T - 1
	y.length = this.T - 1

	// 1.
	// Zkopírování pravé částí klíčů do nového uzlu
	for j := 0; j < this.T-1; j++ {
		z.keys[j] = y.keys[j+this.T]
		y.keys[j+this.T] = 0
	}

	if !y.isLeaf {
		// Zkopírování pravé částí potomků do nového uzlu
		for j := 0; j < this.T; j++ {
			z.childrens[j] = y.childrens[j+this.T]
			y.childrens[j+this.T] = nil

			// Potomci v novém uzlu budou mít správného rodiče
			z.childrens[j].parent = z
		}
	}

	// 2.
	// Posuneme klíče v aktuálním uzlu o 1 doprava
	for j := this.length - 1; j >= index; j-- {
		this.keys[j+1] = this.keys[j]
	}

	// Posuneme potomky v aktuálním uzlu o 1 doprava
	for j := this.length; j >= index+1; j-- {
		this.childrens[j+1] = this.childrens[j]
	}

	// 3.
	// Do aktuálního uzlu zapojíme nový uzel
	this.childrens[index+1] = z
	this.keys[index] = y.keys[this.T-1]
	y.keys[this.T-1] = 0
	this.length++
}

func (this *node) Search(key int) (*node, int) {
	i := 0

	for i < this.length && key > this.keys[i] {
		i++
	}

	if i < this.length && key == this.keys[i] {
		return this, i
	}

	if this.isLeaf {
		return nil, -1
	}

	return this.childrens[i].Search(key)
}

// Spojí uzel se sourozencem (levým nebo pravým) a separátorem z rodiče.
// Levý z dvojice uzlů přežije, pravý zaniká.
func (this *node) MargeTwoNodes(leftSibling bool, thisNodeIndex int) {
	var leftIndex int

	if leftSibling {
		leftIndex = thisNodeIndex - 1
	} else {
		leftIndex = thisNodeIndex
	}

	left := this.parent.childrens[leftIndex]
	right := this.parent.childrens[leftIndex+1]

	// Separátor z rodiče
	left.keys[left.length] = this.parent.keys[leftIndex]

	// Klíče pravého uzlu
	for i := 0; i < right.length; i++ {
		left.keys[left.length+1+i] = right.keys[i]
	}

	// Potomci pravého uzlu
	if !left.isLeaf {
		for i := 0; i <= right.length; i++ {
			left.childrens[left.length+1+i] = right.childrens[i]
			left.childrens[left.length+1+i].parent = left
		}
	}

	left.length += right.length + 1

	// Odebrání separátoru a pravého uzlu z rodiče
	for i := leftIndex; i < this.parent.length-1; i++ {
		this.parent.keys[i] = this.parent.keys[i+1]
	}
	this.parent.keys[this.parent.length-1] = 0

	for i := leftIndex + 1; i < this.parent.length; i++ {
		this.parent.childrens[i] = this.parent.childrens[i+1]
	}
	this.parent.childrens[this.parent.length] = nil

	this.parent.length--
}

// Smaže klíč na pozici index a zbalancuje strom směrem ke kořeni.
func (this *node) Delete(index int) {
	this.DeletePhaseOne(index).DeletePhaseTwo()
}

// Odstraní klíč a vrátí list, ve kterém došlo k fyzickému odebrání.
func (this *node) DeletePhaseOne(index int) *node {
	if this.isLeaf {
		return this.DeleteFromLeaf(index)
	} else {
		return this.DeleteFromNoLeaf(index)
	}
}

func (this *node) DeleteFromLeaf(index int) *node {
	for i := index; i < this.length-1; i++ {
		this.keys[i] = this.keys[i+1]
	}

	this.length--
	this.keys[this.length] = 0

	return this
}

func (this *node) DeleteFromNoLeaf(index int) *node {
	// Klíč nahradíme předchůdcem (maximum levého podstromu)
	maxNode := this.childrens[index]

	for !maxNode.isLeaf {
		maxNode = maxNode.childrens[maxNode.length]
	}

	this.keys[index] = maxNode.GetLastKey()

	return maxNode.DeleteFromLeaf(maxNode.GetLastKeyIndex())
}

// Přesune klíč ze sourozence přes rodiče do tohoto uzlu.
func (this *node) MoveKeyOverParent(leftSibling bool, thisNodeIndex int) {
	if leftSibling {
		sibling := this.parent.childrens[thisNodeIndex-1]
		keyIndex := thisNodeIndex - 1

		for i := this.length; i > 0; i-- {
			this.keys[i] = this.keys[i-1]
		}

		this.keys[0] = this.parent.keys[keyIndex]

		if !this.isLeaf {
			for i := this.length + 1; i > 0; i-- {
				this.childrens[i] = this.childrens[i-1]
			}

			// Poslední potomek sourozence přejde na začátek
			this.childrens[0] = sibling.childrens[sibling.length]
			this.childrens[0].parent = this
			sibling.childrens[sibling.length] = nil
		}

		this.parent.keys[keyIndex] = sibling.keys[sibling.length-1]
		sibling.keys[sibling.length-1] = 0
		sibling.length--
		this.length++
	} else {
		sibling := this.parent.childrens[thisNodeIndex+1]
		keyIndex := thisNodeIndex

		this.keys[this.length] = this.parent.keys[keyIndex]

		if !this.isLeaf {
			// První potomek sourozence přejde na konec
			this.childrens[this.length+1] = sibling.childrens[0]
			this.childrens[this.length+1].parent = this
		}

		this.parent.keys[keyIndex] = sibling.keys[0]

		for i := 0; i < sibling.length-1; i++ {
			sibling.keys[i] = sibling.keys[i+1]
		}
		sibling.keys[sibling.length-1] = 0

		if !sibling.isLeaf {
			for i := 0; i < sibling.length; i++ {
				sibling.childrens[i] = sibling.childrens[i+1]
			}
			sibling.childrens[sibling.length] = nil
		}

		sibling.length--
		this.length++
	}
}

func (this *node) IndexOf() int {
	for i := 0; i <= this.parent.length; i++ {
		if this.parent.childrens[i] == this {
			return i
		}
	}

	return -1
}

func (this *node) DeletePhaseTwo() {
	// 1.
	// Kořen může mít libovolný počet klíčů
	if this.parent == nil || this.length >= this.T-1 {
		return
	}

	thisNodeIndex := this.IndexOf()

	// 2.
	// Levý sourozenec
	if thisNodeIndex > 0 && this.parent.childrens[thisNodeIndex-1].length > this.T-1 {
		this.MoveKeyOverParent(true, thisNodeIndex)
		return
	}

	// Pravý sourozenec
	if thisNodeIndex < this.parent.length && this.parent.childrens[thisNodeIndex+1].length > this.T-1 {
		this.MoveKeyOverParent(false, thisNodeIndex)
		return
	}

	// 3.
	// Pravý sourozenec
	if thisNodeIndex < this.parent.length {
		this.MargeTwoNodes(false, thisNodeIndex)
	} else {
		// Levý sourozenec
		this.MargeTwoNodes(true, thisNodeIndex)
	}

	this.parent.DeletePhaseTwo()
}

type tree struct {
	T    int   //
	root *node // kořenový uzel
}

func NewBtree(t int) *tree {
	if t < 2 {
		panic(errors.New("t cannot be less than 2"))
	}

	return &tree{
		T:    t,
		root: NewBtreeNode(t, true),
	}
}

func (this *tree) Insert(key int) {
	root := this.root

	if root.length == 2*this.T-1 {
		var newNode = NewBtreeNode(this.T, false)
		newNode.childrens[0] = root
		this.root = newNode
		root.parent = newNode

		newNode.SplitChild(0)
		newNode.InsertNonFull(key)
	} else {
		root.InsertNonFull(key)
	}
}

func (this *tree) Search(key int) (*node, int) {
	return this.root.Search(key)
}

func (this *tree) FindMaxKey() (int, int) {
	return this.root.FindMaxKey()
}

func (this *tree) Delete(key int) int {
	node, index := this.Search(key)

	// Node not found
	if node == nil {
		return -1
	}

	node.Delete(index)

	// Kořen bez klíčů a s jedním potomkem se zruší
	if this.root.length == 0 && !this.root.isLeaf {
		this.root = this.root.childrens[0]
		this.root.parent = nil
	}

	return key
}
