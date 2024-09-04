# Using kigumi CLI

Once you have followed the build instructions, a program named `kigumi` (or `kigumi.exe` on Windows) is created under
`build/cli`. Here is the usage of the program:

```
usage: kigumi <command> [<args>]

Commands:
  boolean       apply boolean operations to two meshes
  convert       convert between mesh formats
```

```
usage: kigumi boolean [--first] <file> [--second] <file> [--int <file>]
                      [--uni <file>] [--dif <file>] [--sym <file>]

Options:
  --first <file>        the first input mesh
  --second <file>       the second input mesh
  --int <file>          output the intersection of the two meshes
  --uni <file>          output the union of the two meshes
  --dif <file>          output the difference of the two meshes
  --sym <file>          output the symmetric difference of the two meshes
```

```
usage: kigumi convert [--from] <file> [--to] <file>

Options:
  --from <file>         the input mesh
  --to <file>           the output mesh
```
