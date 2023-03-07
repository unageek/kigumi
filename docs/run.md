# Running example programs

```
./run r examples/mix -- --first a.obj --second b.obj --out mixed
```

The intermediate data is saved to the file `mixed` in the current directory. To apply a Boolean operator and extract the result, run the program `extract`.

```
./run r examples/extract -- --in mixed --out out.obj --op uni
```
