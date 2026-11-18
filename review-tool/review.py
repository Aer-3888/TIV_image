"""Check the snippets of one extraction folder, one image at a time.

Y keeps the snippet, N moves its png and txt to <output>/unsorted, B goes back
and undoes the last answer, Esc quits.
"""

import argparse
import shutil
import tkinter as tk
from pathlib import Path

from PIL import Image, ImageTk

VIEW = 400


def move_files(files: list[Path], dest: Path) -> list[Path]:
    dest.mkdir(parents=True, exist_ok=True)
    targets = [dest / f.name for f in files]
    for target in targets:
        if target.exists():
            raise FileExistsError(f"{target.name} is already in {dest.name}/")
    for src, target in zip(files, targets):
        shutil.move(src, target)
    return targets


class Review:
    def __init__(self, pngs: list[Path], unsorted: Path, title: str):
        self.pngs = pngs
        self.unsorted = unsorted
        self.answers: list[str | None] = [None] * len(pngs)
        self.moved: dict[int, list[Path]] = {}
        self.pos = 0

        self.window = tk.Tk()
        self.window.title(title)
        self.window.resizable(False, False)
        self.canvas = tk.Canvas(self.window, width=VIEW, height=VIEW, bg="white", highlightthickness=0)
        self.canvas.pack(padx=12, pady=12)
        self.info = tk.Label(self.window, font=("TkDefaultFont", 12))
        self.info.pack()
        self.status = tk.Label(self.window)
        self.status.pack(pady=(4, 12))
        self.window.bind("<Key>", self.on_key)
        self.window.protocol("WM_DELETE_WINDOW", self.window.destroy)
        self.show()

    def on_key(self, event: tk.Event) -> None:
        actions = {"y": self.yes, "n": self.no, "b": self.back, "escape": self.window.destroy}
        action = actions.get(event.keysym.lower())
        if action:
            action()

    def yes(self) -> None:
        if self.pos < len(self.pngs):
            self.answers[self.pos] = "yes"
            self.pos += 1
            self.show()

    def no(self) -> None:
        if self.pos == len(self.pngs):
            return
        png = self.pngs[self.pos]
        files = [p for p in (png, png.with_suffix(".txt")) if p.exists()]
        try:
            self.moved[self.pos] = move_files(files, self.unsorted)
        except OSError as err:
            self.error(str(err))
            return
        self.answers[self.pos] = "no"
        self.pos += 1
        self.show()

    def back(self) -> None:
        if self.pos == 0:
            return
        prev = self.pos - 1
        if self.answers[prev] == "no":
            try:
                move_files(self.moved[prev], self.pngs[prev].parent)
            except OSError as err:
                self.error(str(err))
                return
            del self.moved[prev]
        self.answers[prev] = None
        self.pos = prev
        self.show()

    def show(self) -> None:
        self.canvas.delete("all")
        self.status.config(fg="gray30")
        total = len(self.pngs)
        if self.pos == total:
            self.info.config(text=f"Done: {self.summary()}")
            self.status.config(text="B back    Esc quit")
            return
        png = self.pngs[self.pos]
        with Image.open(png) as img:
            img = img.convert("RGB")
        scale = min(VIEW / img.width, VIEW / img.height)
        size = (max(1, round(img.width * scale)), max(1, round(img.height * scale)))
        self.photo = ImageTk.PhotoImage(img.resize(size, Image.Resampling.LANCZOS))
        self.canvas.create_image(VIEW // 2, VIEW // 2, image=self.photo)
        self.info.config(text=f"{png.stem}    {self.pos + 1}/{total}")
        self.status.config(text="Y yes    N no, to unsorted    B back    Esc quit")

    def error(self, message: str) -> None:
        self.status.config(text=message, fg="red")

    def summary(self) -> str:
        return f"{self.answers.count('yes')} yes, {self.answers.count('no')} moved to unsorted"


def main() -> None:
    parser = argparse.ArgumentParser(description="Check the snippets of one <label>/<size> folder.")
    parser.add_argument("folder", type=Path, help="a <label>/<size> folder of the extraction output")
    parser.add_argument("--unsorted", type=Path, help="where wrong snippets go (default: <output>/unsorted)")
    args = parser.parse_args()

    folder = args.folder.resolve()
    if not folder.is_dir():
        parser.error(f"folder not found: {args.folder}")
    unsorted = (args.unsorted or folder.parent.parent / "unsorted").resolve()
    if unsorted == folder:
        parser.error("this folder is the unsorted folder")
    pngs = sorted(folder.glob("*.png"))
    if not pngs:
        parser.error(f"no png in {args.folder}")

    review = Review(pngs, unsorted, f"{folder.parent.name}/{folder.name}")
    review.window.mainloop()
    print(review.summary())


if __name__ == "__main__":
    main()
