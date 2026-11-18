# TIV image

Group project for the Image and Video Processing course (TIV, 4 INFO, INSA Rennes).

The goal is to build a database of handwritten symbols from scanned forms, then use it to extract features and classify the symbols.

- **Semester 7**: document processing chain that cuts every drawing out of the scanned forms and labels it (ground truth).
- **Semester 8**: feature extraction and classification of the symbols.

## Team

Theo, Thomas, Nedas and Matthis.

## Data

The forms come from the NicIcon dataset. 35 scripters each filled 22 forms, drawing 14 emergency icons (accident, bomb, car, fire, police...) in boxes, five boxes per row. Forms 00 and 01 have no size given. Forms 02 to 21 ask for each icon in a random order and in a given size (small, medium or large).

`docs/images/00000.png` is a sample scan (scripter 000, form 00).

## Output

Every extracted drawing is saved without its box border, as a PNG with a text file next to it describing it (label, scripter, page, row, column, size). Results are grouped by label and size, for example `car/small/` or `car/none/` when no size is given.

## Page alignment

The scans are slightly rotated, shifted or scaled, so the first step is to put each page back straight. Every form has two "+" crosses (top-right and bottom-left). They are found by template matching with `assets/image_plus.png`, then a rotation + scale + translation is computed to bring them to their reference position (`src/alignment.cpp`, `include/alignment.hpp`).

If a cross is missing or unreadable, the page is marked as not processable and listed in `not_processable.txt`.

```bash
mkdir -p build && cd build && cmake .. && make align_pages
./align_pages [folder] [--show]   # default folder: ../NicIcon/all-scans
```

## Binary strip

Each form has a binary code in the header bar that gives the page number, so no OCR is needed. `src/binaryStrip.cpp` crops the bar, binarizes it with Otsu, finds the contours and maps each one to a bit position (first position at 27.5 px, 55.7 px pitch, 15 bits), then prints the bits and their value.

This is still a prototype: the scan path is hardcoded, and it is not yet connected to the page alignment step.

```bash
cd build && cmake .. && make binary_strip && ./binary_strip
```

## Repository

| Path                                       | Content                                  |
| ------------------------------------------ | ---------------------------------------- |
| `src/`, `include/`, `CMakeLists.txt` | C++ / OpenCV processing chain            |
| `review-tool/`                           | Script to check the extracted snippets   |
| `docs/*.pdf`                             | Project subject and course slides        |
| `docs/images/`                           | Sample scan                              |
| `assets/`                                | Template image of the "+" cross          |

## Review tool

The extraction is never perfect, so `review-tool/review.py` goes through the snippets of one label and size folder and asks yes or no for each. The wrong ones are moved to an `unsorted/` folder. See `review-tool/README.md`.

## Contributing

Branch and commit conventions are in [CONTRIBUTING.md](CONTRIBUTING.md).

## License

GPL-3.0. See [LICENSE](LICENSE).
