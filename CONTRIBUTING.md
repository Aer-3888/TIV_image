# Contributing

## Branches

`main` is the stable branch. Do not commit on it directly: work on a branch and open a pull request.

Name a branch `<type>/<short-description>` in lowercase, words separated by `-`:

```
feat/binary-strip
fix/cross-detection
docs/readme-update
chore/repo-cleanup
```

`<type>` is one of the commit types below. Keep branches short-lived and focused on one subject.

## Commits

Use [Conventional Commits](https://www.conventionalcommits.org/), on a single line:

```
<type>: <description>
```

- Lowercase, imperative mood ("add", not "added"), no final period, 72 characters at most.
- One logical change per commit.
- No body and no trailers.

| Type       | Use for                                       |
| ---------- | --------------------------------------------- |
| `feat`     | New feature                                   |
| `fix`      | Bug fix                                       |
| `refactor` | Code change that neither fixes nor adds       |
| `perf`     | Performance improvement                       |
| `docs`     | Documentation only                            |
| `test`     | Adding or fixing tests                        |
| `chore`    | Maintenance, repository layout, ignored files |
| `ci`       | Continuous integration                        |

Examples:

```
feat: detect the two crosses and straighten the pages
fix: handle pages with a missing cross
docs: describe the binary strip in the readme
chore: move pdfs to docs
```

## Pull requests

- Target `main` and keep the title in the same `<type>: <description>` format.
- Describe what changed and how you checked it.
- Do not commit scans, build output (`build/`, `cmake-build-debug/`) or the `NicIcon/` dataset.
