# Table of Boolean operators

All 16 Boolean operators are supported:

| `Boolean_operator::`           | Set notation       | Venn diagram                         |
| ------------------------------ | ------------------ | ------------------------------------ |
| `V`                            | $U$, the universe  | <img width="100" src="Venn1111.png"> |
| `A`,<br>`UNION`                | $A ∪ B$            | <img width="100" src="Venn0111.png"> |
| `B`                            | $(B ⧵ A)^c$        | <img width="100" src="Venn1101.png"> |
| `C`                            | $(A ⧵ B)^c$        | <img width="100" src="Venn1011.png"> |
| `D`                            | $(A ∩ B)^c$        | <img width="100" src="Venn1110.png"> |
| `E`                            | $(A △ B)^c$        | <img width="100" src="Venn1001.png"> |
| `F`                            | $A^c$              | <img width="100" src="Venn1010.png"> |
| `G`                            | $B^c$              | <img width="100" src="Venn1100.png"> |
| `H`                            | $B$                | <img width="100" src="Venn0011.png"> |
| `I`                            | $A$                | <img width="100" src="Venn0101.png"> |
| `J`,<br>`SYMMETRIC_DIFFERENCE` | $A △ B$            | <img width="100" src="Venn0110.png"> |
| `K`,<br>`INTERSECTION`         | $A ∩ B$            | <img width="100" src="Venn0001.png"> |
| `L`,<br>`DIFFERENCE`           | $A ⧵ B$            | <img width="100" src="Venn0100.png"> |
| `M`                            | $B ⧵ A$            | <img width="100" src="Venn0010.png"> |
| `X`                            | $(A ∪ B)^c$        | <img width="100" src="Venn1000.png"> |
| `O`                            | $∅$, the empty set | <img width="100" src="Venn0000.png"> |

## Notes

- The result of each Boolean operation is regularized, i.e., the interior of the result is taken first, followed by its closure.
- The alphabets are the Bocheński notation.
