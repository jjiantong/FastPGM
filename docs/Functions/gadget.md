# gadget.h

## set<Combination> GenAllCombFromSets(set<Combination> \*)
A recursive function generate all possible combinations from several sets. <br/>

Input: several sets (set of sets) <br/>
Output: all possible combinations of elements of each set, by picking one element in each set. <br/>

Example (pseudocode)
```
A = {1, 3, 5}
B = {a, b, c}
set_of_sets = {A, B}
result = GenAllCombFromSets(set_of_sets)
print(result)
```
Output:
```
{1a, 1b, 1c, 3a, 3b, 3c, 5a, 5b, 5c}
```

## bool EachFirstIsInSecond(Combination \*first, Combination \*second)
A function that is used when multipling two factors. <br/>
If each element in the first Combination is in the second Combination, return true. <br/>

Input: two `Combination`s <br/>
Output: bool <br/>

Example (pseudocode)
```
fir_1 = { <a,1> , <b,2> }
sec_1 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_2 = { <a,0> , <b,2> }
sec_2 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_3 = { <a,1> , <b,2> , <c,3> }
sec_3 = { <a,1> , <b,2> }

print( EachFirstIsInSecond(fir_1, sec_1),
       EachFirstIsInSecond(fir_2, sec_2),
       EachFirstIsInSecond(fir_3, sec_3) )
```
Output
```
truefalsefalse
```

## bool FirstCompatibleSecond(Combination \*first, Combination \*second)
A function that is used when multipling two factors. <br/>
If elements in the first Combination is compatible with the second Combination, return true. <br/>

Input: two `Combination`s <br/>
Output: bool <br/>

Example (pseudocode)
```
  f1={ <a,1> , <b,2> }
  s1={ <a,1> , <b,2> , <c,3> , whatever more... }

  f2={ <a,0> , <b,2> }
  s2={ <a,1> , <b,2> , <c,3> , whatever more... }

  f3={ <a,1> , <b,2> , <c,3> }
  s3={ <a,1> , <b,2> }

  f4={ <a,1> , <b,2> , <c,3> }
  s4={ <a,1> , <b,2> , <d,4> }

  f5={ <a,1> , <b,2> , <c,3> }
  s5={ <a,1> , <b,2> , <c,4> }

  f6={ <a,1> , <b,2> }
  s6={ <c,3> , <d,4> }

  print( EachFirstIsInSecond(f1, s1),
         EachFirstIsInSecond(f2, s2),
         EachFirstIsInSecond(f3, s3),
         EachFirstIsInSecond(f4, s4),
         EachFirstIsInSecond(f5, s5),
         EachFirstIsInSecond(f6, s6), )

```
Output
```
truefalsetruetruefalsetrue
```