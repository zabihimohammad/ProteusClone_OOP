import os

# آدرس پوشه‌ی اصلی پروژه‌تان در ویندوز
project_folder = r"E:\6th_Semester\Objective_Programming\new\ProteusClone_OOP"

# آدرس پوشه src درون پروژه
source_folder = os.path.join(project_folder, "src")

# آدرس فایل خروجی روی دسکتاپ
output_filename = r"C:\Users\ADMIN\Desktop\output_proj.txt"

valid_extensions = (".cpp", ".h")

# بررسی وجود داشتن پوشه src قبل از شروع
if not os.path.exists(source_folder):
    print(f"Error: Could not find folder at '{source_folder}'")
else:
    file_count = 0
    with open(output_filename, "w", encoding="utf-8") as outfile:
        for root, dirs, files in os.walk(source_folder):
            for file in files:
                if file.endswith(valid_extensions):
                    filepath = os.path.join(root, file)
                    
                    outfile.write(f"\n{'='*60}\n")
                    outfile.write(f"FILE: {filepath}\n")
                    outfile.write(f"{'='*60}\n\n")
                    
                    try:
                        with open(filepath, "r", encoding="utf-8") as infile:
                            outfile.write(infile.read())
                            outfile.write("\n")
                            file_count += 1
                    except Exception as e:
                        outfile.write(f"Error reading file: {e}\n")

    print(f"Done! Merged {file_count} files into '{output_filename}'")