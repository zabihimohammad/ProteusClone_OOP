from pathlib import Path


# مسیرهای پروژه
project_folder = Path(__file__).resolve().parent
source_folder = project_folder / "src"
output_file = project_folder / "all_project_code.txt"


def source_files():
    # فایل‌های C++ را مرتب برگردان.
    files = list(source_folder.rglob("*.cpp"))
    files.extend(source_folder.rglob("*.h"))
    return sorted(files, key=lambda path: str(path.relative_to(project_folder)).lower())


def combine_sources():
    # همه سورس‌ها را در یک فایل جمع کن.
    with output_file.open("w", encoding="utf-8") as output:
        for path in source_files():
            relative_path = path.relative_to(project_folder)
            output.write(f"\n{'=' * 60}\n")
            output.write(f"FILE: {relative_path}\n")
            output.write(f"{'=' * 60}\n\n")
            output.write(path.read_text(encoding="utf-8"))
            output.write("\n")

    print(f"Combined source saved to: {output_file}")


if __name__ == "__main__":
    combine_sources()
