# Snippet review

A small script to check the snippets extracted from the TIV forms, one folder at a time.

The extraction sorts every snippet into a `<label>/<size>/` folder and will make mistakes. This script shows the snippets of one folder one by one and you answer yes or no. The wrong ones go to an `unsorted/` folder next to the label folders, with their `.txt`.

## Use

It needs Python 3.10+ and Pillow (`pip install pillow`).

```
python review-tool/review.py output/car/small
```

| Key | Action |
|---|---|
| `Y` | correct, next |
| `N` | wrong, move the png and txt to `output/unsorted/`, next |
| `B` | back one snippet, and put it back if it was moved |
| `Esc` | quit |

A snippet already in `unsorted/` under the same name is never overwritten. At the end it prints how many were kept and moved.
