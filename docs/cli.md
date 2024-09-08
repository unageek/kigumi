# Using kigumi CLI

Once you have followed the build instructions, a program named `kigumi` (or `kigumi.exe` on Windows) is created under
`build/cli`. Here is the usage of the program:

```
usage: kigumi <command> [<args>]

Commands:
  boolean       apply boolean operations to two meshes
  convert       convert between mesh formats
  defects       find defects in a mesh
```

## kigumi boolean

```
usage: kigumi boolean [--first] (<file> | :empty: | :full:)
                      [--second] (<file> | :empty: | :full:)
                      [--int <file>] [--uni <file>] [--dif <file>]
                      [--sym <file>]

Options:
  --first (<file> | :empty: | :full:)
                              the first input mesh
  --second (<file> | :empty: | :full:)
                              the second input mesh
  --int <file>                output the intersection of the two meshes
  --uni <file>                output the union of the two meshes
  --dif <file>                output the difference of the two meshes
  --sym <file>                output the symmetric difference of the two meshes
```

## kigumi convert

```
usage: kigumi convert [--in] <file> [--out] <file>

Options:
  --in <file>           the input mesh
  --out <file>          the output mesh
```

## kigumi defects

```
usage: kigumi defects [--in] <file>

Options:
  --in <file>           the input mesh
```
