# TIV image

Group project for the Image and Video Processing course (TIV, 4 INFO, INSA Rennes).

The goal is to build a database of handwritten symbols from scanned forms, then use it to extract features and classify the symbols.

- **Semester 7**: document processing chain that cuts every drawing out of the scanned forms and labels it (ground truth).
- **Semester 8**: feature extraction and classification of the symbols.

## Team

Theo, Thomas, Nedas and Matthis.

## Data

The forms come from the NicIcon dataset. 35 scripters each filled 22 forms, drawing 14 emergency icons (accident, bomb, car, fire, police...) in boxes, five boxes per row. Forms 00 and 01 have no size given. Forms 02 to 21 ask for each icon in a random order and in a given size (small, medium or large).

`00000.png` is a sample scan (scripter 000, form 00).

## Output

Every extracted drawing is saved without its box border, as a PNG with a text file next to it describing it (label, scripter, page, row, column, size). Results are grouped by label and size, for example `car/small/` or `car/none/` when no size is given.

## Repository

| Path                                       | Content                                  |
| ------------------------------------------ | ---------------------------------------- |
| `src/`, `include/`, `CMakeLists.txt` | C++ / OpenCV processing chain            |
| `*.pdf`                                  | Project subject and course slides        |

## License

GPL-3.0. See [LICENSE](LICENSE).
